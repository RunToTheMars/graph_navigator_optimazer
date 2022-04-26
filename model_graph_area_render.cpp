#include "model_graph_area_render.h"

#include "veh_on_graph_painter.h"
#include "gno_graph_initial.h"
#include "gno_graph.h"

model_graph_area_render::model_graph_area_render (graph::graph_initial *graph_initial, QWidget *parent)
    : render_area_widget (parent)
{
    m_veh_painter = std::make_unique<veh_on_graph_painter> (graph_initial->get_graph (), this);
    m_veh_painter->set_is_editable (false);
    m_veh_painter->set_point_size_mult (0.5);
    m_veh_painter->set_edge_width_mult (0.5);
    setMouseTracking (true);
}

model_graph_area_render::~model_graph_area_render () = default;

void model_graph_area_render::draw (QPainter &painter)
{
  m_veh_painter->draw (painter);
}


