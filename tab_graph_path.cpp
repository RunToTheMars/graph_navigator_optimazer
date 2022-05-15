#include "tab_graph_path.h"

#include "path_graph_widget.h"

#include "veh_on_graph_painter.h"

#include "gno_path_finder_brute_force.h"
#include "gno_path_finder_dijkstra.h"
#include "gno_modeling_simple_on_edge.h"
#include "gno_modeling_simple_macro.h"
#include "gno_graph_initial.h"
#include "gno_graph_initial_state.h"
#include "gno_graph.h"
#include "color_builder.h"

#include <QGroupBox>
#include <QVBoxLayout>
#include <QSplitter>
#include <QSpinBox>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QSlider>
#include <QCheckBox>

graph_path_tab::graph_path_tab (graph::graph_initial *graph_initial, QWidget *parent): QWidget (parent), m_graph_initial (graph_initial)
{
    QVBoxLayout *layout = new QVBoxLayout (this);

    // path
    QGroupBox *path_groupbox = new QGroupBox ("Path", this);
    {
        QGridLayout *path_layout = new QGridLayout (path_groupbox);
        path_groupbox->setLayout (path_layout);

        path_layout->setAlignment (Qt::AlignTop);

        path_layout->addWidget (new QLabel ("Start", path_groupbox), 0, 0);
        path_layout->addWidget (m_start_spinbox = new QSpinBox (path_groupbox), 1, 0);

        path_layout->addWidget (new QLabel ("End", path_groupbox), 0, 1);
        path_layout->addWidget (m_end_spinbox = new QSpinBox (path_groupbox), 1, 1);

        path_layout->addWidget (new QLabel ("Start Time", path_groupbox), 2, 0);
        path_layout->addWidget (m_start_time_editline = new QLineEdit (path_groupbox), 2, 1);
        m_start_time_editline->setValidator (new QDoubleValidator (0, 100, 2, m_start_time_editline) );
    }
    path_groupbox->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);

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

        time_horizontal_layout->addWidget (new QLabel ("Show Src and Dst", this));
        time_horizontal_layout->addWidget (m_show_src_dst_checkbox = new QCheckBox (this));

        time_horizontal_layout->addWidget (new QLabel ("Show Path", this));
        time_horizontal_layout->addWidget (m_show_path_checkbox = new QCheckBox (this));
    }

    // solvers
    m_model = std::make_unique<graph::gno_modeling_simple_macro> ();
