#include "gno_modeling_simple_acceleration.h"

#include "gno_test.h"
#include "gno_graph.h"

namespace graph
{
int gno_modeling_simple_acceleration::run (const graph_initial &initial_state)
{
    if (!OK (graph::check_path (initial_state)))
      return -1;

    const graph_initial_state_base *initial_states = initial_state.get_initial_state ();
    const graph::uid veh_count = initial_states->vehicle_count ();

    m_t = 0.;
    m_states.clear ();
    m_states.resize (veh_count);

    m_velocities.assign (veh_count, m_start_velocity);
    m_accelerations.assign (veh_count, m_start_acc);

    find_critical_time (m_t, m_states);

    while (!is_finished (initial_state))
    {
      if (!OK (do_step (initial_state)))
        return -2;
    }
    return 0;
}

bool gno_modeling_simple_acceleration::is_finished (const graph_initial &initial_state) const
{
    const graph_initial_state_base *initial_states = initial_state.get_initial_state ();
    const graph::uid veh_count = initial_states->vehicle_count ();

    for (graph::uid veh_id = 0; veh_id < veh_count; veh_id++)
    {       
      if (!fuzzy_eq (m_states[veh_id].part, 1.) || m_states[veh_id].edge_num != isize (initial_states->vehicle (veh_id).path) - 1)
        return false;
    }
    return true;
}

int gno_modeling_simple_acceleration::do_step (const graph_initial &initial_state)
{
    const graph_initial_state_base *initial_states = initial_state.get_initial_state ();
    const graph_base *graph = initial_state.get_graph ();

    const graph::uid veh_count = initial_states->vehicle_count ();

    double min_critical_time = -1.;
    graph::uid min_veh_id = graph::invalid_uid;

    for (graph::uid veh_id = 0; veh_id < veh_count; veh_id++)
    {
        if (fuzzy_eq (m_states[veh_id].part, 1.) && m_states[veh_id].edge_num  == isize (initial_states->vehicle (veh_id).path) - 1)
          continue;

        const graph::Directional_Vehicle &veh = initial_states->vehicle(veh_id);
        const size_t edge_num = m_states[veh_id].edge_num;

        const double S = graph->length (veh.path[edge_num]);
        const double dS = S * (1. - m_states[veh_id].part);

        const double V = m_velocities[veh_id];
        const double a = m_accelerations[veh_id];

        double critical_time = 0.;

        if (m_t < veh.t)
        {
          critical_time = veh.t - m_t;
        }
        else if (fuzzy_eq (a, 0.))
        {
          critical_time = dS / V;
        }
        else
        {
          const double crit_time_end_edge = (-V + sqrt (V * V + 2 * a * dS)) / a;;
          const double crit_time_end_acc = (graph::V_MAX - V) / a;

          if (crit_time_end_acc < crit_time_end_edge)
            critical_time = crit_time_end_acc;
          else
            critical_time = crit_time_end_edge;
        }

        if (min_veh_id == graph::invalid_uid || critical_time < min_critical_time)
        {
            min_critical_time = critical_time;
            min_veh_id = veh_id;
        }
    }

    m_t += min_critical_time;

    for (graph::uid veh_id = 0; veh_id < veh_count; veh_id++)
    {
        if (fuzzy_eq (m_states[veh_id].part, 1.) && m_states[veh_id].edge_num  == isize (initial_states->vehicle (veh_id).path) - 1)
            continue;

        const graph::Directional_Vehicle &veh = initial_states->vehicle(veh_id);
        const size_t edge_num = m_states[veh_id].edge_num;

        const double S = graph->length (veh.path[edge_num]);

        const double V = m_velocities[veh_id];
        const double a = m_accelerations[veh_id];

        const double dS = V * min_critical_time + 0.5 * a * min_critical_time * min_critical_time;

        m_states[veh_id].part += dS / S;

        if (fuzzy_eq (m_states[veh_id].part, 1.))
        {
            const auto &path = initial_states->vehicle (veh_id).path;
            if (edge_num == isize (path) - 1)
              continue;
            m_states[veh_id].edge_num ++;
            m_states[veh_id].part = 0.;
        }
    }

    //update velocities and acc
    for (graph::uid veh_id = 0; veh_id < veh_count; veh_id++)
    {
        if (m_t < initial_states->vehicle(veh_id).t)
          continue;

        m_velocities[veh_id] += min_critical_time * m_accelerations[veh_id];
        if (fuzzy_eq (m_velocities[veh_id], graph::V_MAX))
            m_accelerations[veh_id] = 0.;
    }

    find_critical_time (m_t, m_states);
    return 0;
}

}

