#ifndef PATH_GRAPH_WIDGET_H
#define PATH_GRAPH_WIDGET_H

#include <QWidget>
#include "gno_graph_fwd.h"

class model_graph_area_render;
class veh_on_graph_painter;

class QLineEdit;

class path_graph_widget: public QWidget
{
    Q_OBJECT

    graph::gno_path_finder_base *m_path_finder = nullptr;
    graph::gno_continuous_modeling *m_continuous_modeling = nullptr;

    QLineEdit *m_res_line = nullptr;

    model_graph_area_render *area_render = nullptr;

    graph::uid m_src;
    graph::uid m_dst;

    double m_start_time = 0.;

    std::vector<graph::uid> m_best_path;
    std::vector<graph::vehicle_continuous_line_states> m_line_states;

public:
    path_graph_widget (graph::graph_initial *graph_initial, graph::gno_path_finder_base *path_finder, graph::gno_continuous_modeling *continuous_modeling, QWidget *parent = nullptr);
    ~path_graph_widget ();

    void set_src_dst (graph::uid src, graph::uid dst);
    void set_start_time (double start_time) { m_start_time = start_time; }

    const std::vector<graph::uid> &get_path () const { return m_best_path; }

    const std::vector<graph::vehicle_continuous_line_states> &get_line_states () const { return m_line_states; }

    veh_on_graph_painter *get_painter ();

    Q_SIGNAL void update_times_needed ();
};

#endif // PATH_GRAPH_WIDGET_H
