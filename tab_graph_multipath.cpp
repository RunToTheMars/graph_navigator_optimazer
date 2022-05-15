#include "tab_graph_multipath.h"

#include "veh_on_graph_painter.h"
#include "model_graph_widget.h"
#include "multipath_graph_widget.h"
#include "gno_modeling_simple_on_edge.h"
#include "gno_modeling_simple_macro.h"
#include "gno_modeling_simple_micro.h"
#include "gno_multipath_finder_nothing.h"
#include "gno_multipath_finder_loop.h"
#include "gno_path_finder_brute_force.h"
#include <chrono>

#include "QVBoxLayout"
#include "QGridLayout"
#include "QSlider"
#include "QLineEdit"
#include "QGroupBox"
#include "QSplitter"
#include "QPushButton"
#include "QSpinBox"
#include "QLabel"

graph_multipath_tab::graph_multipath_tab (graph::graph_initial *graph_initial, QWidget *parent): QWidget(parent), m_graph_initial (graph_initial)
{
    QVBoxLayout *vertical_layout = new QVBoxLayout (this);

    // times
    QGroupBox *time_groupbox = new QGroupBox ("Time", this);
    {
        QHBoxLayout *time_horizontal_layout = new QHBoxLayout (time_groupbox);
        time_groupbox->setLayout (time_horizontal_layout);

        time_horizontal_layout->setAlignment (Qt::AlignTop);
        time_horizontal_layout->addWidget (m_run_button = new QPushButton ("Run", this));
        time_horizontal_layout->addWidget (m_stop_button = new QPushButton ("Stop", this));

        m_stop_button->setVisible (false);

        time_horizontal_layout->addWidget (new QLabel ("Sleep for (ms): ", this));
        time_horizontal_layout->addWidget (m_sleep_spinbox = new QSpinBox (this));
        m_sleep_spinbox->setSizePolicy (QSizePolicy::Minimum, QSizePolicy::Minimum);
        m_sleep_spinbox->setRange (2, 1000);


        time_horizontal_layout->addWidget (new QLabel ("Time: ", this));
        time_horizontal_layout->addWidget (m_time_line = new QLineEdit (this));
        m_time_line->setDisabled (true);
        m_time_line->setSizePolicy (QSizePolicy::Minimum, QSizePolicy::Minimum);
        time_horizontal_layout->addWidget (m_time_slider = new QSlider (Qt::Orientation::Horizontal, this));

        m_time_slider->setRange (0, max_slider_count);
        m_time_slider->setValue (0);
    }

    // solver
//    m_model = std::make_unique<graph::gno_modeling_simple_macro> ();
//    m_model = std::make_unique<graph::gno_modeling_simple_micro> (graph::V_MAX, graph::V_MIN, graph::A_MAX, graph::D, graph::L);;
    m_model = std::make_unique<graph::gno_modeling_simple_on_edge> ();
    m_continuous_modeling = std::make_unique<graph::gno_continuous_modeling> (m_model.get ());
    m_path_finder = std::make_unique<graph::gno_path_finder_brute_force> (m_model.get (), true);

    QGroupBox *models_groupbox = new QGroupBox ("Multipaths", this);
    {
        QGridLayout *models_layout = new QGridLayout (models_groupbox);
        models_groupbox->setLayout (models_layout);

        {
            QGroupBox *modeling_groupbox = new QGroupBox ("Nothing", this);
            QVBoxLayout *modeling_layout = new QVBoxLayout (modeling_groupbox);
            modeling_groupbox->setLayout (modeling_layout);

            m_multipath_finder_nothing = std::make_unique<graph::gno_multipath_finder_nothing> ();
            modeling_layout->addWidget (m_multipath_nothing_widget = new multipath_graph_widget (graph_initial, m_multipath_finder_nothing.get (), m_continuous_modeling.get (), modeling_groupbox));
            models_layout->addWidget (modeling_groupbox, 0, 0);
        }

        {
            QGroupBox *modeling_groupbox = new QGroupBox ("Optimal", this);
            QVBoxLayout *modeling_layout = new QVBoxLayout (modeling_groupbox);
            modeling_groupbox->setLayout (modeling_layout);

            m_multipath_finder_loop = std::make_unique<graph::gno_multipath_finder_loop> (m_path_finder.get (), m_continuous_modeling.get ());
            modeling_layout->addWidget (m_multipath_loop_widget = new multipath_graph_widget (graph_initial, m_multipath_finder_loop.get (), m_continuous_modeling.get (), modeling_groupbox));
            models_layout->addWidget (modeling_groupbox, 0, 1);
        }
    }


    QSplitter *splitter = new QSplitter (Qt::Orientation::Vertical, this);
    splitter->addWidget (time_groupbox);
    splitter->addWidget (models_groupbox);
    vertical_layout->addWidget (splitter);

    QObject::connect (m_run_button, &QPushButton::clicked, this, [this] { run (); });
    QObject::connect (m_stop_button, &QPushButton::clicked, this, [this] { stop (); });

    QObject::connect (m_time_slider, &QSlider::valueChanged, this, [this] { set_time_from_slider (); });

    QObject::connect (m_multipath_nothing_widget, &multipath_graph_widget::update_times_needed, this, [this] { update_max_times (); });
    QObject::connect (m_multipath_loop_widget, &multipath_graph_widget::update_times_needed, this, [this] { update_max_times (); });

    QObject::connect (this, &graph_multipath_tab::set_val, this, [this] (int i) { m_time_slider->setValue (i); });
    QObject::connect (this, &graph_multipath_tab::done, this, [this] { stop (); });
}

graph_multipath_tab::~graph_multipath_tab () = default;

void graph_multipath_tab::clear ()
{
}

void graph_multipath_tab::update_max_times ()
{
    m_max_time = 0.;

    std::vector<double> max_times;

    if (!m_multipath_nothing_widget->get_line_states ().empty ())
        max_times.push_back (m_multipath_nothing_widget->get_line_states ().back ().t2);

    if (!m_multipath_loop_widget->get_line_states ().empty ())
        max_times.push_back (m_multipath_loop_widget->get_line_states ().back ().t2);

    if (max_times.empty ())
        return;

    m_max_time =  *std::max_element (max_times.begin (), max_times.end ());
}

void graph_multipath_tab::set_time_from_slider ()
{
    double t = m_time_slider->value () * m_max_time / max_slider_count;
    set_time (t);
}


void graph_multipath_tab::set_time (double t)
{
    m_multipath_loop_widget->get_painter ()->set_time (t);
    m_multipath_loop_widget->update ();

    m_multipath_nothing_widget->get_painter()->set_time (t);
    m_multipath_nothing_widget->update ();

    m_time_line->setText (QString(" %1").arg(t, 0, 'g'));
}

void graph_multipath_tab::enable_disable (bool is_stop)
{
    m_run_button->setVisible (is_stop);
    m_stop_button->setVisible (!is_stop);
    m_sleep_spinbox->setEnabled (is_stop);
    m_time_slider->setEnabled (is_stop);

    m_stop = is_stop;
}

void graph_multipath_tab::stop ()
{
    enable_disable (true);
}


void graph_multipath_tab::run ()
{
    enable_disable (false);

    m_run_thread = std::thread ([this] {
        for (int i = 0; i <= max_slider_count; i++)
        {
            {
                if (m_stop)
                    return;
                std::this_thread::sleep_for (std::chrono::milliseconds (m_sleep_spinbox->value ()));

                Q_EMIT set_val (i);
            }
        }

        Q_EMIT done ();
    });

    m_run_thread.detach ();
}
