#ifndef GNO_MODELING_SIMPLE_ACCELERATION_H
#define GNO_MODELING_SIMPLE_ACCELERATION_H

#include "gno_modeling.h"

namespace graph
{
class gno_modeling_simple_acceleration: public gno_discrete_modeling_base
{
    double m_start_velocity = 0.;
    double m_start_acc = 0.;

    std::vector<double> m_velocities;
    std::vector<double> m_accelerations;

public:
    void set_start_velocity (double start_velocity) { m_start_velocity = start_velocity; }
    void set_start_acc (double acc) { m_start_acc = acc; }

    void clear_states (const graph_initial &initial_state) override;

private:
    bool is_finished (const graph_initial &initial_state) const override;
    int update_states (const graph_initial &initial_state) override;
};
}
#endif // GNO_MODELING_SIMPLE_ACCELERATION_H
