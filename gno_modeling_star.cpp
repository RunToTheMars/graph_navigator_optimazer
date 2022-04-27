#include "gno_modeling_star.h"
#include "gno_graph.h"

namespace graph
{
  int gno_star_modeling::run (const graph_initial &initial_state)
  {
    m_initial_state = &initial_state;

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

  int gno_star_modeling::init_states ()
  {
      const graph_initial_state_base *initial_states = m_initial_state->get_initial_state ();
      const graph_base *graph = m_initial_state->get_graph ();

      graph::uid veh_count = initial_states->vehicle_count ();

      m_t = 0.;
      m_states.resize (veh_count);
      m_machine_states.resize (veh_count);
      m_veh_on_edge.assign (graph->edge_count(), 0);
      m_next_veh.resize (veh_count, graph::invalid_uid);

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
//          state.node_num = 0;
//          state.edge_uid = next_edge;
      }
    return 0;
  }

  bool gno_star_modeling::is_finished () const
  {
    for (auto &mst : m_machine_states)
    {
      if (mst.st != state_t::finished)
        return false;
    }
    return true;
  }

  int gno_star_modeling::update_states ()
  {
    const graph_initial_state_base *initial_states = m_initial_state->get_initial_state ();

    auto update_on_time = [this] (double dt) {
      for (auto &mst : m_machine_states)
      {
        mst.t_el += dt;
        mst.velocity += dt * mst.acc;
      }
    };

    auto find_next = [this] (graph::uid veh_id) -> std::pair<graph::uid, double> {
        double d = 0.;
        graph::uid closest_veh = graph::invalid_uid;

        const vehicle_discrete_state &cur_state = m_states[veh_id];
//        const std::vector<Edge> &next_edges =
//        for (const auto &state: m_states)
//        {

//        }
        return {closest_veh, d};
    };

    //find critical time
    double global_critical_time = -1.;
    for (graph::uid veh_id = 0; veh_id < isize (m_machine_states); veh_id++)
    {
      const Directional_Vehicle &veh = initial_states->vehicle(veh_id);

      double critical_time = -1.;
      switch (m_machine_states[veh_id].st)
      {
          case state_t::stop:
          {
            //start
            double critical_time_1 = m_t - veh.t;

            //2D distance
            double critical_time_2 = 0.;
          }
      }
    }

//      switch ()

//      update_on_time ();
    return 0;
  }
}

