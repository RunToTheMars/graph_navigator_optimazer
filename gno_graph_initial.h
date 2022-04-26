#ifndef GNO_GRAPH_INITIAL_H
#define GNO_GRAPH_INITIAL_H

#include "gno_graph_fwd.h"
#include <memory>

namespace graph
{

class graph_base;
class graph_initial_state_base;

class graph_initial
{
    graph_base *m_graph;
    graph_initial_state_base *m_initial_state;

public:
  graph_initial (graph_base *graph, graph_initial_state_base *initial_state)
  {
    m_graph = graph;
    m_initial_state = initial_state;
  }

    graph_base *get_graph () { return m_graph; }
    const graph_base *get_graph () const { return m_graph; }

    graph_initial_state_base *get_initial_state () { return m_initial_state; }
    const graph_initial_state_base *get_initial_state () const { return m_initial_state; }
};
}
#endif // GNO_GRAPH_INITIAL_H
