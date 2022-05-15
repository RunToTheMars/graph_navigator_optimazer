#include "main_window.h"

#include "tab_graph_editor.h"
#include "tab_graph_modeling.h"
#include "tab_graph_path.h"
#include "tab_graph_multipath.h"

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
    tab_widget->addTab(m_path_tab = new graph_path_tab (m_graph_initial.get (), this), "Path");
    tab_widget->addTab(m_multipath_tab = new graph_multipath_tab (m_graph_initial.get (), this), "MultiPath");

    QObject::connect (m_editor_tab, &graph_editor_tab::graph_changed, this, [this]
                     {
                         m_modeling_tab->clear ();
                         m_path_tab->clear ();
                         m_multipath_tab->clear ();
                     });

    layout->addWidget (tab_widget);

    m_modeling_tab->clear ();
    m_path_tab->clear ();
    m_multipath_tab->clear ();
}

gno_main_window::~gno_main_window () = default;
