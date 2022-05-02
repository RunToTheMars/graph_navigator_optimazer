#include "color_builder.h"

#include <QColor>

QColor default_color_builder::get_color (double val) const
{
  double v = 3 * (val - m_min_val) * m_range_val_reverse;

  if (v > 3)
    v = 3;
  if (v < 0)
    v = 0;

  return QColor (v <= 1 ? (1. - v) * 255 : 0., v <= 2 && v > 1 ? (2. - v) * 255 : 0. , v <= 3 && v > 2 ? (3. - v) * 255 : 0.);
}
