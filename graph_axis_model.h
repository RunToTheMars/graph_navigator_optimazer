#ifndef GRAPH_AXIS_MODEL_H
#define GRAPH_AXIS_MODEL_H

#include "abstract_axis_model.h"
#include "gno_graph.h"

class graph_axis_model: public abstract_axis_model
{
  graph::graph_base *m_graph = nullptr;

public:
  graph_axis_model (graph::graph_base *graph);
  ~graph_axis_model ();

  QVariant data (axis_settings setting) const override;
};

#endif // GRAPH_AXIS_MODEL_H
