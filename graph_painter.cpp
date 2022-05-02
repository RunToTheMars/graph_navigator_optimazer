#include "graph_painter.h"

#include "gno_graph_initial.h"
#include "gno_graph_initial_state.h"
#include "graph_axis_model.h"
#include "abstract_axis_painter.h"
#include "array"
#include "utils.h"
#include <cmath>

#include <QPainter>
#include <QPen>
#include <QMouseEvent>

graph_painter::graph_painter (graph::graph_initial *graph_initial, render_area_widget *area)
    : abstract_painter (area), m_graph (graph_initial->get_graph ()), m_graph_initial_state (graph_initial->get_initial_state ())
{
    m_axis_model = std::make_unique<graph_axis_model> (graph_initial->get_graph ());
    m_axis_painter = std::make_unique<abstract_axis_painter> (area);
    m_axis_painter->set_model (m_axis_model.get ());
}

graph_painter::~graph_painter () = default;

void graph_painter::draw (QPainter &painter)
{
  m_axis_painter->draw (painter);
  draw_edges (painter);
  draw_nodes (painter);

  if (m_show_names || m_show_numbers)
    draw_names (painter);

  if (m_show_path)
    draw_path (painter);
}

void graph_painter::draw_nodes (QPainter &painter)
{
  painter.save ();

  QPen pen;
//  pen.setColor (QColor ("black"));
//  pen.setWidth (point_border_width);

  QBrush brush;
  brush.setColor (QColor ("blue"));
  brush.setStyle (Qt::SolidPattern);

  painter.setPen (pen);
  painter.setBrush (brush);

  double ps = get_point_size ();
  double hps = get_handle_point_size ();

  for (graph::uid i = 0; i < m_graph->node_count (); i++)
  {
    const graph::Node &node = m_graph->node (i);
    QPointF screen_pos = m_axis_painter->get_screen_pos (node.x, node.y);

    if (m_handle_node == i)
      continue;
    painter.drawEllipse (screen_pos, ps, ps);
  }

  if (m_graph->is_correct_node_uid (m_handle_node))
  {
      const graph::Node &node = m_graph->node (m_handle_node);
      QPointF screen_pos = m_axis_painter->get_screen_pos (node.x, node.y);

      QPen handle_pen;
      handle_pen.setColor (QColor ("yellow"));
      handle_pen.setWidth (point_border_width);

      QBrush handle_brush;
      handle_brush.setColor (QColor ("green"));
      handle_brush.setStyle (Qt::SolidPattern);

      //set
      painter.setPen (handle_pen);
      painter.setBrush (handle_brush);

      painter.drawEllipse (screen_pos, hps, hps);
  }

  painter.restore ();
}

static void draw_arrow (QPainter &painter, QPointF start, QPointF end, double point_size, double edge_length, double start_length)
{
  painter.drawLine (start, end);

  QPointF diff = end - start;
  const double initial_line_length = sqrt (diff.x () * diff.x () + diff.y () * diff.y ());

  if (initial_line_length < point_size)
    return;

  double line_length = initial_line_length - point_size;
  if (fuzzy_eq (line_length, 0.))
    return;

  end = diff * (line_length / initial_line_length) + start;
  diff = end - start;

  const double r_line_length = 1. / line_length;

  const double arrow_length = 1.5 * point_size;
  const double min_length = 3;
  double start_draw_arrow_length = line_length * 0.5;
  if (line_length > min_length)
    start_draw_arrow_length = line_length - arrow_length;

  double start_draw_arrow_t = start_draw_arrow_length * r_line_length;

  QPointF start_draw_arrow = start + diff * start_draw_arrow_t;

  QPointF ortho = {-diff.y (), diff.x ()};
  ortho *= r_line_length * (line_length - start_draw_arrow_length);

  std::array<QPointF, 3> arrow_triangle;
  arrow_triangle[0] = start_draw_arrow + 0.5 * ortho;
  arrow_triangle[1] = start_draw_arrow - 0.5 * ortho;
  arrow_triangle[2] = end;

  painter.drawPolygon (arrow_triangle.data (), isize (arrow_triangle));

  ortho /= sqrt (QPointF::dotProduct (ortho, ortho));

  for (int i = 0; i * graph::D < edge_length; i++)
  {
    if (i * graph::D < start_length)
      continue;

    double part = (i * graph::D - start_length) / (edge_length - start_length);
    painter.drawLine (start + (end - start) * part + ortho, start + diff * part - ortho);
  }
}

