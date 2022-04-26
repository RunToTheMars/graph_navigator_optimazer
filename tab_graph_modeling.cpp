#include "tab_graph_modeling.h"

#include "model_graph_widget.h"
#include "gno_modeling_simple.h"
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

graph_modeling_tab::graph_modeling_tab (graph::graph_initial *graph_initial, QWidget *parent): QWidget(parent), m_graph_initial (graph_initial)
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

  // models
  QGroupBox *models_groupbox = new QGroupBox ("Models", this);
  {
      QGridLayout *models_layout = new QGridLayout (models_groupbox);
      models_groupbox->setLayout (models_layout);

      {
          QGroupBox *model_simple_groupbox = new QGroupBox ("Simple 1", this);
          QVBoxLayout *model_simple_layout = new QVBoxLayout (model_simple_groupbox);
          model_simple_groupbox->setLayout (model_simple_layout);

          m_simple_model = std::make_unique<graph::gno_modeling_simple> ();
          model_simple_layout->addWidget (m_simple_model_widget = new model_graph_widget (graph_initial, m_simple_model.get (),model_simple_groupbox));
          models_layout->addWidget (model_simple_groupbox, 0, 0);
      }

      {
          QGroupBox *model_simple_groupbox = new QGroupBox ("Simple 2", this);
          QVBoxLayout *model_simple_layout = new QVBoxLayout (model_simple_groupbox);
          model_simple_groupbox->setLayout (model_simple_layout);

//          m_simple_model = std::make_unique<graph::gno_modeling_simple> ();
//          model_simple_layout->addWidget (m_simple_model_widget = new model_graph_widget (graph_initial, m_simple_model.get (),model_simple_groupbox));
          models_layout->addWidget (model_simple_groupbox, 0, 1);
      }

      {
          QGroupBox *model_simple_groupbox = new QGroupBox ("Simple 3", this);
          QVBoxLayout *model_simple_layout = new QVBoxLayout (model_simple_groupbox);
          model_simple_groupbox->setLayout (model_simple_layout);

//          m_simple_model = std::make_unique<graph::gno_modeling_simple> ();
//          model_simple_layout->addWidget (m_simple_model_widget = new model_graph_widget (graph_initial, m_simple_model.get (),model_simple_groupbox));
          models_layout->addWidget (model_simple_groupbox, 1, 0);
      }

      {
          QGroupBox *model_simple_groupbox = new QGroupBox ("Simple 4", this);
          QVBoxLayout *model_simple_layout = new QVBoxLayout (model_simple_groupbox);
          model_simple_groupbox->setLayout (model_simple_layout);

//          m_simple_model = std::make_unique<graph::gno_modeling_simple> ();
//          model_simple_layout->addWidget (m_simple_model_widget = new model_graph_widget (graph_initial, m_simple_model.get (),model_simple_groupbox));
          models_layout->addWidget (model_simple_groupbox, 1, 1);
      }
  }


  QSplitter *splitter = new QSplitter (Qt::Orientation::Vertical, this);
  splitter->addWidget (time_groupbox);
  splitter->addWidget (models_groupbox);
  vertical_layout->addWidget (splitter);

  QObject::connect (m_run_button, &QPushButton::clicked, this, [this] { run (); });
  QObject::connect (m_stop_button, &QPushButton::clicked, this, [this] { stop (); });

  QObject::connect (m_time_slider, &QSlider::valueChanged, this, [this] { set_time_from_slider (); });
  QObject::connect (m_simple_model_widget, &model_graph_widget::update_times_needed, this, [this] { update_max_times (); });
  QObject::connect (this, &graph_modeling_tab::set_val, this, [this] (int i) { m_time_slider->setValue (i); });
  QObject::connect (this, &graph_modeling_tab::done, this, [this] { stop (); });
}

void graph_modeling_tab::update_max_times ()
{
  if (m_simple_model_widget->get_line_states().empty ())
    return;

  m_max_time = m_simple_model_widget->get_line_states().back().t2;

}

void graph_modeling_tab::set_time_from_slider ()
{

  double t = m_time_slider->value () * m_max_time / max_slider_count;
  set_time (t);
}


void graph_modeling_tab::set_time (double t)
{
  m_simple_model_widget->set_time (t);
  m_simple_model_widget->update ();
  m_time_line->setText (QString(" %1").arg(t, 0, 'g'));
}

void graph_modeling_tab::enable_disable (bool is_stop)
{
  m_run_button->setVisible (is_stop);
  m_stop_button->setVisible (!is_stop);
  m_sleep_spinbox->setEnabled (is_stop);
  m_time_slider->setEnabled (is_stop);

  m_stop = is_stop;
}

void graph_modeling_tab::stop ()
{
  enable_disable (true);
}


void graph_modeling_tab::run ()
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
