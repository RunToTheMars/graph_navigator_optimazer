#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QWidget>
#include "gno_graph_fwd.h"

class graph_editor_tab;
class graph_modeling_tab;
class graph_path_tab;
class graph_multipath_tab;
class QGroupBox;

class gno_main_window: public QWidget
{
    Q_OBJECT

public:
    gno_main_window (QWidget *parent = nullptr);
    ~gno_main_window ();

private:
    std::unique_ptr<graph::graph_base> m_graph;
    std::unique_ptr<graph::graph_initial_state_base> m_graph_initial_state;
    std::unique_ptr<graph::graph_initial> m_graph_initial;

    graph_editor_tab *m_editor_tab = nullptr;
    graph_modeling_tab *m_modeling_tab = nullptr;
    graph_path_tab *m_path_tab = nullptr;
    graph_multipath_tab *m_multipath_tab = nullptr;
};

#endif // MAIN_WINDOW_H
