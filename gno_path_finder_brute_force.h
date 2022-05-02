#ifndef GNO_PATH_FINDER_BRUTE_FORCE_H
#define GNO_PATH_FINDER_BRUTE_FORCE_H

#include "gno_path_finder.h"

namespace graph
{
class gno_path_finder_brute_force: public gno_path_finder_base
{
  double m_min_time = 0.;
  std::vector<graph::uid> m_min_path;
  gno_discrete_modeling_base *m_model = nullptr;

public:
  gno_path_finder_brute_force (gno_discrete_modeling_base *model);

  std::vector<graph::uid> run (const graph_initial &initial_state, graph::uid src, graph::uid dst, Vehicle veh, double t);
};
}

#endif // GNO_PATH_FINDER_BRUTE_FORCE_H
