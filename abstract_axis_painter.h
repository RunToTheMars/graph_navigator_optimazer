#ifndef ABSTRACT_AXIS_PAINTER_H
#define ABSTRACT_AXIS_PAINTER_H

#include "abstract_painter.h"
#include <memory>

//#define AXIS_DEBUG_MODE

#include <QRectF>

class abstract_axis_model;
class axis_painter_x;
class axis_painter_y;

class abstract_axis_painter: public abstract_painter
{
public:
    static constexpr int default_margin_vertical = 5;
    static constexpr int default_margin_horizontal = 5;

    static constexpr int default_segments = 10;

   std::unique_ptr<abstract_axis_model> m_model;
   QRectF m_draw_rect;

   //additional painters
   std::unique_ptr<axis_painter_x> m_axis_painter_x;
   std::unique_ptr<axis_painter_y> m_axis_painter_y;

public:
    abstract_axis_painter (render_area_widget *area);

    void draw (QPainter &painter) override;
    void set_model (abstract_axis_model *model);

    const abstract_axis_model *get_model () const { return m_model.get (); }

    QRectF get_initial_rect () const;
    QRectF get_draw_rect () const { return m_draw_rect; }

    QPointF get_screen_pos (double x, double y) const;
    void get_local_pos (QPointF screen_pos, double &x, double &y) const;

private:

    void update_rect (QPainter &painter);

    void configure_axis (QPainter &painter);

    void set_clipping (QPainter &painter) const;
    void draw_background (QPainter &painter) const;
    void draw_axis (QPainter &painter) const;
};

class axis_painter
{
protected:
  abstract_axis_painter *m_axis_painter = nullptr;

public:
  axis_painter (abstract_axis_painter *axis_painter)
      : m_axis_painter (axis_painter)
  {}

  virtual ~axis_painter () = default;

  virtual int text_top_spacing () { return 2; };
  virtual int text_bottom_spacing () { return 3; };

  virtual int text_left_spacing () { return 2; };
  virtual int text_right_spacing () { return 3; };

  virtual int value_top_spacing () { return 5; }
  virtual int value_bottom_spacing () { return 1; }

  virtual int value_left_spacing () { return 3; }
  virtual int value_right_spacing () { return 3; }

  virtual int min_segment_one_side_size () { return 3; }

  virtual void draw (QPainter &painter) = 0;
};


class axis_painter_x final: public axis_painter
{
public:
  axis_painter_x (abstract_axis_painter *axis_area_painter);
  void draw (QPainter &painter) override;
};

class axis_painter_y final: public axis_painter
{
  int m_max_width = 0;

public:
  axis_painter_y (abstract_axis_painter *axis_painter);
  void draw (QPainter &painter) override;
  void set_max_width (int max_width) { m_max_width = max_width; }
};

#endif // ABSTRACT_AXIS_PAINTER_H
