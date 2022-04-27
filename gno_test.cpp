#include "gno_test.h"

#include "gno_graph.h"
#include "gno_graph_initial.h"
#include "gno_graph_initial_state.h"

namespace graph
{
  int check_path (const graph_initial &initial)
  {
    const graph_base *graph = initial.get_graph ();
    const graph_initial_state_base *initial_states = initial.get_initial_state ();

    for (graph::uid veh_id = 0; veh_id < initial_states->vehicle_count (); veh_id++)
    {
      const graph::Directional_Vehicle &veh = initial_states->vehicle (veh_id);

      if (veh.src == graph::invalid_uid)
        return -1;

      if (veh.dst == graph::invalid_uid)
        return -2;

      if (veh.path.size () == 0)
        return -3;

      if (veh.src != graph->edge (veh.path.front ()).start)
        return -4;

      if (veh.dst != graph->edge (veh.path.back ()).end)
        return -5;

      for (size_t i = 0; i < veh.path.size () - 1; i++)
        {
          if (graph->edge (veh.path[i]).end != graph->edge (veh.path[i + 1]).start)
            return -6;
        }
    }
    return 0;
  }
} // namespace graph
