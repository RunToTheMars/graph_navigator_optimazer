#include "abstract_axis_painter.h"

#include "abstract_axis_model.h"
#include "abstract_painter_holder.h"
#include "utils.h"

#include "render_area_widget.h"

#include <QPen>
#include <QPainter>
#include <QVariant>
#include <QString>

inline QString value_to_string (double val, char format = 'g',
                                int precision = -1)
{
  return QString("%1").arg(val, 0, format, precision);
}

abstract_axis_painter::abstract_axis_painter(render_area_widget *area): abstract_painter(area)
{
  m_axis_painter_x = std::make_unique<axis_painter_x> (this);
  m_axis_painter_y = std::make_unique<axis_painter_y> (this);
}

void abstract_axis_painter::set_model (abstract_axis_model *model)
{
  m_model.reset (model);
}

QRectF abstract_axis_painter::get_initial_rect () const
{
    return get_render_area ()->rect ();
}

QPointF abstract_axis_painter::get_screen_pos (double x, double y) const
{
  double screen_x, screen_y;

  QRectF draw_rect = get_draw_rect ();

  double min_x = m_model->data (axis_settings::min_x).value<double> ();
  double min_y = m_model->data (axis_settings::min_y).value<double> ();
  double max_x = m_model->data (axis_settings::max_x).value<double> ();
  double max_y = m_model->data (axis_settings::max_y).value<double> ();

  double t_x = 0.5;
  double t_y = 0.5;

  double diff_x = max_x - min_x;
  if (!fuzzy_eq (diff_x, 0.))
    t_x = (x - min_x) / diff_x;

  double diff_y = max_y - min_y;
  if (!fuzzy_eq (diff_y, 0.))
    t_y = (y - min_y) / diff_y;

  screen_x = t_x * (draw_rect.right () - draw_rect.left ()) + draw_rect.left ();
  screen_y = t_y * (draw_rect.top ()   - draw_rect.bottom ()) + draw_rect.bottom ();

  return {screen_x, screen_y};
}

void abstract_axis_painter::get_local_pos (QPointF screen_pos, double &x, double &y) const
{
    QRectF draw_rect = get_draw_rect ();

    double min_x = m_model->data (axis_settings::min_x).value<double> ();
    double min_y = m_model->data (axis_settings::min_y).value<double> ();
    double max_x = m_model->data (axis_settings::max_x).value<double> ();
    double max_y = m_model->data (axis_settings::max_y).value<double> ();

    double t_x = 0.5;
    double t_y = 0.5;

    double height = draw_rect.height ();
    double width = draw_rect.width ();

    if (!fuzzy_eq (width, 0.))
      t_x = (screen_pos.x () - draw_rect.left ()) / width;

    if (!fuzzy_eq (height, 0.))
      t_y = (draw_rect.bottom () - screen_pos.y ()) / height;

    x = t_x * (max_x - min_x) + min_x;
    y = t_y * (max_y - min_y) + min_y;
}

