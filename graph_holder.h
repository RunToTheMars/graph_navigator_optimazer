#ifndef GRAPH_HOLDER_H
#define GRAPH_HOLDER_H

#include "abstract_painter_holder.h"

class graph_area_render;

class graph_holder: public abstract_painter_holder
{
  graph_area_render *m_area_render = nullptr;

public:
    graph_holder (QWidget *parent = nullptr);
};

#endif // GRAPH_HOLDER_H
