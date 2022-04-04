#ifndef GNO_GRAPH_INITIAL_H
#define GNO_GRAPH_INITIAL_H

#include "gno_graph_fwd.h"
#include <memory>

namespace graph
{

template <typename Node, typename Edge>
class graph_base;

template <typename Vehicle>
class graph_initial_state_base;

template <typename Node = Node_Base, typename Edge = Edge_Base, typename Vehicle = Vehicle_Base>
class graph_initial
{
    graph_base<Node, Edge> *m_graph;
    graph_initial_state_base<Vehicle> *m_initial_state;

public:
  graph_initial (graph_base<Node, Edge> *graph, graph_initial_state_base<Vehicle> *initial_state)
  {
    m_graph = graph;
    m_initial_state = initial_state;
  }

    graph_base<Node, Edge> *get_graph () { return m_graph; }
    const graph_base<Node, Edge> *get_graph () const { return m_graph; }

    graph_initial_state_base<Vehicle> *get_initial_state () { return m_initial_state; }
    const graph_initial_state_base<Vehicle> *get_initial_state () const { return m_initial_state; }
};
}
#endif // GNO_GRAPH_INITIAL_H