void abstract_axis_painter::update_rect (QPainter &painter)
{
  int top_margin    = default_margin_vertical,
      bottom_margin = default_margin_vertical;
  int left_margin   = default_margin_horizontal,
      right_margin  = default_margin_horizontal;

  QFontMetrics fm = painter.fontMetrics ();

  {
    //axis name
    bottom_margin += fm.height ();
    bottom_margin += m_axis_painter_x->text_top_spacing ();
    bottom_margin += m_axis_painter_x->text_bottom_spacing ();

    //values
    bottom_margin += m_axis_painter_x->value_left_spacing ();
    bottom_margin += m_axis_painter_x->value_right_spacing ();

    //segments
    bottom_margin += m_axis_painter_x->min_segment_one_side_size ();

  }

  {
    int precision = m_model->data (axis_settings::precision).value <int> ();

    double min_y = m_model->data (axis_settings::min_y).value<double> ();
    double max_y = m_model->data (axis_settings::max_y).value<double> ();
    double diff_y = max_y - min_y;
    double step = diff_y / (default_segments - 1);

    int max_width = 0;
    for (int segment_y = 0; segment_y < default_segments; segment_y++)
    {
      double segment_value = min_y + step * segment_y;
      QString str_value = value_to_string (segment_value, 'g', precision);
      int width = fm.horizontalAdvance (str_value);
      if (width > max_width)
        max_width = width;
    }

    QString name = m_model->data (axis_settings::name_y).value <QString> ();

    //axis name
    left_margin += fm.horizontalAdvance (name);
    left_margin += m_axis_painter_x->text_left_spacing ();
    left_margin += m_axis_painter_x->text_right_spacing ();

    //values
    left_margin += m_axis_painter_x->text_left_spacing ();
    left_margin += max_width;
    left_margin += m_axis_painter_x->text_right_spacing ();

    //segments
    left_margin += m_axis_painter_x->min_segment_one_side_size ();

    m_axis_painter_y->set_max_width (max_width);
  }



  QRectF initial_rect = get_initial_rect ();
  m_draw_rect = QRectF (initial_rect.left () + left_margin, initial_rect.top () + top_margin,
                       initial_rect.width () - left_margin - right_margin,
                       initial_rect.height () - top_margin - bottom_margin);
}

void abstract_axis_painter::configure_axis (QPainter &painter)
{
  update_rect (painter);
}

void abstract_axis_painter::set_clipping (QPainter &painter) const
{
    QRectF rect_for_draw = get_draw_rect ();
    painter.setClipRect (rect_for_draw);
}

void abstract_axis_painter::draw_background (QPainter &painter) const
{
#ifdef AXIS_DEBUG_MODE
  {
    QColor color = QColor ("gray");
    painter.fillRect (get_initial_rect (), color);
  }
#endif
  QColor color = m_model->data (axis_settings::background_color).value<QColor> ();
  painter.fillRect (get_draw_rect (), color);

}

void abstract_axis_painter::draw_axis (QPainter &painter) const
{
  m_axis_painter_x->draw (painter);
  m_axis_painter_y->draw (painter);
}


void abstract_axis_painter::draw (QPainter &painter)
{
  configure_axis (painter);

  draw_background (painter);
  draw_axis (painter);
  set_clipping (painter);
}

//-----------------------------------------------------------------------------

axis_painter_x::axis_painter_x (abstract_axis_painter *axis_area_painter): axis_painter (axis_area_painter)
{

}

void axis_painter_x::draw (QPainter &painter)
{
  const abstract_axis_model *model = m_axis_painter->get_model ();
  QRectF draw_rect = m_axis_painter->get_draw_rect ();

  QFontMetrics fm = painter.fontMetrics ();

  double y = draw_rect.bottom () + min_segment_one_side_size () +
          value_top_spacing () + 3 * fm.height () / 2 +
          value_bottom_spacing () + text_top_spacing ();

  // 1. draw name
  QString axis_name = model->data (axis_settings::name_x).value<QString> ();

  QPointF center = draw_rect.center ();
  painter.drawText (center.x (), y, axis_name);

  y -= fm.height () + value_bottom_spacing () + text_top_spacing ();
  // 2. draw values

  double width = draw_rect.width ();
  double left = draw_rect.left ();
  double top = draw_rect.top ();
  double bottom = draw_rect.bottom ();

  double width_of_segment = width / (abstract_axis_painter::default_segments - 1);

  double min_x = model->data (axis_settings::min_x).value<double> ();
  double max_x = model->data (axis_settings::max_x).value<double> ();
  double diff_x = max_x - min_x;
  double step = diff_x / (abstract_axis_painter::default_segments - 1);

  int precision = model->data (axis_settings::precision).value <int> ();

  for (int segment_x = 0; segment_x < abstract_axis_painter::default_segments; segment_x++)
  {
      double segment_value = min_x + step * segment_x;
      QString str_value = value_to_string (segment_value, 'g', precision);
      int str_width = fm.horizontalAdvance (str_value);

      double x = left + segment_x * width_of_segment - 0.5 * str_width;
      painter.drawText (static_cast<int> (x), y, str_value);
  }

  // 3. draw line
  {
    QPen pen;
    pen.setWidth (2);
    painter.setPen (pen);
    painter.drawLine (draw_rect.bottomLeft (), draw_rect.bottomRight ());
  }

  // 4. draw segment
  {
    QPen pen;
    pen.setWidth (1);
    pen.setStyle (Qt::PenStyle::DotLine);
    painter.setPen (pen);

    for (int segment_x = 0; segment_x < abstract_axis_painter::default_segments; segment_x++)
    {
      double x = left + segment_x * width_of_segment;
      painter.drawLine (QPointF (x, top), QPointF (x, bottom));
    }
  }

  // 5. draw min segments
  {
      QPen pen;
      pen.setWidth (1);
      pen.setStyle (Qt::PenStyle::SolidLine);
      painter.setPen (pen);
      for (int segment_x = 0; segment_x < abstract_axis_painter::default_segments; segment_x++)
      {
          double x = left + segment_x * width_of_segment;
          painter.drawLine (QPointF (x, bottom + min_segment_one_side_size ()), QPointF (x, bottom - min_segment_one_side_size ()));
      }
  }
}

