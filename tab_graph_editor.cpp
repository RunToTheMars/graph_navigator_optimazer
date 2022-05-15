#include "tab_graph_editor.h"

#include "editable_graph_area_render.h"
#include "editor_settings_widget.h"
#include "default_graphs.h"
#include "graph_painter.h"
#include "color_builder.h"

#include <QGroupBox>
#include <QHBoxLayout>
#include <QSplitter>

graph_editor_tab::graph_editor_tab (graph::graph_initial *graph_initial, QWidget *parent) : QWidget (parent), m_graph_initial (graph_initial)
{
  QHBoxLayout *horizon_layout = new QHBoxLayout (this);

  // settings
  QGroupBox *settings_groupbox = new QGroupBox ("Settings", this);
  {
    QVBoxLayout *setting_vertial_layout = new QVBoxLayout (settings_groupbox);
    settings_groupbox->setLayout (setting_vertial_layout);

    setting_vertial_layout->setAlignment (Qt::AlignTop);
    setting_vertial_layout->addWidget (m_editor_settings = new gno_editor_settings_widget (settings_groupbox));
  }

  m_editor_settings->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);

  //painter
  QGroupBox *graph_groupbox = new QGroupBox ("Graph", this);
  {
    QVBoxLayout *algorithms_vertial_layout = new QVBoxLayout (graph_groupbox);
    graph_groupbox->setLayout (algorithms_vertial_layout);

    algorithms_vertial_layout->addWidget (m_editable_graph_area_render = new editable_graph_area_render (m_graph_initial, graph_groupbox));
  }

  QSplitter *splitter = new QSplitter (Qt::Orientation::Horizontal, this);
  splitter->addWidget (settings_groupbox);
  splitter->addWidget (graph_groupbox);
  horizon_layout->addWidget (splitter);

  QObject::connect (m_editor_settings, &gno_editor_settings_widget::show_name_signal, this, [this] (bool show) {
      m_editable_graph_area_render->get_painter ()->set_show_names (show);
      m_editable_graph_area_render->update ();
  });

  QObject::connect (m_editor_settings, &gno_editor_settings_widget::show_numbers_signal, this, [this] (bool show) {
      m_editable_graph_area_render->get_painter ()->set_show_numbers (show);
      m_editable_graph_area_render->update ();
  });

  QObject::connect (m_editor_settings, &gno_editor_settings_widget::show_path, this, [this] (bool show) {
      m_editable_graph_area_render->get_painter ()->set_show_path (show);
      m_editable_graph_area_render->update ();
  });

  QObject::connect (m_editor_settings, &gno_editor_settings_widget::show_veh_path, this, [this] (int veh_num) {
      graph::uid veh_count = m_graph_initial->get_initial_state()->vehicle_count ();
      if (veh_num < 0 || veh_num >= veh_count)
        return;

      default_color_builder builder;
      builder.set_val_range  (0, veh_count);

      const std::vector<graph::uid> &path = m_graph_initial->get_initial_state()->vehicle (veh_num).path;
      m_editable_graph_area_render->get_painter ()->set_path (path);
      m_editable_graph_area_render->get_painter ()->set_path_color (builder.get_color (veh_num));
      m_editable_graph_area_render->update ();
  });

  QObject::connect (m_editor_settings, &gno_editor_settings_widget::clear_signal, this, [this] ()
                   {
                       m_graph_initial->get_graph()->clear ();
                       m_graph_initial->get_initial_state()->clear ();
                       m_editable_graph_area_render->update ();
                       Q_EMIT graph_changed ();
                   });

  QObject::connect (m_editor_settings, &gno_editor_settings_widget::fill_random_signal, this, [this] (unsigned int node_count, unsigned int veh_count) {
      graph::fill_random (m_graph_initial, node_count, veh_count);
      m_editable_graph_area_render->update ();
      Q_EMIT graph_changed ();
  });

  QObject::connect (m_editor_settings, &gno_editor_settings_widget::load_default_1_signal, this, [this] () {
      graph::set_default_graph_1(m_graph_initial);
      m_editable_graph_area_render->update ();
      Q_EMIT graph_changed ();
  });

  QObject::connect (m_editor_settings, &gno_editor_settings_widget::load_default_2_signal, this, [this] () {
      graph::set_default_graph_2(m_graph_initial);
      m_editable_graph_area_render->update ();
      Q_EMIT graph_changed ();
  });

  QObject::connect (m_editor_settings, &gno_editor_settings_widget::load_manh_signal, this, [this] () {
      graph::set_graph_manh(m_graph_initial);
      m_editable_graph_area_render->update ();
      Q_EMIT graph_changed ();
  });

  QObject::connect (m_editor_settings, &gno_editor_settings_widget::load_default_3_signal, this, [this] () {
      graph::set_default_graph_3(m_graph_initial);
      m_editable_graph_area_render->update ();
      Q_EMIT graph_changed ();
  });

  QObject::connect (m_editor_settings, &gno_editor_settings_widget::load_default_4_signal, this, [this] () {
      graph::set_default_graph_4(m_graph_initial);
      m_editable_graph_area_render->update ();
      Q_EMIT graph_changed ();
  });
}
