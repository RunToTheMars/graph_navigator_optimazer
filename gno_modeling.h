#ifndef GNO_MODELING_H
#define GNO_MODELING_H

#include "gno_graph_initial.h"
#include "gno_graph_initial_state.h"
#include "utils.h"
#include <vector>
#include <functional>
#include <cassert>

namespace graph
{
class gno_discrete_modeling_base
{
    using func_type = std::function<void (double /*time*/, const std::vector<vehicle_discrete_state> &/*states*/)>;
    func_type m_do_in_critical_time;

    bool m_interrupted = false;

protected:
    std::vector<vehicle_discrete_state> m_states;
    double m_t = 0.;

public:
    void set_do_in_critical_time (func_type do_in_critical_time) { m_do_in_critical_time = do_in_critical_time; }
    int run (const graph_initial &initial_state);

    virtual void set_model_independer (graph::uid /*veh_uid*/) { assert (false); }
    void interrupt () { m_interrupted = true; }

    void set_states (double t, const std::vector<vehicle_discrete_state> &states) { m_t = t; m_states = states; }
    virtual void clear_states (const graph_initial &initial_state) = 0;

private:
    virtual bool is_finished (const graph_initial &initial_state) const = 0;
    virtual int update_states (const graph_initial &initial_state) = 0;
};

class gno_continuous_modeling final
{
    using func_type = std::function<void (vehicle_continuous_line_states)>;
    func_type m_do_on_linear_time;

    gno_discrete_modeling_base *m_discrete_modeling = nullptr;

    std::vector<vehicle_discrete_state> m_prev_discrete_states;
    double m_prev_time;

    size_t m_cricital_count = 0;

public:
    gno_continuous_modeling (gno_discrete_modeling_base *discrete_modeling);

    gno_discrete_modeling_base *discrete_model () { return m_discrete_modeling; }

    void set_do_on_linear_time (func_type do_on_linear_time) { m_do_on_linear_time = do_on_linear_time; }
    int run (const graph_initial &initial_state);
};
}


#endif // GNO_MODELING_H
