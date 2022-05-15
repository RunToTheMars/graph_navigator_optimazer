#ifndef GNO_PATH_FINDER_H
#define GNO_PATH_FINDER_H

#include "gno_graph_fwd.h"

namespace graph
{
class gno_path_finder_base
{
protected:
    int m_modeling_count = 0;

public:
  virtual std::vector<graph::uid> run (const graph_initial &initial_state,
                                       graph::uid src, graph::uid dst, Vehicle veh, double start_time) = 0;
};
}


#endif // GNO_PATH_FINDER_H