static void draw_arc_arrow (QPainter &painter, QPointF start, QPointF end, size_t shift, double point_size, double edge_length)
{
    QPointF diff = end - start;
    QPointF norm = {-diff.y (), diff.x ()};

    const double length = sqrt (diff.x () * diff.x () + diff.y () * diff.y ());
    if (fuzzy_eq (length, 0.))
      return;

    QPointF median = start + 0.5 * diff;
    QPointF angle_point = median + norm * shift * 0.05;

    draw_arrow (painter, start, angle_point, point_size, edge_length * 0.5, 0.);
    draw_arrow (painter, angle_point, end, point_size, edge_length, 0.5 * edge_length);
}

void graph_painter::draw_edges (QPainter &painter)
{
    painter.save ();

    QPen pen;
    pen.setColor (QColor ("black"));
    pen.setWidth (edge_width * m_edge_width_mult);

    QBrush brush;
    brush.setColor (QColor ("black"));
    brush.setStyle (Qt::SolidPattern);

    painter.setPen (pen);
    painter.setBrush (brush);
//    for (graph::uid i = 0; i < m_graph->edge_count (); i++)
//    {
//        const graph::Edge &edge = m_graph->edge (i);

//        const graph::Node node_1 = m_graph->node (edge.start);
//        const graph::Node node_2 = m_graph->node (edge.end);

//        QPointF start = m_axis_painter->get_screen_pos (node_1.x, node_1.y);
//        QPointF end = m_axis_painter->get_screen_pos (node_2.x, node_2.y);

//        draw_arrow (painter, start, end);
//    }

    for (graph::uid i = 0; i < m_graph->node_count (); i++)
    {
        const std::vector<graph::uid> &uids = m_graph->edges_started_from (i);

        std::vector<std::pair<graph::uid, graph::uid>> uids_map;

        for (graph::uid j: uids)
        {
          const graph::Edge e = m_graph->edge (j);
          uids_map.push_back ({j, e.end});
        }

        std::sort (uids_map.begin (), uids_map.end (),
                   [] (std::pair<int, int> a, std::pair<int, int> b) {
                     return a.second < b.second;
                   });

        for (size_t i = 0; i < uids_map.size ();)
        {
            size_t j = i;
            while (j + 1 < uids_map.size () && uids_map[i].second == uids_map[j + 1].second)
            {
              j++;
            }

            for (size_t k = i; k <= j; k++)
            {
              const graph::Edge edge = m_graph->edge (uids_map[k].first);

              const graph::Node node_1 = m_graph->node (edge.start);
              const graph::Node node_2 = m_graph->node (edge.end);

              QPointF start = m_axis_painter->get_screen_pos (node_1.x, node_1.y);
              QPointF end = m_axis_painter->get_screen_pos (node_2.x, node_2.y);

              draw_arc_arrow (painter, start, end, k - i + 1, get_point_size (), m_graph->length (uids_map[k].first));
            }

            i = j + 1;
        }
    }
    painter.restore ();
}

void graph_painter::draw_names (QPainter &painter)
{
    QFontMetrics fm = painter.fontMetrics ();

    for (graph::uid i = 0; i < m_graph->node_count (); i++)
    {
        const graph::Node &node = m_graph->node (i);
        QPointF screen_pos = m_axis_painter->get_screen_pos (node.x, node.y);

        QString name = m_show_numbers ? std::to_string(i).c_str () :  node.name.c_str ();

        screen_pos.setX (screen_pos.x () - fm.horizontalAdvance (name) / 2);
        screen_pos.setY (screen_pos.y () - point_size - 3);

        painter.drawText (screen_pos, name);
    }
}

