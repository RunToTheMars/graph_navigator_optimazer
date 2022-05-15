#ifndef TAB_GRAPH_MULTIPATH_H
#define TAB_GRAPH_MULTIPATH_H

#include "QWidget"
#include <memory>
#include "gno_graph_fwd.h"
#include <thread>
#include <mutex>

class QSlider;
class QSpinBox;
class QLineEdit;
class QPushButton;
class model_graph_widget;
class multipath_graph_widget;

class graph_multipath_tab: public QWidget
{
    Q_OBJECT

    static constexpr int max_slider_count = 100;

    graph::graph_initial *m_graph_initial = nullptr;
    double m_max_time = 0.;

public:
    graph_multipath_tab (graph::graph_initial *graph_initial, QWidget *parent);
    ~graph_multipath_tab ();

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

    QPushButton *m_run_button = nullptr;
    QPushButton *m_stop_button = nullptr;

    QLineEdit *m_time_line = nullptr;
    QSpinBox *m_sleep_spinbox = nullptr;
    QSlider *m_time_slider = nullptr;

    std::unique_ptr<graph::gno_multipath_finder_base> m_multipath_finder_nothing;
    multipath_graph_widget *m_multipath_nothing_widget;

    std::unique_ptr<graph::gno_multipath_finder_base> m_multipath_finder_loop;
    multipath_graph_widget *m_multipath_loop_widget;

    std::unique_ptr<graph::gno_path_finder_base> m_path_finder;

    std::unique_ptr<graph::gno_discrete_modeling_base> m_model;
    std::unique_ptr<graph::gno_continuous_modeling> m_continuous_modeling;

    std::atomic_bool m_stop;
    std::thread m_run_thread;
};

#endif // TAB_GRAPH_MULTIPATH_H
