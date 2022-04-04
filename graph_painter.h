#ifndef GRAPH_PAINTER_H
#define GRAPH_PAINTER_H

#include "abstract_painter.h"
#include <memory>
#include <gno_graph.h>
#include <QPointF>

class abstract_axis_painter;
class abstract_axis_model;

class QMouseEvent;

class graph_painter : public abstract_painter
{
public:
  static constexpr int point_border_width = 2;
  static constexpr int point_size = 5;
  static constexpr int edge_width = 2;

  //for handle node
  static constexpr double min_handle_node_distance = 2 * point_size;
  static constexpr int handle_point_size = 7;
  static constexpr double scale_handle_point = 1.5;

private:

  std::unique_ptr<abstract_axis_painter> m_axis_painter;
  std::unique_ptr<abstract_axis_model>   m_axis_model;

  //graph
  graph::graph_base<> *m_graph = nullptr;

  //settings
  bool m_show_names = false;

  //handle
  graph::uid m_handle_node = graph::invalid_uid;

  //drag
  graph::uid m_drag_node = graph::invalid_uid;
  QPointF m_start_drag_pos;
  QPointF m_move_drag_pos;

public:
  graph_painter (graph::graph_base<> *graph, render_area_widget *area);
  ~graph_painter ();

  void mouse_pos_changed (QMouseEvent *event) override;

  bool mouse_drag_started (QMouseEvent *event) override;
  bool mouse_drag (QMouseEvent *event) override;
  void mouse_drag_finished (QMouseEvent *event) override;

  void draw (QPainter &painter) override;
  void draw_nodes (QPainter &painter);
  void draw_edges (QPainter &painter);
  void draw_names (QPainter &painter);

  void set_show_names (bool show) { m_show_names = show; }

private:
  graph::uid node_uid_under_mouse (QPointF screen_pos) const;
};

#endif // GRAPH_PAINTER_H
