#include "gno_modeling.h"

namespace graph
{
gno_continuous_modeling::gno_continuous_modeling (gno_discrete_modeling_base *discrete_modeling): m_discrete_modeling (discrete_modeling)
{

}

int gno_continuous_modeling::run (const graph_initial &initial_state)
{
    int res = 0;

    graph::uid veh_count = initial_state.get_initial_state()->vehicle_count ();

    m_cricital_count = 0;
    m_discrete_modeling->set_do_in_critical_time ([this, &res, veh_count] (double time, const std::vector<vehicle_discrete_state> &cur_discrete_states) {
        if (m_cricital_count > 0)
        {
            if (fuzzy_eq (time, m_prev_time))
            {
                res = -1;
                return;
            }

            std::vector<vehicle_continuous_state> continuous_states (veh_count);

            for (graph::uid veh_id = 0; veh_id < veh_count; veh_id++)
            {
                const vehicle_discrete_state &prev_discrete_state = m_prev_discrete_states[veh_id];
                const vehicle_discrete_state &cur_discrete_state = cur_discrete_states[veh_id];

                vehicle_continuous_state &cur_continuous_states = continuous_states[veh_id];
                cur_continuous_states.part_start = prev_discrete_state.part;
                cur_continuous_states.edge_uid_start = prev_discrete_state.edge_uid;

                cur_continuous_states.part_end = cur_discrete_state.part;
                cur_continuous_states.edge_uid_end = cur_discrete_state.edge_uid;
            }

            m_do_on_linear_time ({m_prev_time, time, std::move (continuous_states)});
        }

        m_prev_discrete_states = cur_discrete_states;
        m_prev_time = time;
        m_cricital_count ++;
    });

    int r = m_discrete_modeling->run (initial_state);
    if (r < res)
      return r;
    return res;
}
}


