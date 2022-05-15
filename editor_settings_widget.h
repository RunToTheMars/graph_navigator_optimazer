#ifndef EDITOR_SETTINGS_WIDGET_H
#define EDITOR_SETTINGS_WIDGET_H

#include <QWidget>

class QPushButton;
class QSpinBox;
class QLineEdit;
class QCheckBox;

class gno_editor_settings_widget: public QWidget
{
    Q_OBJECT

public:
  enum class editor_state
  {
    nothing,
    moving,
    deleting,
    creating,
    connecting,
  };

  gno_editor_settings_widget (QWidget *parent = nullptr);

Q_SIGNALS:
    void show_name_signal (bool show);
    void show_numbers_signal (bool show);
    void show_path (bool show);
    void show_veh_path (int veh);
    void clear_signal ();
    void fill_random_signal (unsigned int node_count, unsigned int veh_count);
    void load_default_1_signal ();
    void load_default_2_signal ();
    void load_manh_signal ();
    void load_default_3_signal ();
    void load_default_4_signal ();
    void load_default_5_signal ();

    void editor_state_changed_signal (gno_editor_settings_widget::editor_state state);

private:
//    //axis settings
//    QLineEdit *m_min_x = nullptr;
//    QLineEdit *m_max_x = nullptr;

//    QLineEdit *m_min_y = nullptr;
//    QLineEdit *m_max_y = nullptr;

    //show options
    QCheckBox *m_show_names = nullptr;
    QCheckBox *m_show_numbers = nullptr;
    QCheckBox *m_show_veh_path = nullptr;
    QSpinBox *m_veh_num = nullptr;

    //fill options
    QPushButton *m_clear_button = nullptr;

    QPushButton *m_random_button = nullptr;
    QSpinBox *m_random_node_count_spinbox = nullptr;
    QSpinBox *m_random_vehicle_count_spinbox = nullptr;

    QPushButton *m_load_default_1_button = nullptr;
    QPushButton *m_load_default_2_button = nullptr;
    QPushButton *m_load_manh_button = nullptr;
    QPushButton *m_load_default_3_button = nullptr;
    QPushButton *m_load_default_4_button = nullptr;
    QPushButton *m_load_default_5_button = nullptr;
};

#endif // EDITOR_SETTINGS_WIDGET_H
