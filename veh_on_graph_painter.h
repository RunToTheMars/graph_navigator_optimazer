#ifndef VEH_ON_GRAPH_PAINTER_H
#define VEH_ON_GRAPH_PAINTER_H

#include "graph_painter.h"

class veh_on_graph_painter : public graph_painter
{
    using parent_t = graph_painter;

    double m_cur_time = 0.;
    std::vector<graph::vehicle_continuous_line_states> *m_line_states = nullptr;

public:
    veh_on_graph_painter (graph::graph_initial *graph_initial, render_area_widget *area);

    virtual void draw (QPainter &painter) override;
    void draw_veh (QPainter &painter);

    void set_line_states (std::vector<graph::vehicle_continuous_line_states> *line_states)
    {
      m_line_states = line_states;
    }

    void set_time (double t) { m_cur_time = t; }
};

#endif // VEH_ON_GRAPH_PAINTER_H
