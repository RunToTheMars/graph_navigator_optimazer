#include "gno_painter_widget.h"
#include <QPainter>


gno_painter_widget::gno_painter_widget(QWidget *parent): QWidget(parent)
{

}

void gno_painter_widget::paintEvent (QPaintEvent */*event*/)
{
    QPainter painter (this);
    painter.setPen(QPen(Qt::black, 12, Qt::DashDotLine, Qt::RoundCap));
    painter.drawLine(0, 0, 200, 200);
}
