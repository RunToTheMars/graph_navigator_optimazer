#include "multipath_graph_widget.h"

#include "utils.h"
#include "gno_modeling.h"
#include "gno_multipath_finder.h"
#include "model_graph_area_render.h"
#include "veh_on_graph_painter.h"

#include "QPushButton"
#include "QVBoxLayout"
#include "QLineEdit"

multipath_graph_widget::multipath_graph_widget(graph::graph_initial *graph_initial, graph::gno_multipath_finder_base *multipath_finder,
                                     graph::gno_continuous_modeling *continuous_modeling, QWidget *parent):
    QWidget (parent), m_multipath_finder (multipath_finder), m_continuous_modeling (continuous_modeling)
{
    QVBoxLayout *horizon_layout = new QVBoxLayout (this);
    QPushButton *run_button = new QPushButton ("Solve", this);

    m_new_graph_initial_state = std::make_unique<graph::graph_initial_state_impl> ();
    m_new_graph_initial = std::make_unique<graph::graph_initial> (const_cast<graph::graph_base *>(graph_initial->get_graph ()), m_new_graph_initial_state.get ());

    horizon_layout->addWidget (area_render = new model_graph_area_render (m_new_graph_initial.get (), this));
    horizon_layout->addWidget (run_button);
    horizon_layout->addWidget (m_res_line = new QLineEdit (this));
    m_res_line->setDisabled (true);
    m_res_line->setText ("?");

    QObject::connect (
        run_button, &QPushButton::clicked, this, [this, graph_initial] {
            m_best_multipath = m_multipath_finder->run (*graph_initial);

            m_new_graph_initial_state->clear ();
            for (graph::uid veh_uid = 0; veh_uid < graph_initial->get_initial_state ()->vehicle_count (); veh_uid++)
            {
              graph::Directional_Vehicle veh = graph_initial->get_initial_state ()->vehicle (veh_uid);
              veh.path = m_best_multipath[veh_uid];
              m_new_graph_initial_state->add_vehicle (veh);
            }

            m_line_states.clear ();

            m_continuous_modeling->discrete_model ()->set_model_independer (graph::invalid_uid);
            m_continuous_modeling->set_do_on_linear_time (
                [this] (const graph::vehicle_continuous_line_states &states) {
                    m_line_states.push_back (states);
                });

            int r = m_continuous_modeling->run (*m_new_graph_initial);
            if (!OK (r) || m_line_states.empty ())
            {
                m_line_states.clear ();
                m_res_line->setText ("?");
                return;
            }

            area_render->get_painter ()->set_line_states (&m_line_states);

            std::vector<double> times (graph_initial->get_initial_state ()->vehicle_count(), 0.);

            for (const graph::vehicle_continuous_line_states &line_states : m_line_states)
            {
                for (graph::uid veh_uid = 0; veh_uid < graph_initial->get_initial_state ()->vehicle_count (); veh_uid++)
                    {
                      const graph::Directional_Vehicle &veh = m_new_graph_initial_state->vehicle (veh_uid);

                      if (times[veh_uid] > 0)
                        continue;

                      const graph::vehicle_continuous_state &target_state = line_states.states[veh_uid];
                      if (fuzzy_eq (target_state.part_end, 1.) && target_state.edge_uid_end == veh.path.back ())
                      {
                        times[veh_uid] = line_states.t2;
                        break;
                      }
                    }
            }

            double average_time = 0.;
            for (graph::uid veh_uid = 0; veh_uid < graph_initial->get_initial_state ()->vehicle_count (); veh_uid++)
              average_time += times[veh_uid];

            average_time /= graph_initial->get_initial_state ()->vehicle_count ();

            m_res_line->setText (QString ("%1").arg (average_time));
            get_painter ()->set_time (0.);
            area_render->update ();

            Q_EMIT update_times_needed ();
        });
}

multipath_graph_widget::~multipath_graph_widget () = default;

veh_on_graph_painter *multipath_graph_widget::get_painter ()
{
    return area_render->get_painter ();
};
