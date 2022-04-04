#ifndef GRAPH_EDITOR_TAB_H
#define GRAPH_EDITOR_TAB_H

#include <QWidget>
#include <memory>
#include "gno_graph.h"
#include "gno_graph_initial_state.h"
#include "gno_graph_initial.h"


class gno_editor_settings_widget;
class editable_graph_area_render;

class graph_editor_tab: public QWidget
{
public:
    graph_editor_tab (QWidget *parent);

private:
    gno_editor_settings_widget *m_editor_settings = nullptr;
    editable_graph_area_render *m_editable_graph_area_render = nullptr;

    std::unique_ptr<graph::graph_impl<>> m_graph;
    std::unique_ptr<graph::graph_initial_state_impl<>> m_graph_initial_state;

    std::unique_ptr<graph::graph_initial<>> m_graph_initial;
};

#endif // GRAPH_EDITOR_TAB_H
