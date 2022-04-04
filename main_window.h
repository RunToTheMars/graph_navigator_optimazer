#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QWidget>

class graph_editor_tab;


class QGroupBox;

class gno_main_window: public QWidget
{
public:
    gno_main_window(QWidget *parent = nullptr);

private:
    graph_editor_tab *m_editor_tab = nullptr;
    graph_editor_tab *m_calculations_tab = nullptr;
};

#endif // MAIN_WINDOW_H
