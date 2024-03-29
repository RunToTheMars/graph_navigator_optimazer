#include "model_graph_widget.h"

#include "utils.h"
#include "gno_modeling.h"
#include "model_graph_area_render.h"
#include "veh_on_graph_painter.h"

#include "QPushButton"
#include "QVBoxLayout"

model_graph_widget::model_graph_widget(graph::graph_initial *graph_initial, graph::gno_discrete_modeling_base *discrete_modeling, QWidget *parent):
    QWidget (parent), m_discrete_modeling (discrete_modeling)
{
  m_continuous_modeling = std::make_unique<graph::gno_continuous_modeling> (m_discrete_modeling);

  QVBoxLayout *horizon_layout = new QVBoxLayout (this);
  QPushButton *run_button = new QPushButton ("Calculate", this);

  horizon_layout->addWidget (area_render = new model_graph_area_render (graph_initial, this));
  horizon_layout->addWidget (run_button);

  QObject::connect (
      run_button, &QPushButton::clicked, this, [this, graph_initial] {
        m_line_states.clear ();
        area_render->get_painter ()->set_line_states (&m_line_states);

        m_continuous_modeling->set_do_on_linear_time (
            [this] (const graph::vehicle_continuous_line_states &states) {
                m_line_states.push_back (states);
            });

        m_discrete_modeling->clear_states (*graph_initial);
        int r = m_continuous_modeling->run (*graph_initial);
        if (!OK (r))
        {
          m_line_states.clear ();
          return;
        }

        set_time (0.);
        area_render->update ();

        Q_EMIT update_times_needed ();
      });
}

model_graph_widget::~model_graph_widget () = default;

void model_graph_widget::set_time (double t)
{
  area_render->get_painter ()->set_time (t);
}

veh_on_graph_painter *model_graph_widget::get_painter ()
{
  return area_render->get_painter ();
};
