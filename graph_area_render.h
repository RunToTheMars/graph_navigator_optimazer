#ifndef GRAPH_AREA_RENDER_H
#define GRAPH_AREA_RENDER_H

#include "render_area_widget.h"
#include <memory>

class graph_painter;

class graph_area_render final: public render_area_widget
{
    std::unique_ptr<graph_painter> m_graph_painter;

public:
    graph_area_render (QWidget *parent = 0);

protected:
   void draw (QPainter &painter) override;
};

#endif // GRAPH_AREA_RENDER_H
