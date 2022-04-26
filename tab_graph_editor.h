#ifndef TAB_GRAPH_EDITOR_H
#define TAB_GRAPH_EDITOR_H

#include <QWidget>
#include <memory>
#include "gno_graph_fwd.h"


class gno_editor_settings_widget;
class editable_graph_area_render;

class graph_editor_tab: public QWidget
{
    graph::graph_initial *m_graph_initial = nullptr;

public:
    graph_editor_tab (graph::graph_initial *graph_initial, QWidget *parent);

private:
    gno_editor_settings_widget *m_editor_settings = nullptr;
    editable_graph_area_render *m_editable_graph_area_render = nullptr;
};

#endif // TAB_GRAPH_EDITOR_H
