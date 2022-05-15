#include "gno_multipath_finder_loop.h"
#include "gno_graph_initial_state.h"
#include "gno_graph_initial.h"
#include "gno_modeling.h"
#include "gno_path_finder.h"


namespace graph
{
  gno_multipath_finder_loop::gno_multipath_finder_loop (graph::gno_path_finder_base *path_finder, graph::gno_continuous_modeling *continuous_modeling):
    m_path_finder (path_finder), m_continuous_modeling (continuous_modeling)
  {

  }

  double gno_multipath_finder_loop::average (graph::graph_initial *initial)
  {
      //modeling
      std::vector<graph::vehicle_continuous_line_states> line_states;
      m_continuous_modeling->discrete_model ()->set_model_independer (graph::invalid_uid);
      m_continuous_modeling->set_do_on_linear_time (
          [&line_states] (const graph::vehicle_continuous_line_states &states) {
              line_states.push_back (states);
          });

      int r = m_continuous_modeling->run (*initial);
      if (!OK (r) || line_states.empty ())
      {
          line_states.clear ();
          return -1.;
      }

      //times
      std::vector<double> times (initial->get_initial_state ()->vehicle_count(), 0.);

      for (const graph::vehicle_continuous_line_states &line_states : line_states)
      {
          for (graph::uid veh_uid = 0; veh_uid < initial->get_initial_state ()->vehicle_count (); veh_uid++)
          {
              const graph::Directional_Vehicle &veh = initial->get_initial_state ()->vehicle (veh_uid);

              if (times[veh_uid] > 0)
                  continue;

              const graph::vehicle_continuous_state &target_state = line_states.states[veh_uid];
              if (fuzzy_eq (target_state.part_end, 1.) && target_state.edge_uid_end == veh.path.back ())
              {
                  times[veh_uid] = line_states.t2;
                  break;
              }
          }
      }

      double average_time = 0.;
      for (graph::uid veh_uid = 0; veh_uid < initial->get_initial_state ()->vehicle_count (); veh_uid++)
          average_time += times[veh_uid];
//      double average_time = times.back ();

      average_time /= initial->get_initial_state ()->vehicle_count ();
      return average_time;
  }

  std::vector<std::vector<graph::uid>> gno_multipath_finder_loop::run (const graph_initial &initial_state)
  {
      int modeling_count = 0;
      int steps = 0;
      m_editable_graph_initial_state.reset ();
      m_editable_graph_initial.reset ();

      m_editable_graph_initial_state = std::make_unique<graph::graph_initial_state_impl> ();
      m_editable_graph_initial = std::make_unique<graph::graph_initial> (const_cast<graph::graph_base *>(initial_state.get_graph ()), m_editable_graph_initial_state.get ());

      for (graph::uid veh_uid = 0; veh_uid < initial_state.get_initial_state ()->vehicle_count (); veh_uid++)
      {
          graph::Directional_Vehicle veh = initial_state.get_initial_state ()->vehicle (veh_uid);
          m_editable_graph_initial_state->add_vehicle (veh);
      }

      while (true)
      {
          bool updated = false;

          for (graph::uid veh_uid = 0; veh_uid < initial_state.get_initial_state ()->vehicle_count (); veh_uid++)
          {
              double old_average = average (m_editable_graph_initial.get ());
              graph::Directional_Vehicle old_veh = m_editable_graph_initial_state->vehicle (veh_uid);

              m_path_finder->set_phi ([veh_uid] (const std::vector<double> &values)
                                     {
//                                         double res = 0.;
//                                         for (auto v: values)
//                                             res += v;
//                                         return res;
                                         return values[veh_uid];
                                     });
              std::vector<graph::uid> new_path = m_path_finder->run (*m_editable_graph_initial, veh_uid);
              steps ++;
              modeling_count += m_path_finder->m_modeling_count;
              m_editable_graph_initial_state->vehicle (veh_uid).path = new_path;
              double new_average = average (m_editable_graph_initial.get ());

              bool is_other_path = new_path.size () != old_veh.path.size ();
              if (!is_other_path)
              {
                for (int i = 0; i < isize (new_path); i++)
                {
                  if (new_path[i] != old_veh.path[i])
                  {
                    is_other_path = true;
                    break;
                  }
                }
              }

              if (is_other_path && ((graph::MINIMIZE && new_average < old_average) || (!graph::MINIMIZE && new_average > old_average)))
              {
                updated = true;
                m_editable_graph_initial_state->vehicle (veh_uid).path = new_path;
                break;
              }

              m_editable_graph_initial_state->vehicle (veh_uid).path = old_veh.path;
          }

          if (!updated)
            break;
      }

      std::vector<std::vector<graph::uid>> res;
      for (graph::uid veh_uid = 0; veh_uid < initial_state.get_initial_state ()->vehicle_count (); veh_uid++)
        res.push_back (m_editable_graph_initial_state->vehicle (veh_uid).path);

      return res;
  }
}