//    m_model = std::make_unique<graph::gno_modeling_simple_on_edge> ();
    m_continuous_modeling = std::make_unique<graph::gno_continuous_modeling> (m_model.get ());

    QGroupBox *solvers_groupbox = new QGroupBox ("Solvers", this);
    {
        QGridLayout *solvers_layout = new QGridLayout (solvers_groupbox);
        solvers_groupbox->setLayout (solvers_layout);
        {
            QGroupBox *solver_brute_force_groupbox = new QGroupBox ("Brute Force", this);
            QVBoxLayout *solver_brute_force_layout = new QVBoxLayout (solver_brute_force_groupbox);
            solver_brute_force_groupbox->setLayout (solver_brute_force_layout);

            m_brute_force = std::make_unique<graph::gno_path_finder_brute_force> (m_model.get (), false);

            solver_brute_force_layout->addWidget (m_brute_force_widget = new path_graph_widget (graph_initial, m_brute_force.get (), m_continuous_modeling.get (), solver_brute_force_groupbox));
            m_brute_force_widget->get_painter()->show_src_dst (true);
            solvers_layout->addWidget (solver_brute_force_groupbox, 0, 0);
        }

        {
            QGroupBox *solver_dijkstra_groupbox = new QGroupBox ("Dijkstra", this);
            QVBoxLayout *solver_dijkstra_layout = new QVBoxLayout (solver_dijkstra_groupbox);
            solver_dijkstra_groupbox->setLayout (solver_dijkstra_layout);

            m_dijkstra = std::make_unique<graph::gno_path_finder_dijkstra> (m_model.get (), false);

            solver_dijkstra_layout->addWidget (m_dijkstra_widget = new path_graph_widget (graph_initial, m_dijkstra.get (), m_continuous_modeling.get (), solver_dijkstra_groupbox));
            m_dijkstra_widget->get_painter()->show_src_dst (true);
            solvers_layout->addWidget (solver_dijkstra_groupbox, 0, 1);
        }

        {
            QGroupBox *solver_brute_force_groupbox = new QGroupBox ("Brute Force (Depence)", this);
            QVBoxLayout *solver_brute_force_layout = new QVBoxLayout (solver_brute_force_groupbox);
            solver_brute_force_groupbox->setLayout (solver_brute_force_layout);

            m_brute_force_with_depence = std::make_unique<graph::gno_path_finder_brute_force> (m_model.get (), true);

            solver_brute_force_layout->addWidget (m_brute_force_with_depence_widget = new path_graph_widget (graph_initial, m_brute_force_with_depence.get (), m_continuous_modeling.get (), solver_brute_force_groupbox));
            m_brute_force_with_depence_widget->get_painter()->show_src_dst (true);
            solvers_layout->addWidget (solver_brute_force_groupbox, 1, 0);
        }

        {
            QGroupBox *solver_dijkstra_groupbox = new QGroupBox ("Dijkstra (Depence)", this);
            QVBoxLayout *solver_dijkstra_layout = new QVBoxLayout (solver_dijkstra_groupbox);
            solver_dijkstra_groupbox->setLayout (solver_dijkstra_layout);

            m_dijkstra_with_depence = std::make_unique<graph::gno_path_finder_dijkstra> (m_model.get (), true);

            solver_dijkstra_layout->addWidget (m_dijkstra_with_depence_widget = new path_graph_widget (graph_initial, m_dijkstra_with_depence.get (), m_continuous_modeling.get (), solver_dijkstra_groupbox));
            m_dijkstra_with_depence_widget->get_painter()->show_src_dst (true);
            solvers_layout->addWidget (solver_dijkstra_groupbox, 1, 1);
        }
    }

    QObject::connect (m_start_spinbox, QOverload<int>::of(&QSpinBox::valueChanged), this, [this] { set_start_end (); });
    QObject::connect (m_end_spinbox, QOverload<int>::of(&QSpinBox::valueChanged), this, [this] { set_start_end (); });

    QObject::connect (m_start_time_editline, &QLineEdit::textChanged, this, [this] (QString text)
                     {
                       m_brute_force_widget->set_start_time (text.toDouble ());
                       m_dijkstra_widget->set_start_time (text.toDouble ());
                       m_brute_force_with_depence_widget->set_start_time (text.toDouble ());
                       m_dijkstra_with_depence_widget->set_start_time (text.toDouble ());
                     });

    QObject::connect (m_run_button, &QPushButton::clicked, this, [this] { run (); });
    QObject::connect (m_stop_button, &QPushButton::clicked, this, [this] { stop (); });

    QObject::connect (m_time_slider, &QSlider::valueChanged, this, [this] { set_time_from_slider (); });

    QObject::connect (m_brute_force_widget, &path_graph_widget::update_times_needed, this, [this] { update_max_times (); });
    QObject::connect (m_dijkstra_widget, &path_graph_widget::update_times_needed, this, [this] { update_max_times (); });
    QObject::connect (m_brute_force_with_depence_widget, &path_graph_widget::update_times_needed, this, [this] { update_max_times (); });
    QObject::connect (m_dijkstra_with_depence_widget, &path_graph_widget::update_times_needed, this, [this] { update_max_times (); });

    QObject::connect (this, &graph_path_tab::set_val, this, [this] (int i) { m_time_slider->setValue (i); });
    QObject::connect (this, &graph_path_tab::done, this, [this] { stop (); });

    QObject::connect (m_show_src_dst_checkbox, &QCheckBox::clicked, this, [this] (bool checked) {
        m_brute_force_widget->get_painter()->show_src_dst (checked);
        m_dijkstra_widget->get_painter()->show_src_dst (checked);
        m_brute_force_with_depence_widget->get_painter()->show_src_dst (checked);
        m_dijkstra_with_depence_widget->get_painter()->show_src_dst (checked);
        m_brute_force_widget->get_painter()->update ();
        m_dijkstra_widget->get_painter()->update ();
        m_brute_force_with_depence_widget->get_painter()->update ();
        m_dijkstra_with_depence_widget->get_painter()->update ();
    });

    QObject::connect (m_show_path_checkbox, &QCheckBox::clicked, this, [this] (bool checked) {
        default_color_builder builder;
        graph::uid veh_count = m_graph_initial->get_initial_state ()->vehicle_count ();
        builder.set_val_range (0, veh_count);

        QColor veh_color = builder.get_color (veh_count);

        m_brute_force_widget->get_painter()->set_show_path (checked);
        m_dijkstra_widget->get_painter()->set_show_path (checked);
        m_brute_force_with_depence_widget->get_painter()->set_show_path (checked);
        m_dijkstra_with_depence_widget->get_painter()->set_show_path (checked);

        m_brute_force_widget->get_painter()->set_path (m_brute_force_widget->get_path ());
        m_dijkstra_widget->get_painter()->set_path (m_dijkstra_widget->get_path ());
        m_brute_force_with_depence_widget->get_painter()->set_path (m_brute_force_with_depence_widget->get_path ());
        m_dijkstra_with_depence_widget->get_painter()->set_path (m_dijkstra_with_depence_widget->get_path ());

        m_brute_force_widget->get_painter()->set_path_color (veh_color);
        m_dijkstra_widget->get_painter()->set_path_color (veh_color);
        m_brute_force_with_depence_widget->get_painter()->set_path_color (veh_color);
        m_dijkstra_with_depence_widget->get_painter()->set_path_color (veh_color);

        m_brute_force_widget->get_painter()->update ();
        m_dijkstra_widget->get_painter()->update ();
        m_brute_force_with_depence_widget->get_painter()->update ();
        m_dijkstra_with_depence_widget->get_painter()->update ();
    });

    QSplitter *splitter = new QSplitter (Qt::Orientation::Vertical, this);
    splitter->addWidget (path_groupbox);
    splitter->addWidget (time_groupbox);
    splitter->addWidget (solvers_groupbox);
    layout->addWidget (splitter);

    m_brute_force_widget->get_painter()->show_src_dst (false);
    m_dijkstra_widget->get_painter()->show_src_dst (false);
    m_brute_force_with_depence_widget->get_painter()->show_src_dst (false);
    m_dijkstra_with_depence_widget->get_painter()->show_src_dst (false);

    update_spinboxes ();
    set_start_end ();
}

