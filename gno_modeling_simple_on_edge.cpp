#include "gno_modeling_simple_on_edge.h"

#include "gno_graph.h"
#include "gno_test.h"

namespace graph
{
int gno_modeling_simple_on_edge::run (const graph_initial &initial_state)
{
    m_initial_state = &initial_state;

    if (!OK (graph::check_path (initial_state)))
        return -1;

    if (!OK (init_states ()))
        return -1;

    find_critical_time (m_t, m_states);

    while (!is_finished ())
    {
        if (!OK (update_states ()))
            return -1;

        find_critical_time (m_t, m_states);
    }
    return 0;
}

int gno_modeling_simple_on_edge::init_states ()
{
    const graph_initial_state_base *initial_states = m_initial_state->get_initial_state ();
    const graph::uid veh_count = initial_states->vehicle_count ();

    m_t = 0.;
    m_states.clear ();
    m_machine_states.clear ();

    m_states.resize (veh_count);
    m_machine_states.resize (veh_count);

    return 0;
}

bool gno_modeling_simple_on_edge::is_finished () const
{
    for (auto &mst : m_machine_states)
    {
        if (mst.st != state_t::finished)
            return false;
    }
    return true;
}

int gno_modeling_simple_on_edge::update_states ()
{
    const graph_base *graph = m_initial_state->get_graph ();
    const graph_initial_state_base *initial_states = m_initial_state->get_initial_state ();

    //1. find critical time:
    graph::uid global_min_timer = graph::invalid_uid;
    double global_critical_time = -1.;
    for (graph::uid veh_id = 0; veh_id < isize (m_machine_states); veh_id++)
    {
        const Directional_Vehicle &veh = initial_states->vehicle(veh_id);
        const vehicle_discrete_state &cur_state = m_states[veh_id];
        const graph::uid cur_edge = veh.path[cur_state.edge_num];
        const double veh_v = m_machine_states[veh_id].velocity;

        double critical_time = -1.;
        switch (m_machine_states[veh_id].st)
        {
        case state_t::wait:
        {
            //start
            critical_time = veh.t - m_t;
            break;
        }
        case state_t::move:
        {
            const double S = graph->length (cur_edge);
            const double dS = S * (1. - m_states[veh_id].part);

            critical_time = dS / veh_v;
            break;
        }
        case state_t::finished:
        {
          continue;
        }
        }

        if (global_min_timer == graph::invalid_uid || critical_time < global_critical_time)
        {
            global_critical_time = critical_time;
            global_min_timer = veh_id;
        }
    }

    if (global_critical_time < 0.)
        return -1;

    //2. update:
    for (graph::uid veh_id = 0; veh_id < isize (m_machine_states); veh_id++)
    {
        const Directional_Vehicle &veh = initial_states->vehicle(veh_id);
        const vehicle_discrete_state &cur_state = m_states[veh_id];
        const graph::uid cur_edge = veh.path[cur_state.edge_num];
        const double veh_v = m_machine_states[veh_id].velocity;

        double dS = veh_v * global_critical_time;

        m_states[veh_id].part += dS / graph->length (cur_edge);
    }

    m_t += global_critical_time;

    //3. update states
    for (graph::uid veh_id = 0; veh_id < isize (m_machine_states); veh_id++)
    {
        const Directional_Vehicle &veh = initial_states->vehicle(veh_id);
        const vehicle_discrete_state &cur_state = m_states[veh_id];

        if (fuzzy_eq (m_states[veh_id].part, 1.))
        {
            if (cur_state.edge_num == isize (veh.path) - 1)
            {
                m_machine_states[veh_id].st = state_t::finished;
                continue;
            }

            m_states[veh_id].part = 0.;
            m_states[veh_id].edge_num ++;
        }

        switch (m_machine_states[veh_id].st)
        {
        case state_t::wait:
        {
            if (m_t < veh.t)
              continue;

            m_machine_states[veh_id].st = state_t::move;
            break;
        }
        case state_t::move:
        {
          break;
        }
        case state_t::finished:
        {
          break;
        }
        }
    }

    //update velocities
    for (graph::uid veh_id = 0; veh_id < isize (m_machine_states); veh_id++)
    {
      if (m_machine_states[veh_id].st != state_t::move)
        {
          m_machine_states[veh_id].velocity = 0.;
          continue;
        }

      const Directional_Vehicle &veh = initial_states->vehicle (veh_id);
      const vehicle_discrete_state &cur_state = m_states[veh_id];
      const graph::uid cur_edge = veh.path[cur_state.edge_num];

      int order = 0;
      int n = 1;

      for (graph::uid veh_id_other = 0; veh_id_other < isize (m_machine_states);
           veh_id_other++)
      {
        if (m_machine_states[veh_id_other].st != state_t::move)
          continue;

        if (veh_id_other == veh_id)
          continue;

        const Directional_Vehicle &other_veh =
            initial_states->vehicle (veh_id_other);
        const vehicle_discrete_state &other_state = m_states[veh_id_other];
        const graph::uid other_edge = other_veh.path[other_state.edge_num];

        if (m_machine_states[veh_id_other].st != state_t::move)
          continue;

        if (other_edge != cur_edge)
          continue;

        n++;

        if (other_state.part > cur_state.part)
          order++;
      }

      m_machine_states[veh_id].velocity = graph::V_MAX * (1 - (double) order / n);
    }
  return 0;
}
}

