#ifndef GNO_MODELING_SIMPLE_MACRO_H
#define GNO_MODELING_SIMPLE_MACRO_H

#include "gno_modeling.h"


namespace graph
{
class gno_modeling_simple_macro final: public gno_discrete_modeling_base
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

    std::vector<state> m_machine_states;
    std::vector<int> m_counts;

    graph::uid model_independer_uid = graph::invalid_uid;
public:
    void set_model_independer (graph::uid veh_uid) override { model_independer_uid = veh_uid; }

    void clear_states (const graph_initial &initial_state) override;

private:
    void init_states ();

    bool is_finished (const graph_initial &initial_state) const override;
    int update_states (const graph_initial &initial_state) override;
};
}

#endif // GNO_MODELING_SIMPLE_MACRO_H
