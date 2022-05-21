#include "gno_path_finder_dijkstra.h"

#include "gno_graph.h"
#include "gno_graph_initial.h"
#include "gno_graph_initial_state.h"
#include "gno_modeling.h"
#include "utils.h"
#include <chrono>

#include <stack>

graph::gno_path_finder_dijkstra::gno_path_finder_dijkstra (graph::gno_discrete_modeling_base *model, bool use_depence):
    m_model (model), m_use_depence (use_depence)
{
}

std::vector<graph::uid> graph::gno_path_finder_dijkstra::run (const graph_initial &initial, graph::uid veh_uid)
{
    auto start = std::chrono::system_clock::now();
    m_modeling_count = 0;
    const graph::graph_base *graph = initial.get_graph ();
    const graph_initial_state_base *initial_state = initial.get_initial_state();

    const graph::uid node_count = graph->node_count ();
    const graph::uid veh_count = initial_state->vehicle_count();

    m_updatable_initial_state = std::make_unique<graph::graph_initial_state_impl> ();
    for (graph::uid veh_uid = 0; veh_uid < veh_count; veh_uid++)
      m_updatable_initial_state->add_vehicle (initial_state->vehicle (veh_uid));

    m_updatable_initial = std::make_unique<graph::graph_initial> (
        const_cast<graph_base *>(graph), m_updatable_initial_state.get ());

    std::vector<double> phi_to_node (node_count, -1.);
    std::vector<double> phi_to_node_valid (node_count, 0);
    std::vector<int> node_done (node_count, 0);
    std::vector<graph::uid> from_edge (node_count, graph::invalid_uid);

    Directional_Vehicle veh_opt = initial.get_initial_state()->vehicle (veh_uid);

    int node_done_count = 0;
    phi_to_node[veh_opt.src] = 0.;
    phi_to_node_valid[veh_opt.src] = 1;

    while (node_done_count != node_count)
    {
        graph::uid best_node = graph::invalid_uid;
        double best_time = -1.;

        for (graph::uid node_uid = 0; node_uid < node_count; node_uid ++)
        {
            double val = phi_to_node[node_uid];
            if (!(node_done[node_uid] == 0 && phi_to_node_valid[node_uid] == 1))
              continue;

            if (best_node == graph::invalid_uid || ((graph::MINIMIZE && val < best_time) || (!graph::MINIMIZE && val > best_time)))
            {
              best_node = node_uid;
              best_time = val;
            }
        }

        if (best_node < 0)
          return {};

        for (graph::uid edge_uid: graph->edges_started_from (best_node))
        {
            graph::uid next_node = graph->edge (edge_uid).end;

            std::vector<vehicle_discrete_state> states (veh_count);

            Directional_Vehicle &veh = m_updatable_initial_state->vehicle (veh_uid);
            veh.dst = next_node;

            std::vector<graph::uid> path;

            path.push_back (edge_uid);
            graph::uid prev_edge = from_edge[best_node];
            while (prev_edge != graph::invalid_uid)
            {
                path.push_back (prev_edge);
                prev_edge = from_edge[graph->edge (prev_edge).start];
            }

            std::reverse (path.begin (), path.end ());

            veh.path = path;

            m_model->clear_states (*m_updatable_initial);
            m_model->set_states (0., states);

            if (!m_use_depence)
              m_model->set_model_independer (veh_uid);
            else
              m_model->set_model_independer (graph::invalid_uid);

            std::vector<double> times (m_updatable_initial_state->vehicle_count (), 0.);
            m_model->set_do_in_critical_time ([this, &times] (double time, const std::vector<vehicle_discrete_state> &states)
               {
                 for (graph::uid veh_uid = 0; veh_uid < m_updatable_initial_state->vehicle_count (); veh_uid++)
                    {
                      if (times[veh_uid] > 0)
                        continue;

                      const vehicle_discrete_state &state = states[veh_uid];

                      if (fuzzy_eq (state.part, 1.) && state.edge_num == isize (m_updatable_initial_state->vehicle (veh_uid).path) - 1)
                        times[veh_uid] = time;
                    }
                });

            int r = m_model->run (*m_updatable_initial);
            if (!OK (r))
              return {};

            m_modeling_count ++;

            double val = m_phi (times);

            double next_node_val = phi_to_node[next_node];
            if (phi_to_node_valid[next_node] == 0 || ((graph::MINIMIZE && val < next_node_val) || (!graph::MINIMIZE && val > next_node_val)))
            {
              phi_to_node[next_node] = val;
              phi_to_node_valid[next_node] = 1;
              from_edge[next_node] = edge_uid;
            }
        }

        node_done[best_node] = 1;
        node_done_count ++;
        if (best_node == veh_opt.dst)
          break;
    }

    std::vector<graph::uid> path;
    graph::uid prev_edge = from_edge[veh_opt.dst];
    while (prev_edge != graph::invalid_uid)
    {
      path.push_back (prev_edge);
      prev_edge = from_edge[graph->edge (prev_edge).start];
    }

    std::reverse (path.begin (), path.end ());
    if (path.empty ())
      return {};

    if (graph->edge (path.front ()).start != veh_opt.src)
      return {};

    if (graph->edge (path.back ()).end != veh_opt.dst)
      return {};

    auto end = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_seconds = end - start;
    m_time_elapsed = elapsed_seconds.count();

    return path;
}
