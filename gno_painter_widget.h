#ifndef GNO_PAINTER_WIDGET_H
#define GNO_PAINTER_WIDGET_H

#include <QWidget>

class gno_painter_widget : public QWidget
{
public:
  gno_painter_widget (QWidget *parent = nullptr);

protected:
  void paintEvent (QPaintEvent *event) override;
};

#endif // GNO_PAINTER_WIDGET_H
