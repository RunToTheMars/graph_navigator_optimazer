#ifndef ABSTRACT_PAINTER_HOLDER_H
#define ABSTRACT_PAINTER_HOLDER_H

#include <QWidget>
#include <vector>

class QRect;
class QPainter;

class render_area_widget;

class abstract_painter_holder: public QWidget
{
  Q_OBJECT

  render_area_widget *m_render_area_widget = nullptr;

public:
  abstract_painter_holder (QWidget *parent);
  virtual ~abstract_painter_holder () = default;
};

#endif // ABSTRACT_PAINTER_HOLDER_H
