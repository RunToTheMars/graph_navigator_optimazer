#include "thread_pool.h"
#include "cassert"

namespace parallel
{

thread_pool::thread_t::thread_t (thread_pool *pool, int timeout_ms, uid_type uid, std::function<void()> &&task)
{
    m_data = std::make_unique<thread_t_data> ();
    m_data->m_pool = pool;
    m_data->m_uid = uid;
    m_data->m_timeout_ms = timeout_ms;
    set_task_unsafe (std::move (task));
    m_data->m_terminated = false;

    m_worker = std::make_unique<std::thread> ([data_ptr = m_data.get ()] ()
    {
        for (;;)
          {
            bool delete_thread = false;
            {
              std::unique_lock<std::mutex> lock (data_ptr->m_mutex);

              bool is_timeout;
              bool no_timeout = false;

              do
                {
                  no_timeout = data_ptr->m_timeout_ms < 0;
                  if (!no_timeout)
                    {
                      auto now = std::chrono::system_clock::now ();
                      is_timeout = !data_ptr->m_task_condition.wait_until (lock, now + std::chrono::milliseconds (data_ptr->m_timeout_ms), [data_ptr] {
                        return data_ptr->m_terminated || data_ptr->m_task || data_ptr->m_timeout_ms < 0;
                      });
                    }
                  else
                    {
                      data_ptr->m_task_condition.wait (lock, [data_ptr] { return data_ptr->m_terminated || data_ptr->m_task || data_ptr->m_timeout_ms >= 0; });
                      is_timeout = false;
                    }
                  // for case when timeout change sign in runtime
                } while (no_timeout != (data_ptr->m_timeout_ms < 0));

              if (is_timeout || (data_ptr->m_terminated && !data_ptr->m_task))
                {
                  delete_thread = true;
                  // for skiping in send task method
                  data_ptr->m_terminated = true;
                }
            }

            if (delete_thread)
              {
                data_ptr->m_pool->delete_thread (data_ptr->m_uid);
                return;
              }

            data_ptr->do_task ();
          }
      });

  m_worker->detach ();
}

thread_pool::thread_t::thread_t (thread_pool::thread_t &&thread)
{
  (*this) = std::move (thread);
}

thread_pool::thread_t::~thread_t ()
{
  if (!m_data)
    return;

  {
    std::unique_lock<std::mutex> lock (m_data->m_mutex);
    assert (!m_data->m_has_task);
    m_data->m_terminated = true;
  }

  m_data->m_task_condition.notify_one ();
}

void thread_pool::thread_t::thread_t_data::do_task ()
{
  std::function<void ()> task;
  {
    std::unique_lock<std::mutex> lock (m_mutex);
    assert (m_task);
    if (!m_task)
      return;

    task = std::move (*m_task);
    m_task.reset ();
  }

  task ();

  {
    std::unique_lock<std::mutex> lock (m_mutex);
    m_has_task = false;
  }

  m_pool->m_wait_condition.notify_all ();
}

thread_pool::thread_t &thread_pool::thread_t::operator = (thread_t &&thread)
{
  std::unique_lock<std::mutex> lock (thread.m_data->m_mutex);

  m_data = std::move (thread.m_data);

  dynamic_cast<parent_t&>(*this) = std::move (dynamic_cast<parent_t&>(thread));
  return *this;
}

bool thread_pool::thread_t::has_task ()
{
  std::unique_lock<std::mutex> lock (m_data->m_mutex);
  return (bool) m_data->m_has_task;
}

bool thread_pool::thread_t::has_task_unsafe ()
{
  return (bool) m_data->m_has_task;
}

bool thread_pool::thread_t::is_ready_for_task_unsafe ()
{
  return !m_data->m_terminated && !m_data->m_has_task;
}

void thread_pool::thread_t::terminate ()
{
  {
    std::unique_lock<std::mutex> lock (m_data->m_mutex);
    m_data->m_terminated = true;
  }

  m_data->m_task_condition.notify_one ();
}

void thread_pool::thread_t::set_timeout (int timeout_ms)
{
  {
    std::unique_lock<std::mutex> lock (m_data->m_mutex);
    m_data->m_timeout_ms = timeout_ms;
  }

  m_data->m_task_condition.notify_one ();
}

void thread_pool::thread_t::set_task_unsafe (std::function<void ()> &&task)
{
  m_data->m_task = std::make_unique<std::function<void ()>> (std::move (task));
  m_data->m_has_task = true;
}

bool thread_pool::thread_t::try_set_task (std::function<void ()> &&task)
{
  bool setted = false;
  {
    std::unique_lock<std::mutex> lock (m_data->m_mutex);
    if (is_ready_for_task_unsafe ())
      {
        set_task_unsafe (std::move (task));
        setted = true;
      }
  }
  if (setted)
    m_data->m_task_condition.notify_one ();
  return setted;
}


//--//--//--//--//--//--//--//--//--//--//--//--//--//--//--//--//--//--//--//--//--//--//--//--//--//--//

void thread_pool::uid_generator::add_uid (uid_type uid)
{
  m_uids.insert (std::upper_bound (m_uids.begin (), m_uids.end (), uid), uid);
}

void thread_pool::uid_generator::del_uid (uid_type uid)
{
  auto upper = std::upper_bound (m_uids.begin (), m_uids.end (), uid, [] (uid_type a, uid_type b) { return a <= b; });
  if (upper == m_uids.end ())
    return;

  if (*upper == uid)
    m_uids.erase (upper);
}

uid_type thread_pool::uid_generator::generate_new_uid () const
{
  for (int i = 0; i < (int) m_uids.size (); i++)
    {
      uid_type uid = (uid_type) i;
      if (m_uids[i] != uid)
        return uid;
    }
  return m_uids.empty () ? 0 : m_uids.back () + 1;
}

//--//--//--//--//--//--//--//--//--//--//--//--//--//--//--//--//--//--//--//--//--//--//--//--//--//--//

thread_pool::thread_pool ()
{
  m_uid_generator = std::make_unique<uid_generator> ();
}

thread_pool::~thread_pool ()
{
  join ();
}

void thread_pool::set_timeout (int timeout_ms)
{
  std::unique_lock<std::mutex> lock (m_mutex);
  set_timeout_unsafe (timeout_ms);
}

bool thread_pool::add_task (std::function<void()> &&task)
{
  std::unique_lock<std::mutex> lock (m_mutex);

  if (m_is_closed)
    return false;

  send_task_unsafe  (std::move (task));
  return true;
}

void thread_pool::wait ()
{
  std::unique_lock<std::mutex> lock (m_mutex);

  m_wait_condition.wait (lock, [this] ()
  {
      for (thread_t &thread: m_threads)
        {
          if (thread.has_task ())
            return false;
        }
      return true;
    });
}

void thread_pool::join ()
{
  std::unique_lock<std::mutex> lock (m_mutex);

  if (m_is_closed)
    return;

  close_pool_unsafe ();
  terminate_threads_unsafe ();

  //we can't use std::thead::join becouse threads are not joinable (see std::thread::detach)
  m_join_condition.wait (lock, [this] () { return m_threads.size () == 0; });
}

bool thread_pool::empty ()
{
  std::unique_lock<std::mutex> lock (m_mutex);

  return m_threads.empty ();
}

bool thread_pool::no_tasks ()
{
  std::unique_lock<std::mutex> lock (m_mutex);

  for (thread_t &thread: m_threads)
    {
      if (thread.has_task ())
        return false;
    }
  return true;
}

bool thread_pool::delete_thread (uid_type uid)
{
  {
    std::unique_lock<std::mutex> lock (m_mutex);
    auto thread_it = std::find_if (m_threads.begin (), m_threads.end (), [uid] (thread_t &thread) { return thread.uid () == uid; });
    if (thread_it == m_threads.end ())
      {
        assert (false);
        return false;
      }

    assert (!thread_it->has_task_unsafe ());
    m_uid_generator->del_uid (uid);
    m_threads.erase (thread_it);
  }
  m_join_condition.notify_one ();
  return true;
}

void thread_pool::send_task_unsafe (std::function<void()> &&task)
{
  for (thread_t &thread : m_threads)
    {
      if (thread.try_set_task (std::move (task)))
        return;
    }

  uid_type new_uid = m_uid_generator->generate_new_uid ();
  m_uid_generator->add_uid (new_uid);
  m_threads.emplace_back (this, m_timeout_ms, new_uid, std::move (task));
}

void thread_pool::terminate_threads_unsafe ()
{
  for (thread_t &thread: m_threads)
    thread.terminate ();
}

void thread_pool::close_pool_unsafe ()
{
  m_is_closed = true;
}

void thread_pool::set_timeout_unsafe (int timeout_ms)
{
  m_timeout_ms = timeout_ms;

  for (thread_t &thread: m_threads)
    thread.set_timeout (timeout_ms);
}

}  // namespace parallel









