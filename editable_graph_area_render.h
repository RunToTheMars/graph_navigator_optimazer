#ifndef EDITABLE_GRAPH_AREA_RENDER_H
#define EDITABLE_GRAPH_AREA_RENDER_H

#include "render_area_widget.h"
#include <memory>
#include "gno_graph_fwd.h"

class graph_painter;

class editable_graph_area_render final: public render_area_widget
{
    std::unique_ptr<graph_painter> m_graph_painter;

    bool m_in_drag = false;
    bool m_enable_drag = true;

public:
    editable_graph_area_render (graph::graph_initial *graph_initial, QWidget *parent = 0);
    ~editable_graph_area_render ();

    void mouseReleaseEvent (QMouseEvent *event) Q_DECL_OVERRIDE; // событие отпускания мыши
    void mousePressEvent (QMouseEvent *event) Q_DECL_OVERRIDE; // событие щелчка мыши
    void mouseMoveEvent (QMouseEvent *event) Q_DECL_OVERRIDE; // событие движения мыши

    graph_painter *get_painter () { return m_graph_painter.get (); }

protected:
   void draw (QPainter &painter) override;
};

#endif // EDITABLE_GRAPH_AREA_RENDER_H
