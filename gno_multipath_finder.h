#ifndef GNO_MULTIPATH_FINDER_H
#define GNO_MULTIPATH_FINDER_H

#include "gno_graph_fwd.h"

namespace graph
{
class gno_multipath_finder_base
{
protected:
    int m_modeling_count = 0;

public:
    virtual std::vector<std::vector<graph::uid>> run (const graph_initial &initial_state) = 0;
};
}


#endif // GNO_MULTIPATH_FINDER_H
