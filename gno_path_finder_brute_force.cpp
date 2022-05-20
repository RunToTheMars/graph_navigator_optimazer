#include "gno_path_finder_brute_force.h"

#include "gno_graph.h"
#include "gno_graph_initial.h"
#include "gno_graph_initial_state.h"
#include "gno_modeling.h"
#include <chrono>
#include "utils.h"

#include <stack>

static void go_from_start (std::vector<int> &actual_src_nodes, graph::uid node_uid, const graph::graph_base *graph)
{
  if (actual_src_nodes[node_uid] == 1)
    return;
  else
  {
    actual_src_nodes[node_uid] = 1;
    for (graph::uid next_edge_uid : graph->edges_started_from (node_uid))
    {
      graph::uid next_node_uid = graph->edge (next_edge_uid).end;
      go_from_start (actual_src_nodes, next_node_uid, graph);
    }
  }
}

static void go_from_end (std::vector<int> &actual_dst_nodes, graph::uid node_uid, const graph::graph_base *graph)
{
  if (actual_dst_nodes[node_uid] == 1)
    return;
  else
  {
    actual_dst_nodes[node_uid] = 1;
    for (graph::uid prev_edge_uid : graph->edges_ended_on (node_uid))
    {
      graph::uid prev_node_uid = graph->edge (prev_edge_uid).start;
      go_from_end (actual_dst_nodes, prev_node_uid, graph);
    }
  }
}

static void do_on_path_find_rec (const std::vector<int> &actual_nodes, std::vector<int> &exist_node,
                                graph::uid dst,
                                size_t max_path_size,
                                const graph::graph_base *graph,
                                std::stack<graph::uid> &path,
                                std::function<void (const std::stack<graph::uid> &/*path*/)> on_path_find)
{
  graph::uid last_edge = path.top ();
  graph::uid last_node = graph->edge (last_edge).end;

  if (max_path_size >= 0 && path.size () >= max_path_size)
    return;

  if (last_node == dst)
  {
    on_path_find (path);
    return;
  }

  for (graph::uid next_edge : graph->edges_started_from (last_node))
  {
      graph::uid next_node = graph->edge (next_edge).end;

      if (actual_nodes[next_node] == 0 || exist_node[next_node] == 1)
        continue;

      path.push (next_edge);
      exist_node[next_node] = 1;

      do_on_path_find_rec (actual_nodes, exist_node, dst, max_path_size, graph, path, on_path_find);

      path.pop ();
      exist_node[next_node] = 0;
  }
}

static void do_on_path_find (const std::vector<int> &actual_nodes, graph::uid src, graph::uid dst,
                             const graph::graph_base *graph,
                             size_t max_path_size,
                             std::function<void (const std::stack<graph::uid> &/*path*/)> on_path_find)
{
  std::stack<graph::uid> path;
  std::vector<int> exist_node (graph->node_count (), 0);
  exist_node[src] = 1;

  for (graph::uid edge_uid: graph->edges_started_from (src))
  {
      graph::uid node_uid = graph->edge (edge_uid).end;
      if (actual_nodes[node_uid] == 0)
        continue;

      path.push (edge_uid);
      exist_node[node_uid] = 1;

      do_on_path_find_rec (actual_nodes, exist_node, dst, max_path_size, graph, path, on_path_find);

      path.pop ();
      exist_node[node_uid] = 0;
  }

}

graph::gno_path_finder_brute_force::gno_path_finder_brute_force (graph::gno_discrete_modeling_base *model, bool use_depence): m_model (model), m_use_depence (use_depence)
{
}

std::vector<graph::uid> graph::gno_path_finder_brute_force::run (const graph_initial &initial_state, graph::uid veh_uid)
{
  m_modeling_count = 0;
  m_min = 1e100;
  m_max = 0.;
  m_average = 0.;
  auto start = std::chrono::system_clock::now();

  const graph::graph_base *graph = initial_state.get_graph ();
  std::vector<int> actual_nodes (graph->node_count (), 0);

  if (actual_nodes.empty ())
    return {};

  Directional_Vehicle veh_opt = initial_state.get_initial_state()->vehicle (veh_uid);

  {
    std::vector<int> actual_src_nodes (graph->node_count (), 0);

    std::vector<int> actual_dst_nodes (graph->node_count (), 0);

    go_from_start (actual_src_nodes, veh_opt.src, graph);
    go_from_end (actual_dst_nodes, veh_opt.dst, graph);

    for (graph::uid node_uid = 0; node_uid < graph->node_count (); node_uid++)
    {
      if (actual_src_nodes[node_uid] == 1 && actual_dst_nodes[node_uid] == 1)
        actual_nodes[node_uid] = 1;
    }

    if (actual_nodes[veh_opt.src] == 0 || actual_nodes[veh_opt.dst] == 0)
      return {};
  }

    m_min_path.clear ();
    graph_initial_state_impl graph_initial_state;
    for (graph::uid veh_uid = 0; veh_uid < initial_state.get_initial_state ()->vehicle_count (); veh_uid++)
    {
      graph_initial_state.add_vehicle (initial_state.get_initial_state ()->vehicle (veh_uid));
    }
    graph_initial new_initial_state (const_cast<graph_base *>(initial_state.get_graph ()), &graph_initial_state);

    do_on_path_find (actual_nodes, veh_opt.src, veh_opt.dst, graph, max_path_size, [this, &new_initial_state, veh_uid] (const std::stack<graph::uid> &path_stack) {
        std::vector<graph::uid> path (path_stack._Get_container().begin (), path_stack._Get_container().end ());

        std::vector<double> times (new_initial_state.get_initial_state()->vehicle_count (), 0.);
        m_model->set_do_in_critical_time (
              [this, new_initial_state, &times] (double time, const std::vector<vehicle_discrete_state> &states) {
                for (graph::uid veh_uid = 0; veh_uid < new_initial_state.get_initial_state()->vehicle_count (); veh_uid++)
                {
                    if (times[veh_uid] > 0)
                        continue;

                    const vehicle_discrete_state &state = states[veh_uid];

                    if (fuzzy_eq (state.part, 1.) && state.edge_num == isize (new_initial_state.get_initial_state()->vehicle (veh_uid).path) - 1)
                      times[veh_uid] = time;
                }
            });

          Directional_Vehicle &updated_veh = new_initial_state.get_initial_state()->vehicle (veh_uid);
          updated_veh.path = path;

          m_model->clear_states (new_initial_state);
          m_model->run (new_initial_state);

          double val = m_phi (times);
          if (val < m_min)
          {
            m_min = val;
          }
          if (val > m_max)
          {
              m_max = val;
          }
          m_average += val;
          if (m_min_path.empty () ||  ((graph::MINIMIZE && val < m_min_time) || (!graph::MINIMIZE && val > m_min_time)))
          {
              m_min_time = val;
              m_min_path = path;
          }

          if (!m_use_depence)
            m_model->set_model_independer (veh_uid);

          m_modeling_count ++;
        });

  auto end = std::chrono::system_clock::now();
  std::chrono::duration<double> elapsed_seconds = end - start;
  m_time_elapsed = elapsed_seconds.count();
  m_average /= m_modeling_count;
  return m_min_path;
}
