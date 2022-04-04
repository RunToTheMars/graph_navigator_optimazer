#include "abstract_painter_holder.h"

#include "render_area_widget.h"

#include <QWidget>
#include <QRect>
#include <QPainter>

abstract_painter_holder::abstract_painter_holder (QWidget *parent)
    : QWidget (parent)
{
}
