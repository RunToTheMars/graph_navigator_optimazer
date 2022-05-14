#ifndef GNO_PATH_FINDER_BRUTE_FORCE_H
#define GNO_PATH_FINDER_BRUTE_FORCE_H

#include "gno_path_finder.h"

namespace graph
{
class gno_path_finder_brute_force: public gno_path_finder_base
{
  static constexpr size_t max_path_size = 10;

  double m_min_time = 0.;
  std::vector<graph::uid> m_min_path;
  gno_discrete_modeling_base *m_model = nullptr;
  bool m_use_depence = true;

public:
  gno_path_finder_brute_force (gno_discrete_modeling_base *model, bool use_depence);

  std::vector<graph::uid> run (const graph_initial &initial_state, graph::uid src, graph::uid dst, Vehicle veh, double t);
};
}

#endif // GNO_PATH_FINDER_BRUTE_FORCE_H
