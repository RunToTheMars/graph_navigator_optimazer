#include "render_area_widget.h"
#include <QPainter>
#include <QMouseEvent>

render_area_widget::render_area_widget (QWidget *parent) : QWidget (parent)
{
}

QSize render_area_widget::minimumSizeHint() const
{
  return QSize (200, 100);
}

QSize render_area_widget::sizeHint() const
{
  return QSize (400, 200);
}

void render_area_widget::paintEvent (QPaintEvent */*event*/)
{
  QPainter painter (this);

  painter.setRenderHint(QPainter::Antialiasing);
  painter.setRenderHint(QPainter::HighQualityAntialiasing);

  draw (painter);
}
