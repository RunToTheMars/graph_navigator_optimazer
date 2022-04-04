#include "graph_holder.h"

#include "graph_area_render.h"
#include <QVBoxLayout>

graph_holder::graph_holder (QWidget *parent): abstract_painter_holder (parent)
{
  QVBoxLayout *mainLayout = new QVBoxLayout (this);
  mainLayout->addWidget (m_area_render = new graph_area_render (parent));
}
