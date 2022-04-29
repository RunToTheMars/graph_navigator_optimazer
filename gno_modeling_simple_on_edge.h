#ifndef GNO_MODELING_SIMPLE_ON_EDGE_H
#define GNO_MODELING_SIMPLE_ON_EDGE_H

#include "gno_modeling.h"

namespace graph
{
class gno_modeling_simple_on_edge: public gno_discrete_modeling_base
{
    enum class state_t
    {
      wait,
      move,
      finished,
    };

    struct state
    {
      state_t st = state_t::wait;
      double velocity = 0.;
    };

    double m_t = 0.;
    std::vector<vehicle_discrete_state> m_states;

    std::vector<state> m_machine_states;

    const graph_initial *m_initial_state = nullptr;


public:
    int run (const graph_initial &initial_state) override;

private:
    int init_states ();

    bool is_finished () const;
    int update_states ();
};
}
#endif // GNO_MODELING_SIMPLE_ON_EDGE_H
