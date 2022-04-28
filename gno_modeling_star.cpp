#include "gno_modeling_star.h"

#include "gno_graph.h"
#include "gno_test.h"
#include <algorithm>

static int time_for_path (double d, double a, double v, double &t)
{
    t = 0;
    if (fuzzy_eq (a, 0.))
    {
      if (fuzzy_eq (v, 0.) || v < 0)
      {
        return -1;
      }
      else
      {
        t = d / v;
        return 0;
      }
    }
    else
    {
      double D = v * v + 2 * a * d;
      if (D < 0.)
        return -1;

      if (a > 0)
      {
        t = (-v + sqrt (D)) / a;
        return 0;
      }
      else if (v < 0)
        return -1;
      else
      {
        t = (sqrt (D) - v) / a;
        return 0;
      }

    }

    return 0;
}

static int t_for_D_dist (double d, double a1, double v1, double a0, double v0, double &t)
{
  return time_for_path (graph::D - d, a1 - a0, v1 - v0, t);
}

static void to_star (double d, double a1, double v1, double v0, double &t, double &a0)
{
    t = 2 * (d * graph::V_MAX - graph::D * v1) / (a1 * graph::D + v0 * graph::V_MAX - v1 * graph::V_MAX);
    a0 = a1 * a1 * graph::V_MAX * graph::V_MAX + 2 * a1 * graph::D * v0 * graph::V_MAX
         - 2 * a1 * d * graph::V_MAX * graph::V_MAX + v0 * v0 * graph::V_MAX * graph::V_MAX
         - 2 * v1 * v0 * graph::V_MAX * graph::V_MAX + v1 * v1 * graph::V_MAX * graph::V_MAX;
    a0 *= 0.5;
    a0 /= (graph::V_MAX * (graph::D * v1 - d * graph::V_MAX));
}


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
      m_states.clear ();
      m_machine_states.clear ();

      m_states.resize (veh_count);
      m_machine_states.resize (veh_count);

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
    const graph_base *graph = m_initial_state->get_graph ();
    const graph_initial_state_base *initial_states = m_initial_state->get_initial_state ();

    auto update_on_time = [this] (double dt) {
      for (auto &mst : m_machine_states)
      {
        mst.t_el += dt;
        mst.velocity += dt * mst.acc;
      }
    };

    auto find_next = [this, initial_states, graph] (graph::uid veh_id) -> std::pair<graph::uid, double> {
        const graph::Directional_Vehicle veh = initial_states->vehicle(veh_id);

        double min_d = 0.;
        graph::uid closest_veh_id = graph::invalid_uid;

        const vehicle_discrete_state &cur_state = m_states[veh_id];
        const graph::uid cur_edge = veh.path[cur_state.edge_num];

        //find on current edge
        for (graph::uid other_veh_id = 0; other_veh_id < initial_states->vehicle_count(); other_veh_id ++)
        {
            if (other_veh_id == veh_id)
              continue;

            const graph::Directional_Vehicle &other_veh = initial_states->vehicle(other_veh_id);
            const vehicle_discrete_state &other_state = m_states[other_veh_id];

            const graph::uid other_edge = other_veh.path[other_state.edge_num];

            if (other_edge != cur_edge || other_state.part < cur_state.part)
              continue;

            double d = graph->length (cur_edge) * (other_state.part - cur_state.part);
            if (closest_veh_id == graph::invalid_uid || d < min_d)
            {
                closest_veh_id = other_veh_id;
                min_d = d;
            }
        }

        if (closest_veh_id != graph::invalid_uid)
          return {closest_veh_id, min_d};

        //no next
        if (cur_state.edge_num == isize (veh.path) - 1)
          return {closest_veh_id, min_d};

        const graph::uid next_edge = veh.path[cur_state.edge_num + 1];

        //find on next edge
        for (graph::uid other_veh_id = 0; other_veh_id < initial_states->vehicle_count(); other_veh_id ++)
        {
            if (other_veh_id == veh_id)
                continue;

            const graph::Directional_Vehicle &other_veh = initial_states->vehicle(other_veh_id);
            const vehicle_discrete_state &other_state = m_states[other_veh_id];

            const graph::uid other_edge = other_veh.path[other_state.edge_num];

            if (other_edge != next_edge)
              continue;

            double d = graph->length (next_edge) * other_state.part;
            if (closest_veh_id == graph::invalid_uid || d < min_d)
            {
                closest_veh_id = other_veh_id;
                min_d = d;
            }
        }

        if (closest_veh_id == graph::invalid_uid)
          return {closest_veh_id, min_d};

        return {closest_veh_id, min_d + graph->length (cur_edge) * (1. - cur_state.part)};
    };

    //1. find critical time:
    graph::uid global_min_timer = graph::invalid_uid;
    double global_critical_time = -1.;
    for (graph::uid veh_id = 0; veh_id < isize (m_machine_states); veh_id++)
    {
      const Directional_Vehicle &veh = initial_states->vehicle(veh_id);
      const vehicle_discrete_state &cur_state = m_states[veh_id];
      const graph::uid cur_edge = veh.path[cur_state.edge_num];
      const double veh_a = m_machine_states[veh_id].acc;
      const double veh_v = m_machine_states[veh_id].velocity;

      double critical_time = -1.;
      switch (m_machine_states[veh_id].st)
      {
          case state_t::finished:
          {
            continue;
          }
          case state_t::stop:
          {
             //start
            double critical_time_1 = veh.t - m_t;

            //dist_for_start distance
            double critical_time_2 = 0.;
            auto veh_d = find_next (veh_id);
            if (veh_d.first != graph::invalid_uid && !fuzzy_eq (veh_d.second, 0.))
            {
                if (veh_d.second < dist_for_start)
                {
                  const double a = m_machine_states[veh_d.first].acc;
                  const double v = m_machine_states[veh_d.first].velocity;
                  const double dS = dist_for_start - veh_d.second;

                  if (!OK (time_for_path (dS, a, v, critical_time_2)))
                    return -1;
                }
            }

            std::vector<double> crit_times = {critical_time_1, critical_time_2};
            critical_time = *std::max_element (crit_times.begin (), crit_times.end ());
            break;
          }
          case state_t::free_acc:
          {
              const double critical_time_v_max = (graph::V_MAX - veh_v) / (veh_a);
              const double dS = (1. - cur_state.part) * graph->length (cur_edge);
              double critical_time_next_node;
              if (!OK (time_for_path (dS, veh_a, veh_v, critical_time_next_node)))
                return -1;

              if (critical_time_v_max < critical_time_next_node)
                critical_time = critical_time_v_max;
              else
                critical_time = critical_time_next_node;
              break;
          }
          case state_t::free_no_acc:
          {
              //to end of edge
              const double dS = (1. - cur_state.part) * graph->length (cur_edge);

              if (!OK (time_for_path (dS, veh_a, veh_v, critical_time)))
                  return -1;

              //to some next veh
              const auto veh_d = find_next (veh_id);
              if (veh_d.first != graph::invalid_uid)
              {
                if (veh_d.second < graph::D)
                  return -1;

                double t;
                const double a = m_machine_states[veh_d.first].acc;
                const double v = m_machine_states[veh_d.first].velocity;

                if (OK (t_for_D_dist (veh_d.second, a, v, veh_a, veh_v, t)))
                {
                  if (t < critical_time)
                    critical_time = t;
                }
              }
              break;
          }
          case state_t::go_to_star:
          {
            critical_time = m_machine_states[veh_id].target_t - m_machine_states[veh_id].t_el;
            break;
          }
          case state_t::star:
          {
            //no crit
            critical_time = std::numeric_limits<double>::max ();
            m_machine_states[veh_id].acc = m_machine_states[m_machine_states[veh_id].next_veh].acc;
            break;
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
        const double veh_a = m_machine_states[veh_id].acc;
        const double veh_v = m_machine_states[veh_id].velocity;

        if (m_machine_states[veh_id].st == state_t::stop || m_machine_states[veh_id].st == state_t::finished)
          continue;

        double dS = veh_a * 0.5 * global_critical_time * global_critical_time + veh_v * global_critical_time;

        m_states[veh_id].part += dS / graph->length (cur_edge);
    }

    //elapsed time && velocity
    for (auto &mst : m_machine_states)
    {
        mst.t_el += global_critical_time;
        mst.velocity += global_critical_time * mst.acc;
    }

    m_t += global_critical_time;

    //3. update states
    for (graph::uid veh_id = 0; veh_id < isize (m_machine_states); veh_id++)
    {
        const Directional_Vehicle &veh = initial_states->vehicle(veh_id);
        const vehicle_discrete_state &cur_state = m_states[veh_id];
        const graph::uid cur_edge = veh.path[cur_state.edge_num];
        const double veh_a = m_machine_states[veh_id].acc;
        const double veh_v = m_machine_states[veh_id].velocity;

        if (m_machine_states[veh_id].st == state_t::finished)
            continue;

        if (fuzzy_eq (m_states[veh_id].part, 1.))
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
        case state_t::finished:
        {
          continue;
        }
        case state_t::stop:
        {
          auto veh_d = find_next (veh_id);
          if (veh_d.second < dist_for_start && !fuzzy_eq (veh_d.second, 0.))
            continue;

          if (m_t < veh.t)
            continue;

          m_machine_states[veh_id].st = state_t::free_acc;
          m_machine_states[veh_id].acc = graph::A_MAX;
          break;
        }
        case state_t::free_acc:
        {
          if (fuzzy_eq (veh_v, graph::V_MAX))
          {
            m_machine_states[veh_id].st = state_t::free_no_acc;
            m_machine_states[veh_id].acc = 0.;
          }

          break;
        }
        case state_t::free_no_acc:
        {
          // to some next veh
          const auto veh_d = find_next (veh_id);
          if (veh_d.first != graph::invalid_uid && fuzzy_eq (veh_d.second, graph::D))
          {
            m_machine_states[veh_id].st = state_t::go_to_star;

            m_machine_states[veh_id].next_veh = veh_d.first;
            m_machine_states[veh_d.first].prev_veh = veh_id;

            const double a = m_machine_states[veh_d.first].acc;
            const double v = m_machine_states[veh_d.first].velocity;
            to_star (graph::D, a, v, veh_v, m_machine_states[veh_id].target_t, m_machine_states[veh_id].acc);
            if (m_machine_states[veh_id].target_t < 0.)
              return -1;
          }
          break;
        }
        case state_t::go_to_star:
        {
          if (fuzzy_eq (m_machine_states[veh_id].target_t, m_machine_states[veh_id].t_el))
          {
            m_machine_states[veh_id].st = state_t::star;
          }
          break;
        }
        case state_t::star:
        {
          break;
        }
        case state_t::before_turn:
        {
          break;
        }
        }
    }
    return 0;
  }
}

