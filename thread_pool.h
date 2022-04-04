#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include <atomic>
#include <condition_variable>
#include <functional>
#include <future>
#include <memory>
#include <mutex>
#include <queue>
#include <stdexcept>
#include <thread>
#include <vector>

namespace parallel {
using uid_type = int16_t;

class thread_pool {
  class uid_generator final {
    std::vector<uid_type> m_uids;

  public:
    void add_uid(uid_type uid);
    void del_uid(uid_type uid);
    uid_type generate_new_uid() const;
  };

  class thread_t final : std::thread {
    friend class thread_pool;

    using parent_t = std::thread;

    struct thread_t_data {
      void do_task();

      thread_pool *m_pool;
      std::unique_ptr<std::function<void()>> m_task; /// the task
      uid_type m_uid;                                /// unique id

      int m_timeout_ms; /// timeout for death

      std::atomic_bool m_has_task;

      // local mutex
      std::mutex m_mutex;

      // task wait synchronization
      std::condition_variable m_task_condition;

      // init wait synchronization
      std::condition_variable m_init_condition;

      // flags
      std::atomic_bool m_terminated; /// for termiate current thread
    };

  public:
    thread_t(thread_pool *pool, int timeout_ms, uid_type uid,
             std::function<void()> &&task);
    thread_t(thread_t &&thread);
    ~thread_t();

    thread_t &operator=(thread_t &&thread);

    uid_type uid() const { return m_data->m_uid; }

    bool has_task();
    bool has_task_unsafe();

    bool is_ready_for_task_unsafe();

    // signal to terminate thread after task
    void terminate();

    void set_timeout(int timeout_ms);

    void set_task_unsafe(std::function<void()> &&task);

    bool try_set_task(std::function<void()> &&task);

    std::unique_ptr<thread_t_data> m_data;
    std::unique_ptr<std::thread> m_worker;
  };

public:
  thread_pool();
  ~thread_pool();

  int timeout() const { return m_timeout_ms; }
  void set_timeout(int timeout_ms);

  [[nodiscard("Check closed pool")]] bool
  add_task(std::function<void()> &&task);

  // close pool and wait while all tasks will be done and unclose after
  void wait();

  // no threads
  bool empty();

  // no active tasks
  bool no_tasks();

  // add new work item to the pool
  template <class F, class... Args>
  std::future<typename std::result_of<F(Args...)>::type>
  enqueue(F &&f, Args &&...args) {
    using return_type = typename std::result_of<F(Args...)>::type;

    auto task = std::make_shared<std::packaged_task<return_type()>>(
        std::bind(std::forward<F>(f), std::forward<Args>(args)...));

    std::future<return_type> res = task->get_future();
    bool added = add_task([task]() { (*task)(); });
    if (!added)
      throw std::runtime_error("add task on closed pool");

    return res;
  }

private:
  // send task to either existing or new thread
  void send_task_unsafe(std::function<void()> &&task);

  // stop tracking a thread and terminate it
  bool delete_thread(uid_type id);

  // send signal to terminating current threads
  void terminate_threads_unsafe();

  // disable adding tasks
  void close_pool_unsafe();

  void set_timeout_unsafe(int timeout_ms);

  // close pool and wait while all tasks will be done and all threads terminated
  void join();

  // uid generator
  std::unique_ptr<uid_generator> m_uid_generator;

  // timeout for delete thread
  int m_timeout_ms = 5000;

  // all threads
  std::vector<thread_t> m_threads;

  // general mutex
  std::mutex m_mutex;

  // join wait synchronization
  std::condition_variable m_join_condition;

  // wait synchronization
  std::condition_variable m_wait_condition;

  // flags
  std::atomic_bool m_is_closed{false};
};
} // namespace parallel

#endif // THREAD_POOL_H