void graph_painter::draw_path (QPainter &painter)
{
    QColor font_color;

    int black = m_path_color.black ();
    if (black > 127)
      font_color = QColor ("white");
    else
      font_color = QColor ("black");

    QFontMetrics fm = painter.fontMetrics ();
    for (size_t i = 0; i < m_path.size (); i++)
    {

        bool is_first_or_last = i == 0 || i == m_path.size () - 1;

        const graph::uid edge_uid = m_path[i];
        const graph::Edge &edge = m_graph->edge (edge_uid);
        const graph::Node &node_start = m_graph->node (edge.start);
        const graph::Node &node_end = m_graph->node (edge.end);

        const QPointF start = m_axis_painter->get_screen_pos (node_start.x, node_start.y);
        const QPointF end = m_axis_painter->get_screen_pos (node_end.x, node_end.y);

        int k = 0;
        const std::vector<graph::uid> &edges = m_graph->edges (edge.start, edge.end);
        for (k = 0; k < edges.size (); k++)
        {
          if (edges[k] == edge_uid)
            break;
        }

        const QPointF diff = end - start;
        const QPointF norm = {-diff.y (), diff.x ()};

        const QPointF median = start + 0.5 * diff;
        QPointF angle_point = median + norm * (k + 1) * 0.05;

        painter.setBrush (m_path_color);

        if (is_first_or_last)
        {
            painter.setPen({QColor ("yellow"), 2});

            painter.drawEllipse (angle_point, 2 * get_point_size (), 2 * get_point_size ());
        }
        else
            painter.drawEllipse (angle_point, 1.5 * get_point_size (), 1.5 * get_point_size ());

        QString name = QString ("%1").arg (i);

        angle_point.setX (angle_point.x () - fm.horizontalAdvance (name) / 2);
        angle_point.setY (angle_point.y () + fm.height () / 2 - 1);

        painter.setPen (font_color);
        painter.drawText (angle_point, name);
    }

    painter.restore ();
}

graph::uid graph_painter::node_uid_under_mouse (QPointF mouse_pos) const
{
  double min_distance_squared = min_handle_node_distance * min_handle_node_distance;
  graph::uid handle_node = graph::invalid_uid;

  for (graph::uid i = 0; i < m_graph->node_count (); i++)
  {
    const graph::Node &node = m_graph->node (i);
    QPointF screen_pos = m_axis_painter->get_screen_pos (node.x, node.y);

    QPointF diff = mouse_pos - screen_pos;
    double cur_distance_squared = diff.x () * diff.x () + diff.y () * diff.y ();
    if (cur_distance_squared < min_distance_squared)
    {
      min_distance_squared = cur_distance_squared;
      handle_node = i;
    }
  }

  return handle_node;
}

void graph_painter::mouse_pos_changed (QMouseEvent *event)
{
  const QPoint mp = event->pos ();

  graph::uid new_handle_node = node_uid_under_mouse (mp);
  if (new_handle_node != m_handle_node)
  {
    m_handle_node = new_handle_node;
    update ();
  }
}

bool graph_painter::mouse_drag_started (QMouseEvent *event)
{
  if (!m_is_editable)
    return false;

  m_drag_node = node_uid_under_mouse (event->pos ());

  if (!m_graph->is_correct_node_uid (m_drag_node))
    return false;

  m_start_drag_pos = event->pos ();

  return true;
}

bool graph_painter::mouse_drag (QMouseEvent *event)
{
  if (!m_is_editable)
    return false;

  if (!m_graph->is_correct_node_uid (m_drag_node))
    return false;

  graph::Node &node = m_graph->node (m_drag_node);
  m_axis_painter->get_local_pos (event->pos (), node.x, node.y);

  update ();
  return true;
}

void graph_painter::mouse_drag_finished (QMouseEvent */*event*/)
{
  if (!m_is_editable)
    return;

  m_graph->calculate_bounds ();
  update ();
}
