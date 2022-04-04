#ifndef GRAPH_PAINTER_HOLDER_H
#define GRAPH_PAINTER_HOLDER_H

#include "abstract_painter_holder.h"
#include <memory>

class graph_painter_holder final: public abstract_painter_holder
{


public:
  graph_painter_holder (QWidget *canvas_widget);

  void draw (QPainter &painter) override;
};

#endif // GRAPH_PAINTER_HOLDER_H
