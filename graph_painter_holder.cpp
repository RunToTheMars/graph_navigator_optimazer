#include "graph_painter_holder.h"

graph_painter_holder::graph_painter_holder(QWidget *canvas_widget): abstract_painter_holder (canvas_widget)
{

}

void graph_painter_holder::draw (QPainter & /*painter*/) { }
