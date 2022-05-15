#ifndef GNO_MODELING_SIMPLE_MICRO_H
#define GNO_MODELING_SIMPLE_MICRO_H


#include "gno_modeling.h"

namespace graph
{
    class gno_modeling_simple_micro final: public gno_discrete_modeling_base
    {
        enum class state_t
        {
            wait,
            a_max,
            v_max,
            v_min,
            finished,
        };

        struct state
        {
            state_t st = state_t::wait;
            double velocity = 0.;
        };

        std::vector<state> m_machine_states;
        const double m_v_max;
        const double m_v_min;
        const double m_a_max;
        const double m_D;
        const double m_l;

        graph::uid model_independer_uid = graph::invalid_uid;

    public:
        gno_modeling_simple_micro (double v_max, double v_min, double a_max, double D_, double l_);

        void set_model_independer (graph::uid veh_uid) override { model_independer_uid = veh_uid; }

        void clear_states (const graph_initial &initial_state) override;

    private:
        void init_states ();

        bool is_finished (const graph_initial &initial_state) const override;
        int update_states (const graph_initial &initial_state) override;
    };
}

#endif // GNO_MODELING_SIMPLE_MICRO_H
