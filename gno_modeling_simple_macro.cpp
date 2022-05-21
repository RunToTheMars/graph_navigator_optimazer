#include "gno_modeling_simple_macro.h"


#include "gno_graph.h"
#include "gno_test.h"

namespace graph
{
void gno_modeling_simple_macro::clear_states (const graph_initial &initial_state)
{
    const graph::uid veh_count = initial_state.get_initial_state()->vehicle_count ();

    m_t = 0.;
    m_states.clear ();
    m_machine_states.clear ();

    m_states.resize (veh_count);
    m_machine_states.resize (veh_count);

    m_counts.assign (initial_state.get_graph()->edge_count (), 0);
}

bool gno_modeling_simple_macro::is_finished (const graph_initial &/*initial_state*/) const
{
    for (auto &mst : m_machine_states)
    {
        if (mst.st != state_t::finished)
            return false;
    }
    return true;
}

int gno_modeling_simple_macro::update_states (const graph_initial &initial_state)
{
    const graph_base *graph = initial_state.get_graph ();
    const graph_initial_state_base *initial_states = initial_state.get_initial_state ();

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

        if (m_machine_states[veh_id].st != state_t::move)
            continue;

        m_states[veh_id].part += dS / graph->length (cur_edge);
    }

    m_t += global_critical_time;

    //3. update states
    for (graph::uid veh_id = 0; veh_id < isize (m_machine_states); veh_id++)
    {
        const Directional_Vehicle &veh = initial_states->vehicle(veh_id);
        const vehicle_discrete_state &cur_state = m_states[veh_id];

        if (fuzzy_eq (m_states[veh_id].part, 1.) || m_states[veh_id].part > 1.)
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

    std::fill (m_counts.begin (), m_counts.end (), 0);

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

        if (veh_id == model_independer_uid)
          continue;
        m_counts[cur_edge] ++;
    }

    double r_count = 1. / isize (m_machine_states);
    for (graph::uid veh_id = 0; veh_id < isize (m_machine_states);
         veh_id++)
    {
        if (m_machine_states[veh_id].st != state_t::move)
            continue;

        const Directional_Vehicle &veh = initial_states->vehicle (veh_id);
        const vehicle_discrete_state &cur_state = m_states[veh_id];
        const graph::uid cur_edge = veh.path[cur_state.edge_num];

        int n = m_counts[cur_edge];

        if (veh_id == model_independer_uid)
          n ++;

        //m_machine_states[veh_id].velocity = graph::V_MAX / n;
        m_machine_states[veh_id].velocity = (graph::V_MAX - graph::V_MIN) * (1. - n * r_count) + graph::V_MIN;
    }
    return 0;
}
}

