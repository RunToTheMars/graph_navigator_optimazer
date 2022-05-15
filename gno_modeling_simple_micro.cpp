#include "gno_modeling_simple_micro.h"


#include "gno_graph.h"
#include "gno_test.h"

namespace graph
{
gno_modeling_simple_micro::gno_modeling_simple_micro (double v_max, double v_min, double a_max, double D_, double l_):
    m_v_max (v_max), m_v_min (v_min), m_a_max (a_max), m_D (D_), m_l (l_)
{

}

void gno_modeling_simple_micro::clear_states (const graph_initial &initial_state)
{
    const graph::uid veh_count = initial_state.get_initial_state()->vehicle_count ();

    m_t = 0.;
    m_states.clear ();
    m_machine_states.clear ();

    m_states.resize (veh_count);
    m_machine_states.resize (veh_count);
}

bool gno_modeling_simple_micro::is_finished (const graph_initial &/*initial_state*/) const
{
    for (auto &mst : m_machine_states)
    {
        if (mst.st != state_t::finished)
            return false;
    }
    return true;
}

int gno_modeling_simple_micro::update_states (const graph_initial &initial_state)
{
    const graph_base *graph = initial_state.get_graph ();
    const graph_initial_state_base *initial_states = initial_state.get_initial_state ();

    //1. find critical time:
    graph::uid global_min_timer = graph::invalid_uid;
    double global_critical_time = -1.;

    auto find_next = [this, initial_states, graph] (graph::uid veh_id) -> std::pair<graph::uid, double> {
        const graph::Directional_Vehicle veh = initial_states->vehicle(veh_id);

        double min_d = -1.;
        graph::uid closest_veh_id = graph::invalid_uid;

        const vehicle_discrete_state &cur_state = m_states[veh_id];
        const graph::uid cur_edge = veh.path[cur_state.edge_num];

        for (graph::uid other_veh_id = 0; other_veh_id < initial_states->vehicle_count(); other_veh_id ++)
        {
            if (other_veh_id == veh_id)
              continue;

            if (other_veh_id == model_independer_uid)
              continue;

            const graph::Directional_Vehicle &other_veh = initial_states->vehicle(other_veh_id);
            const vehicle_discrete_state &other_state = m_states[other_veh_id];

            const graph::uid other_edge = other_veh.path[other_state.edge_num];

            if (other_edge != cur_edge || other_state.part < cur_state.part)
              continue;

            double d = graph->length (cur_edge) * (other_state.part - cur_state.part);
            if (d > 0 && (closest_veh_id == graph::invalid_uid || d < min_d))
            {
                closest_veh_id = other_veh_id;
                min_d = d;
            }
        }

//        if (veh_id == 33 && cur_state.edge_num == 9)
//        {
//            const vehicle_discrete_state &state28 = m_states[28];
//            const graph::Directional_Vehicle veh28 = initial_states->vehicle(28);
//            (void) veh_id;
//            int x = 0;
//            x ++;
//        }

        return {closest_veh_id, min_d};
    };


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
        case state_t::a_max:
        {
            critical_time = (m_v_max - veh_v) / m_a_max;

            const double S = graph->length (cur_edge);
            const double dS = S * (1. - m_states[veh_id].part);

            double crit_time_in_edge_change = (-veh_v + sqrt (veh_v * veh_v + 2 * m_a_max * dS))/(m_a_max);
            if (crit_time_in_edge_change < critical_time)
              critical_time = crit_time_in_edge_change;

            auto find = find_next (veh_id);
            graph::uid next_veh = find.first;
            double d = find.second;
            if (next_veh == graph::invalid_uid)
              break;

            double other_veh_v = m_machine_states[next_veh].velocity;

            if (d < m_l)
            {
              critical_time = 0.;
               break;
            }

            if (m_machine_states[next_veh].st == state_t::a_max)
            {
                if (fuzzy_eq (m_machine_states[veh_id].velocity, other_veh_v)
                    || m_machine_states[veh_id].velocity < other_veh_v)
                    break;

                double new_crit_time = (d - m_l) / (m_machine_states[veh_id].velocity - other_veh_v);
                if (new_crit_time < critical_time)
                  critical_time = new_crit_time;
            }

            if (m_machine_states[next_veh].st == state_t::v_min)
            {
                double discr = (veh_v - other_veh_v) * (veh_v - other_veh_v) + 2 * m_a_max * (d - m_l);
                double new_crit_time = (other_veh_v - veh_v + sqrt (discr)) / m_a_max;
                if (new_crit_time < critical_time)
                  critical_time = new_crit_time;
            }
            break;
        }
        case state_t::v_max:
        {
            const double S = graph->length (cur_edge);
            const double dS = S * (1. - m_states[veh_id].part);

            critical_time = dS / veh_v;

            auto find = find_next (veh_id);
            graph::uid next_veh = find.first;
            double d = find.second;
            if (next_veh == graph::invalid_uid)
                break;

            if (d < m_l)
            {
                critical_time = 0.;
                break;
            }

            double other_veh_v = m_machine_states[next_veh].velocity;

            if (m_machine_states[next_veh].st == state_t::a_max)
            {
                double discr = (veh_v - other_veh_v) * (veh_v - other_veh_v) - 2 * m_a_max * (d - m_l);

                if (discr < 0)
                  break;

                double new_crit_time = (veh_v - other_veh_v + sqrt (discr)) / m_a_max;
                if (new_crit_time < critical_time)
                    critical_time = new_crit_time;
            }

            if (m_machine_states[next_veh].st == state_t::v_min)
            {
                double new_crit_time = (d - m_l) / (veh_v - other_veh_v);
                if (new_crit_time < critical_time)
                  critical_time = new_crit_time;
            }

            break;
        }
        case state_t::v_min:
        {
            const double S = graph->length (cur_edge);
            const double dS = S * (1. - m_states[veh_id].part);

            critical_time = dS / veh_v;

            auto find = find_next (veh_id);
            graph::uid next_veh = find.first;
            double d = find.second;
            if (next_veh == graph::invalid_uid)
            {
              critical_time = 0.;
              break;
            }

            if (d > m_D)
            {
              critical_time = 0.;
              break;
            }

            double other_veh_v = m_machine_states[next_veh].velocity;

            if (m_machine_states[next_veh].st == state_t::a_max)
            {
                double discr = (veh_v - other_veh_v) * (veh_v - other_veh_v) + 2 * m_a_max * (m_D - d);

                if (discr < 0)
                    break;

                double new_crit_time = (veh_v - other_veh_v + sqrt (discr)) / m_a_max;
                if (new_crit_time < critical_time)
                  critical_time = new_crit_time;
            }

            if (m_machine_states[next_veh].st == state_t::v_max)
            {
                double new_crit_time = (m_D - d) / (other_veh_v - veh_v);
                if (new_crit_time < critical_time)
                  critical_time = new_crit_time;
            }

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

    //2. update x, v:
    for (graph::uid veh_id = 0; veh_id < isize (m_machine_states); veh_id++)
    {
        const Directional_Vehicle &veh = initial_states->vehicle(veh_id);
        const vehicle_discrete_state &cur_state = m_states[veh_id];
        const graph::uid cur_edge = veh.path[cur_state.edge_num];
        const double veh_v = m_machine_states[veh_id].velocity;

        if (m_machine_states[veh_id].st == state_t::wait || m_machine_states[veh_id].st == state_t::finished)
          continue;

        double dS = 0.;
        if (m_machine_states[veh_id].st == state_t::v_max || m_machine_states[veh_id].st == state_t::v_min)
          dS = global_critical_time * veh_v;
        else
        {
          dS = global_critical_time * veh_v + 0.5 * m_a_max * global_critical_time * global_critical_time;
          m_machine_states[veh_id].velocity = veh_v + m_a_max * global_critical_time;
        }

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

            m_machine_states[veh_id].st = state_t::a_max;
            m_machine_states[veh_id].velocity = m_v_min;
            break;
        }
        case state_t::a_max:
        {
            if (m_machine_states[veh_id].velocity > m_v_max || fuzzy_eq (m_machine_states[veh_id].velocity, m_v_max))
            {
                m_machine_states[veh_id].st = state_t::v_max;
                m_machine_states[veh_id].velocity = m_v_max;
                break;
            }

            auto find = find_next (veh_id);
            graph::uid next_veh = find.first;
            double d = find.second;
            if (next_veh == graph::invalid_uid)
              break;

            if (d < m_l || fuzzy_eq (d, m_l))
            {
              m_machine_states[veh_id].st = state_t::v_min;
              m_machine_states[veh_id].velocity = m_v_min;
              break;
            }

            break;
        }
        case state_t::v_min:
        {
            auto find = find_next (veh_id);
            graph::uid next_veh = find.first;
            double d = find.second;
            if (next_veh == graph::invalid_uid)
            {
                m_machine_states[veh_id].st = state_t::a_max;
                m_machine_states[veh_id].velocity = m_v_min;
                break;
            }

            if (d > m_D || fuzzy_eq (d, m_D))
            {
                m_machine_states[veh_id].st = state_t::a_max;
                m_machine_states[veh_id].velocity = m_v_min;
                break;
            }

            break;
        }
        case state_t::v_max:
        {
            auto find = find_next (veh_id);
            graph::uid next_veh = find.first;
            double d = find.second;
            if (next_veh == graph::invalid_uid)
              break;

            if (d < m_l || fuzzy_eq (d, m_l))
            {
              m_machine_states[veh_id].st = state_t::v_min;
              m_machine_states[veh_id].velocity = m_v_min;
              break;
            }

            break;
        }
        case state_t::finished:
        {
            break;
        }
        }
    }

    for (graph::uid veh_id = 0; veh_id < isize (m_machine_states); veh_id++)
    {
        switch (m_machine_states[veh_id].st)
        {
            switch (m_machine_states[veh_id].st)
            {
                case state_t::wait:
                {
                    break;
                }
                case state_t::a_max:
                {
                  if (m_machine_states[veh_id].velocity > m_v_max)
                    return -1;

                  if (m_machine_states[veh_id].velocity < m_v_min)
                    return -1;

                  break;
                }
                case state_t::v_min:
                {
                  if (!fuzzy_eq (m_machine_states[veh_id].velocity, m_v_min))
                    return -1;

                  break;
                }
                case state_t::v_max:
                {
                  if (!fuzzy_eq (m_machine_states[veh_id].velocity, m_v_max))
                    return -1;

                  break;
                }
                case state_t::finished:
                {
                    break;
                }
            }
        }
    }
    return 0;
}
}

