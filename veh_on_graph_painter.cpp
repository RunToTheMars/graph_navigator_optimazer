#include "veh_on_graph_painter.h"

#include "abstract_axis_painter.h"
#include "color_builder.h"
#include "utils.h"
#include <QColor>
#include <QPainter>

veh_on_graph_painter::veh_on_graph_painter (graph::graph_initial *graph_initial, render_area_widget *area): graph_painter (graph_initial, area)
{

}

void veh_on_graph_painter::draw (QPainter &painter)
{
    parent_t::draw (painter);
    draw_veh (painter);
    draw_start_end (painter);
}

static void draw_on_arc_arrow (QPainter &painter, QPointF start, QPointF end, size_t shift, double part, QColor color, graph::uid veh_id)
{
    QPointF diff = end - start;
    QPointF norm = {-diff.y (), diff.x ()};

    const double length = sqrt (diff.x () * diff.x () + diff.y () * diff.y ());
    if (fuzzy_eq (length, 0.))
      return;

    QPointF median = start + 0.5 * diff;
    QPointF angle_point = median + norm * shift * 0.05;

    QPointF on_arc_arrow_point;
    if (part < 0.5)
      on_arc_arrow_point = (angle_point - start) * 2 * part + start;
    else
      on_arc_arrow_point = (end - angle_point) * 2 * (part - 0.5) + angle_point;

    painter.setBrush (color);
    painter.drawEllipse (on_arc_arrow_point, 1.5 * graph_painter::point_size, 1.5 * graph_painter::point_size);

    QString name = QString ("%1").arg (veh_id);

    QFontMetrics fm = painter.fontMetrics ();
    on_arc_arrow_point.setX (on_arc_arrow_point.x () - fm.horizontalAdvance (name) / 2);
    on_arc_arrow_point.setY (on_arc_arrow_point.y () + fm.height () / 2 - 1);

    QColor font_color;

    int black = color.black ();
    if (black > 127)
        font_color = QColor ("white");
    else
        font_color = QColor ("black");

    painter.setPen (font_color);
    painter.drawText (on_arc_arrow_point, name);
}

void veh_on_graph_painter::draw_veh (QPainter &painter)
{
  if (!m_line_states)
    return;

  if (m_line_states->size () == 0)
    return;

  double time = m_cur_time;
  if (time > m_line_states->back ().t2)
      time = m_line_states->back ().t2;

  for (size_t i = 0; i < m_line_states->size (); i++)
  {
      const auto &line_state = m_line_states->at (i);
      if (time < line_state.t1 || line_state.t2 < time)
        continue;

      const auto &states = line_state.states;

      default_color_builder color_builder;
      color_builder.set_val_range (0., static_cast<double> (states.size ()));

      for (graph::uid veh_id = 0; veh_id < isize (states); veh_id ++)
      {
        auto edge_part =  states[veh_id].get_edge_and_par (time, line_state.t1, line_state.t2);
        const graph::uid edge_uid = edge_part.first == 0? states[veh_id].edge_uid_start : states[veh_id].edge_uid_end;
        const graph::Edge edge = m_graph->edge (edge_uid);

        const graph::Node node_1 = m_graph->node (edge.start);
        const graph::Node node_2 = m_graph->node (edge.end);

        //to do: check order with graph_painter
        int edge_num = 0;
        const auto &edges = m_graph->edges (edge.start, edge.end);
        for (int i = 0; i < isize (edges); i++)
        {
          if (edges[i] == edge_part.first)
          {
            edge_num = i;
            break;
          }
        }

        QPointF start = m_axis_painter->get_screen_pos (node_1.x, node_1.y);
        QPointF end = m_axis_painter->get_screen_pos (node_2.x, node_2.y);
        QColor color = color_builder.get_color (static_cast<double> (veh_id));

        draw_on_arc_arrow (painter, start, end, edge_num + 1, edge_part.second, color, veh_id);
      }
  }
}

void veh_on_graph_painter::draw_start_end (QPainter &painter)
{
    if (!m_show_src_dst)
      return;

    draw_start_end (painter, m_src_node, "S");
    draw_start_end (painter, m_dst_node, "E");
}

void veh_on_graph_painter::draw_start_end (QPainter &painter, graph::uid node_uid, QString text)
{
    if (!m_graph->is_correct_node_uid (node_uid))
      return;

    const graph::Node &node = m_graph->node (node_uid);
    QPointF screen_pos = m_axis_painter->get_screen_pos (node.x, node.y);

    painter.setBrush (QColor ("black"));
    painter.drawEllipse (screen_pos, 1.5 * graph_painter::point_size, 1.5 * graph_painter::point_size);

    QFontMetrics fm = painter.fontMetrics ();
    screen_pos.setX (screen_pos.x () - fm.horizontalAdvance (text) / 2);
    screen_pos.setY (screen_pos.y () + fm.height () / 2 - 1);

    painter.setPen (QColor ("white"));
    painter.drawText (screen_pos, text);
}
