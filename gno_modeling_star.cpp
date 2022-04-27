#include "gno_modeling_star.h"
#include "gno_graph.h"
#include "gno_test.h"

namespace graph
{
  int gno_star_modeling::run (const graph_initial &initial_state)
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

  int gno_star_modeling::init_states ()
  {
      const graph_initial_state_base *initial_states = m_initial_state->get_initial_state ();
      const graph::uid veh_count = initial_states->vehicle_count ();

      m_t = 0.;
      m_states.resize (veh_count);
      m_machine_states.resize (veh_count);
      m_next_veh.resize (veh_count, graph::invalid_uid);

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

            //dist_for_start distance
            double critical_time_2 = 0.;
          }
      }
    }

//      switch ()

//      update_on_time ();
    return 0;
  }
}

