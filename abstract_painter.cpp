#include "abstract_painter.h"

#include "render_area_widget.h"

abstract_painter::abstract_painter (render_area_widget *area): m_render_area_widget (area)
{
}

void abstract_painter::update ()
{
  m_render_area_widget->update ();
}

void abstract_painter::mouse_pos_changed (QMouseEvent */*event*/)
{

}

bool abstract_painter::mouse_drag_started (QMouseEvent */*event*/)
{
  return false;
}

bool abstract_painter::mouse_drag (QMouseEvent */*event*/)
{
  return false;
}

void abstract_painter::mouse_drag_finished (QMouseEvent */*event*/)
{

}

