#ifndef VEH_ON_GRAPH_PAINTER_H
#define VEH_ON_GRAPH_PAINTER_H

#include "graph_painter.h"

class veh_on_graph_painter : public graph_painter
{
    using parent_t = graph_painter;

    graph::uid m_src_node = graph::invalid_uid;
    graph::uid m_dst_node = graph::invalid_uid;

    double m_cur_time = 0.;
    bool m_show_d_distance = false;
    bool m_show_src_dst = false;
    std::vector<graph::vehicle_continuous_line_states> *m_line_states = nullptr;

public:
    veh_on_graph_painter (graph::graph_initial *graph_initial, render_area_widget *area);

    virtual void draw (QPainter &painter) override;
    void draw_veh (QPainter &painter);
    void draw_start_end (QPainter &painter);

    void set_line_states (std::vector<graph::vehicle_continuous_line_states> *line_states)
    {
      m_line_states = line_states;
    }

    void set_src (graph::uid src_node) { m_src_node = src_node; }
    void set_dst (graph::uid dst_node) { m_dst_node = dst_node; }
    void show_src_dst (bool show) { m_show_src_dst = show; }

    void set_time (double t) { m_cur_time = t; }

    void show_d_distance (bool show) { m_show_d_distance = show; }

private:
    void draw_start_end (QPainter &painter, graph::uid node_uid, QString text);
};

#endif // VEH_ON_GRAPH_PAINTER_H
