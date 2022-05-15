#include "gno_path_finder_dijkstra.h"

#include "gno_graph.h"
#include "gno_graph_initial.h"
#include "gno_graph_initial_state.h"
#include "gno_modeling.h"
#include "utils.h"

#include <stack>

graph::gno_path_finder_dijkstra::gno_path_finder_dijkstra (graph::gno_discrete_modeling_base *model, bool use_depence):
    m_model (model), m_use_depence (use_depence)
{
    m_continuous_modeling = std::make_unique<graph::gno_continuous_modeling> (model);
}

std::vector<graph::uid> graph::gno_path_finder_dijkstra::run (const graph_initial &initial,
                                                              graph::uid src, graph::uid dst, Vehicle veh, double start_time)
{
    const graph::graph_base *graph = initial.get_graph ();
    const graph_initial_state_base *initial_state = initial.get_initial_state();

    const graph::uid node_count = graph->node_count ();
    const graph::uid veh_count = initial_state->vehicle_count();


    m_updatable_initial_state = std::make_unique<graph::graph_initial_state_impl> ();
    for (graph::uid veh_uid = 0; veh_uid < veh_count; veh_uid++)
      m_updatable_initial_state->add_vehicle (initial_state->vehicle (veh_uid));

    m_updatable_initial = std::make_unique<graph::graph_initial> (
        const_cast<graph_base *>(graph), m_updatable_initial_state.get ());

    m_line_states.clear ();

    m_continuous_modeling->set_do_on_linear_time (
        [this] (const graph::vehicle_continuous_line_states &states) {
          m_line_states.push_back (states);
        });

    int r = m_continuous_modeling->run (initial);
    if (!OK (r))
      return {};

    std::vector<double> time_to_node (node_count, -1.);
    std::vector<int> node_done (node_count, 0);
    std::vector<graph::uid> from_edge (node_count, graph::invalid_uid);

    int node_done_count = 0;
    time_to_node[src] = start_time;

    while (node_done_count != node_count)
    {
        graph::uid best_node = graph::invalid_uid;
        double best_time = -1.;

        for (graph::uid node_uid = 0; node_uid < node_count; node_uid ++)
        {
            double time = time_to_node[node_uid];
            if (node_done[node_uid] == 1 || time < 0.)
              continue;

            if (best_node == graph::invalid_uid || time < best_time)
            {
              best_node = node_uid;
              best_time = time;
            }
        }

        if (best_node < 0 || best_time < 0.)
          return {};

        for (graph::uid edge_uid: graph->edges_started_from (best_node))
        {
            graph::uid next_node = graph->edge (edge_uid).end;

////            int line_state_num = isize (m_line_states) - 1;

//            for (int i = 0; i < isize (m_line_states); i++)
//            {
//                if (m_line_states[i].contains (best_time))
//                {
//                    line_state_num = i;
//                    break;
//                }
//            }

//            const vehicle_continuous_line_states &line_states = m_line_states[line_state_num];

            std::vector<vehicle_discrete_state> states (veh_count + 1)/* = line_states*/;
//            for (graph::uid veh_uid = 0; veh_uid < veh_count; veh_uid++)
//            {
//                auto edge_part =  line_states.states[veh_uid].get_edge_and_par(best_time, line_states.t1, line_states.t2);
//                size_t start_num = edge_part.first == 0? line_states.states[veh_uid].edge_num_start : line_states.states[veh_uid].edge_num_end;
//                states.push_back({edge_part.second, start_num});
//            }

            Directional_Vehicle new_veh;
            new_veh.src = best_node;
            new_veh.dst = next_node;
            new_veh.path = {edge_uid};
            new_veh.vehicle = veh;
            new_veh.t = best_time;

            m_updatable_initial_state->add_vehicle (new_veh);

//            states.push_back ({0., 0});
            m_model->clear_states (*m_updatable_initial);
            m_model->set_states (0., states);

            if (!m_use_depence)
              m_model->set_model_independer (veh_count);

            double time_for_edge = -1.;

            m_model->set_do_in_critical_time (
                [this, &time_for_edge] (double time, const std::vector<vehicle_discrete_state> &states) {
                    const vehicle_discrete_state &state = states.back ();
                    if (!fuzzy_eq (state.part, 1.))
                      return;

                    m_model->interrupt ();
                    time_for_edge = time;
                });

            int r = m_model->run (*m_updatable_initial);
            if (!OK (r))
              return {};

            double next_node_time = time_to_node[next_node];
            if (next_node_time < 0 || time_for_edge < next_node_time)
            {
              time_to_node[next_node] = time_for_edge;
              from_edge[next_node] = edge_uid;
            }

            m_updatable_initial_state->remove_vehicle (veh_count);
        }

        node_done[best_node] = 1;
        node_done_count ++;
        if (best_node == dst)
          break;
    }

    std::vector<graph::uid> path;
    graph::uid prev_edge = from_edge[dst];
    while (prev_edge != graph::invalid_uid)
    {
      path.push_back (prev_edge);
      prev_edge = from_edge[graph->edge (prev_edge).start];
    }

    std::reverse (path.begin (), path.end ());
    if (path.empty ())
      return {};

    if (graph->edge (path.front ()).start != src)
      return {};

    if (graph->edge (path.back ()).end != dst)
      return {};

    return path;
}