void graph_path_tab::update_spinboxes ()
{
  m_start_spinbox->setRange (0, m_graph_initial->get_graph()->node_count());
  m_end_spinbox->setRange (0, m_graph_initial->get_graph()->node_count());
  set_start_end ();
}

void graph_path_tab::set_start_end ()
{
  m_start_spinbox->setRange (0, m_graph_initial->get_graph()->node_count());
  m_end_spinbox->setRange (0, m_graph_initial->get_graph()->node_count());

  int start = m_start_spinbox->value ();
  int end = m_end_spinbox->value ();

  m_brute_force_widget->set_src_dst (start, end);
  m_dijkstra_widget->set_src_dst (start, end);
  m_brute_force_with_depence_widget->set_src_dst (start, end);
  m_dijkstra_with_depence_widget->set_src_dst (start, end);
}

void graph_path_tab::clear ()
{
    m_brute_force_widget->get_painter()->set_line_states (nullptr);
    m_dijkstra_widget->get_painter()->set_line_states (nullptr);
    m_brute_force_with_depence_widget->get_painter()->set_line_states (nullptr);
    m_dijkstra_with_depence_widget->get_painter()->set_line_states (nullptr);

    m_brute_force_widget->get_painter()->set_path ({});
    m_dijkstra_widget->get_painter()->set_path ({});
    m_brute_force_with_depence_widget->get_painter()->set_path ({});
    m_dijkstra_with_depence_widget->get_painter()->set_path ({});

    update_spinboxes ();
}

void graph_path_tab::update_max_times ()
{
    m_max_time = 0.;

    std::vector<double> max_times;

    if (!m_brute_force_widget->get_line_states ().empty ())
        max_times.push_back (m_brute_force_widget->get_line_states ().back ().t2);

    if (!m_dijkstra_widget->get_line_states ().empty ())
        max_times.push_back (m_dijkstra_widget->get_line_states ().back ().t2);

    if (!m_brute_force_with_depence_widget->get_line_states ().empty ())
        max_times.push_back (m_brute_force_with_depence_widget->get_line_states ().back ().t2);

    if (!m_dijkstra_with_depence_widget->get_line_states ().empty ())
        max_times.push_back (m_dijkstra_with_depence_widget->get_line_states ().back ().t2);

    if (max_times.empty ())
        return;

    m_max_time =  *std::max_element (max_times.begin (), max_times.end ());


    //update paths

    m_brute_force_widget->get_painter()->set_path (m_brute_force_widget->get_path ());
    m_dijkstra_widget->get_painter()->set_path (m_dijkstra_widget->get_path ());
    m_brute_force_with_depence_widget->get_painter()->set_path (m_brute_force_with_depence_widget->get_path ());
    m_dijkstra_with_depence_widget->get_painter()->set_path (m_dijkstra_with_depence_widget->get_path ());

    m_brute_force_widget->get_painter()->update ();
    m_dijkstra_widget->get_painter()->update ();
    m_brute_force_with_depence_widget->get_painter()->update ();
    m_dijkstra_with_depence_widget->get_painter()->update ();
}

void graph_path_tab::set_time_from_slider ()
{
    double t = m_time_slider->value () * m_max_time / max_slider_count;
    set_time (t);
}


void graph_path_tab::set_time (double t)
{
    m_brute_force_widget->get_painter()->set_time (t);
    m_brute_force_widget->update ();

    m_dijkstra_widget->get_painter()->set_time (t);
    m_dijkstra_widget->update ();

    m_brute_force_with_depence_widget->get_painter()->set_time (t);
    m_brute_force_with_depence_widget->update ();

    m_dijkstra_with_depence_widget->get_painter()->set_time (t);
    m_dijkstra_with_depence_widget->update ();

    m_time_line->setText (QString(" %1").arg(t, 0, 'g'));
}

void graph_path_tab::enable_disable (bool is_stop)
{
    m_run_button->setVisible (is_stop);
    m_stop_button->setVisible (!is_stop);
    m_sleep_spinbox->setEnabled (is_stop);
    m_time_slider->setEnabled (is_stop);

    m_stop = is_stop;
}

void graph_path_tab::stop ()
{
    enable_disable (true);
}


void graph_path_tab::run ()
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

