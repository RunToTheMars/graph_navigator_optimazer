#ifndef MULTIPATH_GRAPH_WIDGET_H
#define MULTIPATH_GRAPH_WIDGET_H

#include <QWidget>
#include "gno_graph_fwd.h"

class model_graph_area_render;
class veh_on_graph_painter;

class QLineEdit;

class multipath_graph_widget: public QWidget
{
    Q_OBJECT

    graph::gno_multipath_finder_base *m_multipath_finder = nullptr;
    graph::gno_continuous_modeling *m_continuous_modeling = nullptr;

    QLineEdit *m_res_line = nullptr;

    model_graph_area_render *area_render = nullptr;

    std::vector<std::vector<graph::uid>> m_best_multipath;
    std::vector<graph::vehicle_continuous_line_states> m_line_states;

    std::unique_ptr<graph::graph_initial_state_base> m_new_graph_initial_state;
    std::unique_ptr<graph::graph_initial> m_new_graph_initial;

public:
    multipath_graph_widget (graph::graph_initial *graph_initial, graph::gno_multipath_finder_base *multipath_finder,
                            graph::gno_continuous_modeling *continuous_modeling, QWidget *parent = nullptr);
    ~multipath_graph_widget ();

    const std::vector<graph::vehicle_continuous_line_states> &get_line_states () const { return m_line_states; }

    veh_on_graph_painter *get_painter ();

    Q_SIGNAL void update_times_needed ();
};

#endif // MULTIPATH_GRAPH_WIDGET_H
