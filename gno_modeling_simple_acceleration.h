#ifndef GNO_MODELING_SIMPLE_ACCELERATION_H
#define GNO_MODELING_SIMPLE_ACCELERATION_H

#include "gno_modeling.h"

namespace graph
{
class gno_modeling_simple_acceleration: public gno_discrete_modeling_base
{
    double m_t = 0.;
    std::vector<vehicle_discrete_state> m_states;

    double m_start_velocity = 0.;
    double m_start_acc = 0.;

    std::vector<double> m_velocities;
    std::vector<double> m_accelerations;

public:
    int run (const graph_initial &initial_state) override;

    void set_start_velocity (double start_velocity) { m_start_velocity = start_velocity; }
    void set_start_acc (double acc) { m_start_acc = acc; }

private:
    bool is_finished (const graph_initial &initial_state) const;
    int do_step (const graph_initial &initial_state);
};
}
#endif // GNO_MODELING_SIMPLE_ACCELERATION_H
