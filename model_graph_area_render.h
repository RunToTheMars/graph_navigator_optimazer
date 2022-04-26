#ifndef MODEL_GRAPH_AREA_RENDER_H
#define MODEL_GRAPH_AREA_RENDER_H

#include "render_area_widget.h"
#include <memory>
#include "gno_graph_fwd.h"

class veh_on_graph_painter;

class model_graph_area_render final: public render_area_widget
{
    std::unique_ptr<veh_on_graph_painter> m_veh_painter;

public:
    model_graph_area_render (graph::graph_initial *graph_initial, QWidget *parent = 0);
    ~model_graph_area_render ();

    veh_on_graph_painter *get_painter () { return m_veh_painter.get (); };

protected:
    void draw (QPainter &painter) override;
};

#endif // MODEL_GRAPH_AREA_RENDER_H
