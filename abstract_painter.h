#ifndef ABSTRACT_PAINTER_H
#define ABSTRACT_PAINTER_H

class QWidget;
class QPainter;
class QRect;

class render_area_widget;
class QMouseEvent;

class abstract_painter
{
public:
  abstract_painter (render_area_widget *area);

  virtual ~abstract_painter () = default;
  virtual void draw (QPainter &painter) = 0;

  render_area_widget *get_render_area () const { return m_render_area_widget; }

  virtual void update ();

protected:
  virtual void mouse_pos_changed (QMouseEvent *event);

  virtual bool mouse_drag_started (QMouseEvent *event);
  virtual bool mouse_drag (QMouseEvent *event);
  virtual void mouse_drag_finished (QMouseEvent *event);

private:
  render_area_widget *m_render_area_widget = nullptr;
};

#endif // ABSTRACT_PAINTER_H
