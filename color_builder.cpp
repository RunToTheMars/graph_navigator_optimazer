#include "color_builder.h"

#include <QColor>

QColor default_color_builder::get_color(double val) const
{
  double v = 255 * (val - m_min_val) * m_range_val_reverse;

  if (v > 255)
    v = 255;
  if (v < 0)
    v = 0;

  return QColor (v, v, v);
}