axis_painter_y::axis_painter_y (abstract_axis_painter *axis_area_painter): axis_painter (axis_area_painter)
{

}

void axis_painter_y::draw (QPainter &painter)
{
  const abstract_axis_model *model = m_axis_painter->get_model ();
  QRectF initial_rect = m_axis_painter->get_initial_rect ();
  QRectF draw_rect = m_axis_painter->get_draw_rect ();

  QFontMetrics fm = painter.fontMetrics ();

  // 1. draw name
  QString axis_name = model->data (axis_settings::name_y).value<QString> ();

  double x = initial_rect.left () + abstract_axis_painter::default_margin_vertical + fm.horizontalAdvance (axis_name) / 2 + text_left_spacing ();

  QPointF center = draw_rect.center ();
  painter.drawText (x, center.y (), axis_name);

  // 2. draw values

  double height = draw_rect.height ();
  double left = draw_rect.left ();
  double right = draw_rect.right ();
  double bottom = draw_rect.bottom ();

  x += fm.horizontalAdvance (axis_name) / 2 + text_right_spacing () + value_left_spacing ();

  double height_of_segment = height / (abstract_axis_painter::default_segments - 1);

  double min_y = model->data (axis_settings::min_y).value<double> ();
  double max_y = model->data (axis_settings::max_y).value<double> ();
  double diff_y = max_y - min_y;
  double step = diff_y / (abstract_axis_painter::default_segments - 1);

  int precision = model->data (axis_settings::precision).value <int> ();

  for (int segment_y = 0; segment_y < abstract_axis_painter::default_segments; segment_y++)
  {
      double segment_value = min_y + step * segment_y;
      QString str_value = value_to_string (segment_value, 'g', precision);

      double y = bottom - segment_y * height_of_segment + 0.5 * fm.height ();
      painter.drawText (x, static_cast<int> (y), str_value);
  }

  // 3. draw line
  {
    QPen pen;
    pen.setWidth (2);
    painter.setPen (pen);
    painter.drawLine (draw_rect.bottomLeft (), draw_rect.topLeft ());
  }

  // 4. draw segment
  {
    QPen pen;
    pen.setWidth (1);
    pen.setStyle (Qt::PenStyle::DotLine);
    painter.setPen (pen);

    for (int segment_y = 0; segment_y < abstract_axis_painter::default_segments; segment_y++)
    {
      double y = bottom - segment_y * height_of_segment;
      painter.drawLine (QPointF (left, y), QPointF (right, y));
    }
  }

  // 5. draw min segments
  {
    QPen pen;
    pen.setWidth (1);
    pen.setStyle (Qt::PenStyle::SolidLine);
    painter.setPen (pen);
    for (int segment_y = 0; segment_y < abstract_axis_painter::default_segments; segment_y++)
    {
      double y = bottom - segment_y * height_of_segment;
      painter.drawLine (QPointF (left - min_segment_one_side_size (), y), QPointF (left, y));
    }
  }
}
