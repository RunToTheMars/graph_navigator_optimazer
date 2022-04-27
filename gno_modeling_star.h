#ifndef GNO_MODELING_STAR_H
#define GNO_MODELING_STAR_H

#include "gno_modeling.h"

namespace graph
{
class gno_star_modeling: public gno_discrete_modeling_base
{
   static constexpr double dist_for_start = 2 * graph::D;

  enum class state_t
    {
        stop,
        free_acc,
        free_no_acc,
        go_to_star,
        star,
        before_turn,
        finished,
    };

  struct state
  {
    state_t st = state_t::stop;
    double velocity = 0.;
    double acc = 0.;

    double t_el = 0.;
  };

  double m_t = 0.;
  std::vector<vehicle_discrete_state> m_states;

  std::vector<state> m_machine_states;
  std::vector<graph::uid> m_next_veh;

  const graph_initial *m_initial_state = nullptr;


public:
   int run (const graph_initial &initial_state) override;

private:
   int init_states ();

   bool is_finished () const;
   int update_states ();
};


}

#endif // GNO_MODELING_STAR_H
