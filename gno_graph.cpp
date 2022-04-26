#include "gno_graph.h"

namespace graph
{
const std::vector<graph::uid> graph_impl::edges (const uid start, const uid end) const
{
    const std::vector<graph::uid> &edges_with_start = edges_started_from (start);
    std::vector<graph::uid> edges_with_start_end;
    for (graph::uid edge_uid: edges_with_start)
    {
      Edge e = edge (edge_uid);
      if (e.end == end)
      {
        edges_with_start_end.push_back (edge_uid);
      }
    }

    return edges_with_start_end;
}
}

