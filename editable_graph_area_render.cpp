#include "editable_graph_area_render.h"
#include "graph_painter.h"
#include "gno_graph.h"
#include <QMouseEvent>

editable_graph_area_render::editable_graph_area_render (graph::graph_initial<> *graph_initial, QWidget *parent)
    : render_area_widget (parent)
{
  m_graph_painter = std::make_unique<graph_painter> (graph_initial->get_graph (), this);
  setMouseTracking (true);
}

editable_graph_area_render::~editable_graph_area_render () = default;

void editable_graph_area_render::set_show_names (bool show)
{
  m_graph_painter->set_show_names (show);
}

void editable_graph_area_render::mouseReleaseEvent (QMouseEvent *event)
{
  if (m_enable_drag)
  {
    m_graph_painter->mouse_drag_finished (event);
  }

  setCursor (Qt::ArrowCursor);
  m_in_drag = false;
  m_enable_drag = true;
}

void editable_graph_area_render::mousePressEvent (QMouseEvent */*event*/)
{
}

void editable_graph_area_render::mouseMoveEvent (QMouseEvent *event)
{
  m_graph_painter->mouse_pos_changed (event);

  if (!m_enable_drag)
    return;

  Qt::MouseButtons button = event->buttons ();

  if (button != Qt::MouseButton::LeftButton)
  {
    return;
  }

  if (!m_in_drag)
  {
    m_enable_drag = m_graph_painter->mouse_drag_started (event);
    m_in_drag = true;

    if (m_enable_drag)
      setCursor (Qt::PointingHandCursor);
  }
  else
  {
    m_enable_drag = m_graph_painter->mouse_drag (event);
    if (!m_enable_drag)
    {
      setCursor (Qt ::ArrowCursor);
      m_graph_painter->mouse_drag_finished (event);
    }
  }
}

void editable_graph_area_render::draw (QPainter &painter)
{
  m_graph_painter->draw (painter);
}


