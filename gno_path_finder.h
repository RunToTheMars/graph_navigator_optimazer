#ifndef GNO_PATH_FINDER_H
#define GNO_PATH_FINDER_H

#include "gno_graph_fwd.h"
#include "functional"

namespace graph
{
class gno_path_finder_base
{
public:
   int m_modeling_count = 0;

protected:
  std::function<double (std::vector<double>)> m_phi;

public:
  virtual std::vector<graph::uid> run (const graph_initial &initial_state, graph::uid veh_uid) = 0;

  void set_phi (std::function<double (const std::vector<double> &)> phi) { m_phi = phi; }
};
}


#endif // GNO_PATH_FINDER_H
