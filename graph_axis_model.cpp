#include "graph_axis_model.h"

#include <QVariant>
#include <QColor>
#include <optional>

graph_axis_model::graph_axis_model (graph::graph_base<> *graph): m_graph (graph)
{
}

graph_axis_model::~graph_axis_model () = default;

QVariant graph_axis_model::data (axis_settings setting) const
{
  const double margin_x = 0.2 * (m_graph->max_x () - m_graph->min_x ());
  const double margin_y = 0.2 * (m_graph->max_y () - m_graph->min_y ());

  switch (setting)
  {
  case axis_settings::draw_grid:
    return true;
  case axis_settings::min_x:
    return m_graph->min_x () - margin_x;
  case axis_settings::max_x:
    return m_graph->max_x () + margin_x;
  case axis_settings::min_y:
    return m_graph->min_y () - margin_y;
  case axis_settings::max_y:
    return m_graph->max_y () + margin_y;
  case axis_settings::background_color:
    return QColor ("white");
  case axis_settings::name_x:
    return "X";
  case axis_settings::name_y:
    return "Y";
  case axis_settings::precision:
    return 1;
  }
  return {};
}
