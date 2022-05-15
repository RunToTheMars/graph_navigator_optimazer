#include "gno_multipath_finder_nothing.h"
#include "gno_graph_initial.h"
#include "gno_graph_initial_state.h"

namespace graph
{
  std::vector<std::vector<graph::uid>>
  gno_multipath_finder_nothing::run (const graph_initial &initial_state)
  {
    const graph_initial_state_base *initial = initial_state.get_initial_state ();
    std::vector<std::vector<graph::uid>> res;
    for (graph::uid veh_uid = 0; veh_uid < initial->vehicle_count (); veh_uid++)
      res.push_back (initial->vehicle (veh_uid).path);

    return res;
  }
}

