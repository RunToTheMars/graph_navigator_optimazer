#include "gno_modeling_simple.h"
#include "gno_graph.h"

namespace graph
{
int gno_modeling_simple::run (const graph_initial &initial_state)
{
  const graph_initial_state_base *initial_states = initial_state.get_initial_state ();
  const graph_base *graph = initial_state.get_graph ();

  graph::uid veh_count = initial_states->vehicle_count ();

  m_t = 0.;
  m_states.resize (veh_count);
  m_finished.assign (veh_count, 0);
  m_veh_on_edge.assign (graph->edge_count(), 0);

  std::fill (m_veh_on_edge.begin (), m_veh_on_edge.end (), 0);

  for (graph::uid veh_id = 0; veh_id < veh_count; veh_id++)
  {
      vehicle_discrete_state &state = m_states[veh_id];

      //choose way
      const auto &path = initial_states->vehicle (veh_id).path;
      if (path.size () < 2)
        return -1;

      if (path.front () != initial_states->vehicle (veh_id).src)
        return -2;

      if (path.back () != initial_states->vehicle (veh_id).dst)
        return -3;

      auto edges = graph->edges (path[0], path[1]);

      if (edges.size () == 0)
        return -4;

      graph::uid next_edge = edges[0];
      for (size_t i = 1; i < edges.size (); i++)
      {
        graph::uid edge_uid = edges[i];
        if (m_veh_on_edge[edge_uid] < m_veh_on_edge[next_edge])
          next_edge = edge_uid;
      }

      m_veh_on_edge[next_edge] ++;

      state.part = 0.;
      state.velocity = graph::V_MAX;
      state.node_num = 0;
      state.edge_uid = next_edge;
  }

  find_critical_time (m_t, m_states);

  while (!is_finished (initial_state))
  {
    if (!OK (do_step (initial_state)))
      return -4;
  }
  return 0;
}

bool gno_modeling_simple::is_finished (const graph_initial &initial_state)
{
  const graph_initial_state_base *initial_states = initial_state.get_initial_state ();
  const graph_base *graph = initial_state.get_graph ();

  graph::uid veh_count = initial_states->vehicle_count ();
  bool res = true;

  for (graph::uid veh_id = 0; veh_id < veh_count; veh_id++)
  {
    bool is_finish = true;
    if (m_finished[veh_id] == 1)
      continue;

    if (!fuzzy_eq (m_states[veh_id].part, 1.))
      is_finish = false;

    auto edges = graph->edges_ended_on (initial_states->vehicle (veh_id).dst);

    if (std::find (edges.begin (), edges.end (), m_states[veh_id].edge_uid) == edges.end ())
      is_finish = false;

    if (is_finish)
      m_finished[veh_id] = 1;

    if (!is_finish)
      res = false;
  }
  return res;
}

int gno_modeling_simple::do_step (const graph_initial &initial_state)
{
    const graph_initial_state_base *initial_states = initial_state.get_initial_state ();
    const graph_base *graph = initial_state.get_graph ();

    graph::uid veh_count = initial_states->vehicle_count ();

    double min_critical_time = -1.;
    graph::uid min_veh_id = graph::invalid_uid;

    for (graph::uid veh_id = 0; veh_id < veh_count; veh_id++)
    {
        if (m_finished[veh_id] == 1)
            continue;

        double S = graph->length (m_states[veh_id].edge_uid);
        double V = m_states[veh_id].velocity;

        double critical_time = S * (1. - m_states[veh_id].part) / V;

        if (min_veh_id == graph::invalid_uid || critical_time < min_critical_time)
        {
          min_critical_time = critical_time;
          min_veh_id = veh_id;
        }
    }

    m_t += min_critical_time;

    std::fill (m_veh_on_edge.begin (), m_veh_on_edge.end (), 0);
    for (graph::uid veh_id = 0; veh_id < veh_count; veh_id++)
      m_veh_on_edge[m_states[veh_id].edge_uid] ++;

    for (graph::uid veh_id = 0; veh_id < veh_count; veh_id++)
    {
        if (m_finished[veh_id] == 1)
          continue;

        double V = m_states[veh_id].velocity;
        double S = graph->length (m_states[veh_id].edge_uid);

        double dS = V * min_critical_time;

        m_states[veh_id].part += dS / S;

        if (fuzzy_eq (m_states[veh_id].part, 1.))
        {
          const auto &path = initial_states->vehicle (veh_id).path;

          //choose path
          size_t node_num = m_states[veh_id].node_num;
          if (node_num >= path.size () - 2)
            continue;

          auto start_node = path[node_num + 1];
          auto end_node = path[node_num + 2];
          auto edges = graph->edges (start_node, end_node);

          if (edges.size () == 0)
            return -1;

          graph::uid next_edge = edges[0];
          for (graph::uid i = 1; i < edges.size (); i++)
          {
            graph::uid edge_id = edges[i];
            if (m_veh_on_edge[edge_id] < m_veh_on_edge[next_edge])
              next_edge = edge_id;
          }

          m_veh_on_edge[m_states[veh_id].edge_uid] --;
          m_veh_on_edge[next_edge] ++;

          m_states[veh_id].velocity = graph::V_MAX;
          m_states[veh_id].edge_uid = next_edge;
          m_states[veh_id].part = 0.;
          m_states[veh_id].node_num ++;
        }
    }

  find_critical_time (m_t, m_states);
  return 0;
}

}
