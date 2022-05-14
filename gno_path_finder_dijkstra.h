#ifndef GNO_PATH_FINDER_DIJKSTRA_H
#define GNO_PATH_FINDER_DIJKSTRA_H

#include "gno_path_finder.h"
#include "gno_graph_fwd.h"
#include <memory>

namespace graph
{
class gno_path_finder_dijkstra : public gno_path_finder_base
{
    double m_min_time = 0.;
    std::vector<graph::uid> m_min_path;
    gno_discrete_modeling_base *m_model = nullptr;

    std::vector<graph::vehicle_continuous_line_states> m_line_states;
    std::unique_ptr<graph::gno_continuous_modeling> m_continuous_modeling;

    std::unique_ptr<graph::graph_initial> m_updatable_initial;
    std::unique_ptr<graph::graph_initial_state_base> m_updatable_initial_state;

    bool m_use_depence = false;

public:
    gno_path_finder_dijkstra (gno_discrete_modeling_base *model, bool use_depence);

    std::vector<graph::uid> run (const graph_initial &initial_state, graph::uid src, graph::uid dst, Vehicle veh, double t);
};
}

#endif // GNO_PATH_FINDER_DIJKSTRA_H
