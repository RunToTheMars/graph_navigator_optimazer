#ifndef GNO_MULTIPATH_FINDER_LOOP_H
#define GNO_MULTIPATH_FINDER_LOOP_H

#include "gno_multipath_finder.h"
#include <memory>

namespace graph
{
class gno_multipath_finder_loop: public gno_multipath_finder_base
{
    graph::gno_path_finder_base *m_path_finder = nullptr;
    graph::gno_continuous_modeling *m_continuous_modeling = nullptr;

    std::unique_ptr<graph::graph_initial_state_base> m_editable_graph_initial_state;
    std::unique_ptr<graph::graph_initial> m_editable_graph_initial;

public:
    gno_multipath_finder_loop (graph::gno_path_finder_base *path_finder, graph::gno_continuous_modeling *continuous_modeling);
    std::vector<std::vector<graph::uid>> run (const graph_initial &initial_state) override;

private:
    double average (graph::graph_initial *initial);
};
}

#endif // GNO_MULTIPATH_FINDER_LOOP_H
