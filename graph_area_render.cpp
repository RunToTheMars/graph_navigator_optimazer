#include "graph_area_render.h"

#include <graph_painter.h>

graph_area_render::graph_area_render (QWidget *parent)
    : render_area_widget (parent)
{
  m_graph_painter = std::make_unique<graph_painter> (this);
}

void graph_area_render::draw (QPainter &painter)
{
  m_graph_painter->draw (painter);
}


