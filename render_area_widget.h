#ifndef RENDER_AREA_WIDGET_H
#define RENDER_AREA_WIDGET_H

#include <QWidget>

class render_area_widget : public QWidget
{
    Q_OBJECT

public:
    render_area_widget (QWidget *parent = 0);

    QSize minimumSizeHint() const Q_DECL_OVERRIDE;
    QSize sizeHint() const Q_DECL_OVERRIDE;

    void paintEvent (QPaintEvent *event) Q_DECL_OVERRIDE;

protected:
    virtual void draw (QPainter &painter) = 0;
};

#endif // RENDER_AREA_WIDGET_H
