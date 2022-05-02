#ifndef TAB_GRAPH_PATH_H
#define TAB_GRAPH_PATH_H

#include <QWidget>
#include <memory>
#include "gno_graph_fwd.h"
#include <thread>

class path_graph_widget;

class QSpinBox;
class QPushButton;
class QLineEdit;
class QSlider;
class QCheckBox;

class graph_path_tab: public QWidget
{
    Q_OBJECT

    static constexpr int max_slider_count = 100;

    graph::graph_initial *m_graph_initial = nullptr;

public:
    graph_path_tab (graph::graph_initial *graph_initial, QWidget *parent);

    void update_spinboxes ();
    void clear ();

    Q_SIGNAL void set_val (int i);
    Q_SIGNAL void done ();

private:
    void run ();
    void stop ();

    void enable_disable (bool is_start);

    void set_time (double t);
    void set_time_from_slider ();
    void update_max_times ();


private:
    void set_start_end ();

    QSpinBox *m_start_spinbox = nullptr;
    QSpinBox *m_end_spinbox = nullptr;

    QLineEdit *m_start_time_editline = nullptr;

    QPushButton *m_run_button = nullptr;
    QPushButton *m_stop_button = nullptr;

    QLineEdit *m_time_line = nullptr;
    QSpinBox *m_sleep_spinbox = nullptr;
    QSlider *m_time_slider = nullptr;

    QCheckBox *m_show_src_dst_checkbox = nullptr;
    QCheckBox *m_show_path_checkbox = nullptr;

    std::unique_ptr<graph::gno_discrete_modeling_base> m_model;
    std::unique_ptr<graph::gno_continuous_modeling> m_continuous_modeling;

    std::unique_ptr<graph::gno_path_finder_base> m_brute_force;
    std::unique_ptr<graph::gno_path_finder_base> m_dijkstra;

    path_graph_widget *m_brute_force_widget = nullptr;
    path_graph_widget *m_dijkstra_widget = nullptr;

    double m_max_time = 0.;

    std::atomic_bool m_stop;
    std::thread m_run_thread;
};

#endif // TAB_GRAPH_PATH_H
