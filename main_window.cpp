#include "main_window.h"

#include "tab_graph_editor.h"
#include "tab_graph_modeling.h"

#include <QTabWidget>
#include <QVBoxLayout>

#include "gno_graph.h"
#include "gno_graph_initial_state.h"
#include "gno_graph_initial.h"

gno_main_window::gno_main_window(QWidget *parent): QWidget (parent)
{
    m_graph = std::make_unique<graph::graph_impl> ();
    m_graph_initial_state = std::make_unique<graph::graph_initial_state_impl> ();
    m_graph_initial = std::make_unique<graph::graph_initial> (m_graph.get (), m_graph_initial_state.get ());

    QVBoxLayout *layout = new QVBoxLayout (this);
    setLayout (layout);

    QTabWidget *tab_widget = new QTabWidget (this);

    tab_widget->addTab(m_editor_tab = new graph_editor_tab (m_graph_initial.get (), this), "Graph Editor");
    tab_widget->addTab(m_modeling_tab = new graph_modeling_tab (m_graph_initial.get (), this), "Modeling");

    layout->addWidget (tab_widget);
}

gno_main_window::~gno_main_window () = default;
