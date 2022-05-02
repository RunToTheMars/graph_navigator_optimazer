#include "path_graph_widget.h"

#include "utils.h"
#include "gno_modeling.h"
#include "gno_path_finder.h"
#include "model_graph_area_render.h"
#include "veh_on_graph_painter.h"

#include "QPushButton"
#include "QVBoxLayout"
#include "QLineEdit"

path_graph_widget::path_graph_widget(graph::graph_initial *graph_initial, graph::gno_path_finder_base *path_finder,
                                     graph::gno_continuous_modeling *continuous_modeling, QWidget *parent):
    QWidget (parent), m_path_finder (path_finder), m_continuous_modeling (continuous_modeling)
{
    m_path_finder = path_finder;

    QVBoxLayout *horizon_layout = new QVBoxLayout (this);
    QPushButton *run_button = new QPushButton ("Solve", this);

    horizon_layout->addWidget (area_render = new model_graph_area_render (graph_initial, this));
    horizon_layout->addWidget (run_button);
    horizon_layout->addWidget (m_res_line = new QLineEdit (this));
    m_res_line->setDisabled (true);
    m_res_line->setText ("?");

    QObject::connect (
        run_button, &QPushButton::clicked, this, [this, graph_initial] {
            graph::Vehicle veh;
            m_best_path = m_path_finder->run (*graph_initial, m_src, m_dst, veh, m_start_time);

            graph::graph_initial_state_impl graph_initial_state;
            for (graph::uid veh_uid = 0; veh_uid < graph_initial->get_initial_state ()->vehicle_count (); veh_uid++)
              graph_initial_state.add_vehicle (graph_initial->get_initial_state ()->vehicle (veh_uid));

            graph::Directional_Vehicle new_veh;
            new_veh.src = m_src;
            new_veh.dst = m_dst;
            new_veh.path = m_best_path;
            new_veh.vehicle = veh;
            new_veh.t = m_start_time;

            graph_initial_state.add_vehicle (new_veh);
            graph::graph_initial new_initial_state (const_cast<graph::graph_base *>(graph_initial->get_graph ()), &graph_initial_state);

            m_line_states.clear ();

            area_render->get_painter ()->set_line_states (&m_line_states);
            m_continuous_modeling->set_do_on_linear_time (
                [this] (const graph::vehicle_continuous_line_states &states) {
                    m_line_states.push_back (states);
                });

            int r = m_continuous_modeling->run (new_initial_state);
            if (!OK (r) || m_line_states.empty ())
            {
                m_line_states.clear ();
                m_res_line->setText ("?");
                return;
            }

            double t_tes = -1.;
            for (const graph::vehicle_continuous_line_states &line_states : m_line_states)
            {
              const graph::vehicle_continuous_state &target_state = line_states.states.back ();
              if (fuzzy_eq (target_state.part_end, 1.) && target_state.edge_uid_end == new_veh.path.back ())
              {
                t_tes = line_states.t2;
                break;
              }
            }

            m_res_line->setText (QString ("%1").arg (t_tes));
            get_painter ()->set_time (0.);
            area_render->update ();

            Q_EMIT update_times_needed ();
        });
}

path_graph_widget::~path_graph_widget () = default;

void path_graph_widget::set_src_dst (graph::uid src, graph::uid dst)
{
  area_render->get_painter ()->set_src (src);
  area_render->get_painter ()->set_dst (dst);
  area_render->get_painter ()->update ();

  m_src = src;
  m_dst = dst;
}

veh_on_graph_painter *path_graph_widget::get_painter ()
{
    return area_render->get_painter ();
};
