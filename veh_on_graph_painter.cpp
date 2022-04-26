#include "veh_on_graph_painter.h"

#include "abstract_axis_painter.h"
#include "color_builder.h"
#include "utils.h"
#include <QColor>
#include <QPainter>

veh_on_graph_painter::veh_on_graph_painter (graph::graph_base *graph, render_area_widget *area): graph_painter (graph, area)
{

}

void veh_on_graph_painter::draw (QPainter &painter)
{
    parent_t::draw (painter);
    draw_veh (painter);
}

static void draw_on_arc_arrow (QPainter &painter, QPointF start, QPointF end, size_t shift, double part, QColor color)
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
    painter.drawEllipse (on_arc_arrow_point, graph_painter::point_size, graph_painter::point_size);
}

void veh_on_graph_painter::draw_veh (QPainter &painter)
{
  if (!m_line_states)
    return;

  for (size_t i = 0; i < m_line_states->size (); i++)
  {
      const auto &line_state = m_line_states->at (i);
      if (m_cur_time < line_state.t1 || line_state.t2 < m_cur_time)
        continue;

      auto get_edge_and_part = [this, &line_state] (const graph::vehicle_continuous_state &state) -> std::pair<graph::uid, double>
      {
          if (state.edge_uid_start == state.edge_uid_end)
            return  {state.edge_uid_end, (state.part_end - state.part_start) * (m_cur_time - line_state.t1) / (line_state.t2 - line_state.t1) + state.part_start};
          else
          {
              double p = (state.part_end - state.part_start + 1) * (m_cur_time - line_state.t1) / (line_state.t2 - line_state.t1);
              if (p < 1 - state.part_start)
                return  {state.edge_uid_start, state.part_start + p};
              else
                return {state.edge_uid_end, p - 1 + state.part_start};
          }
      };

      const auto &states = line_state.states;

      default_color_builder color_builder;
      color_builder.set_val_range (0., static_cast<double> (states.size ()));

      for (graph::uid veh_id = 0; veh_id < states.size (); veh_id ++)
      {
        auto edge_part = get_edge_and_part (states[veh_id]);
        const graph::Edge edge = m_graph->edge (edge_part.first);

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

        draw_on_arc_arrow (painter, start, end, edge_num + 1, edge_part.second, color);
      }
  }
}