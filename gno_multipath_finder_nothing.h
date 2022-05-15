#ifndef GNO_MULTIPATH_FINDER_NOTHING_H
#define GNO_MULTIPATH_FINDER_NOTHING_H

#include "gno_multipath_finder.h"

namespace graph
{
class gno_multipath_finder_nothing: public gno_multipath_finder_base
{
public:
    std::vector<std::vector<graph::uid>> run (const graph_initial &initial_state) override;
};
}


#endif // GNO_MULTIPATH_FINDER_NOTHING_H
