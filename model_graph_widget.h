#ifndef MODEL_GRAPH_WIDGET_H
#define MODEL_GRAPH_WIDGET_H

#include <QWidget>
#include "gno_graph_fwd.h"

class model_graph_area_render;
class veh_on_graph_painter;

class model_graph_widget: public QWidget
{
  Q_OBJECT

  graph::gno_discrete_modeling_base *m_discrete_modeling = nullptr;
  std::unique_ptr<graph::gno_continuous_modeling> m_continuous_modeling;

  model_graph_area_render *area_render = nullptr;

  std::vector<graph::vehicle_continuous_line_states> m_line_states;

public:
  model_graph_widget (graph::graph_initial *graph_initial, graph::gno_discrete_modeling_base *discrete_modeling, QWidget *parent = nullptr);
  ~model_graph_widget ();

  const std::vector<graph::vehicle_continuous_line_states> &get_line_states () const { return m_line_states; }
  veh_on_graph_painter *get_painter ();
  void set_time (double t);


  Q_SIGNAL void update_times_needed ();
};

#endif // MODEL_GRAPH_WIDGET_H
