#include "main_window.h"

#include "graph_editor_tab.h"
#include <QTabWidget>
#include <QVBoxLayout>

gno_main_window::gno_main_window(QWidget *parent): QWidget (parent)
{
    QVBoxLayout *layout = new QVBoxLayout (this);
    setLayout (layout);

    QTabWidget *tab_widget = new QTabWidget (this);
    tab_widget->addTab(m_editor_tab = new graph_editor_tab (this), "Graph Editor");
    tab_widget->addTab(m_calculations_tab = new graph_editor_tab (this), "Algorithms");

    layout->addWidget (tab_widget);
}
