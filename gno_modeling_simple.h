#ifndef GNO_MODELING_SIMPLE_H
#define GNO_MODELING_SIMPLE_H

#include "gno_modeling.h"

namespace graph
{
class gno_modeling_simple: public gno_discrete_modeling_base
{
  double m_t = 0.;
  std::vector<vehicle_discrete_state> m_states;
  std::vector<int> m_finished;
  std::vector<size_t> m_veh_on_edge;

public:
  int run (const graph_initial &initial_state) override;

  bool is_finished (const graph_initial &initial_state);
  int do_step (const graph_initial &initial_state);
};
}


#endif // GNO_MODELING_SIMPLE_H
