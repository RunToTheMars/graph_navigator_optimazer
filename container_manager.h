#ifndef CONTAINER_MANAGER_H
#define CONTAINER_MANAGER_H

/// \file containers/container_manager.h

#include <functional>

template <typename Data>
class container_manager
{
    using Updater = std::function<void (Data &)>;

    Data &m_data;

    Updater m_do_on_start;
    Updater m_do_on_end;

public:

    class container_helper
    {
        Data &m_data;
        container_manager<Data> *m_container;
    public:
        container_helper (Data &data, container_manager<Data> *container): m_data (data), m_container (container)
        {
            m_container->start_change ();
        }

        ~container_helper ()
        {
            m_container->update ();
            m_container->end_change ();
        }

        Data *operator-> () { return &m_data; }
    };

public:
    template <typename... Args>
    container_manager (const Updater &do_on_start, const Updater &do_on_end, Data &data):
        m_data (data),
        m_do_on_start (do_on_start),
        m_do_on_end (do_on_end)
    {
    }

    [[nodiscard]] container_helper get_mutable () { return container_helper (*m_data, this); }
    const Data *get_const () const { return m_data.get (); }
    const Data &get () const { return *m_data; }

    const Data operator () () const { return *m_data; }
    const Data *operator-> () const { return m_data.get (); }

private:
    void update () { m_updater (*m_data); }
    void start_change () { m_do_on_start (*m_data); }
    void end_change () { m_do_on_end (*m_data); }
};

#endif //CONTAINER_MANAGER_H
