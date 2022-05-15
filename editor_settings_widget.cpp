#include "editor_settings_widget.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QSpinBox>
#include <QGroupBox>
#include <QDoubleValidator>
#include <QLineEdit>
#include <QCheckBox>
//#include <QToolBar>

gno_editor_settings_widget::gno_editor_settings_widget(QWidget *parent): QWidget(parent)
{
    QVBoxLayout *layout = new QVBoxLayout(this);

//    //axis settings
//    {
//        QGroupBox *axis_settings_groupbox = new QGroupBox ("Axis Settings", this);

//        QGridLayout *axis_min_max_layout = new QGridLayout (axis_settings_groupbox);

//        axis_min_max_layout->addWidget (m_min_x = new QLineEdit (axis_settings_groupbox), 0, 0);
//        axis_min_max_layout->addWidget (m_max_x = new QLineEdit (axis_settings_groupbox), 0, 1);
//        axis_min_max_layout->addWidget (m_min_y = new QLineEdit (axis_settings_groupbox), 1, 0);
//        axis_min_max_layout->addWidget (m_max_y = new QLineEdit (axis_settings_groupbox), 1, 1);

//        layout->addWidget (axis_settings_groupbox);
//    }

    //show settings
    {
        QGroupBox *show_groupbox = new QGroupBox ("Show", this);

        QVBoxLayout *show_layout = new QVBoxLayout (show_groupbox);

        {
            show_layout->addWidget (m_show_names = new QCheckBox ("Names", this));
            m_show_names->setTristate (false);

            show_layout->addWidget (m_show_numbers = new QCheckBox ("Show Numbers", this));
            m_show_numbers->setTristate (false);

            QGroupBox *show_veh_groupbox = new QGroupBox ("Vehicle Path", this);
            QHBoxLayout *show_veh_layout = new QHBoxLayout (show_veh_groupbox);
            {
                show_veh_layout->addWidget (m_show_veh_path = new QCheckBox ("Show Path", this));
                m_show_veh_path->setTristate (false);
                show_veh_layout->addWidget (m_veh_num = new QSpinBox (this));
                m_veh_num->setRange (0, 1000);
            }

            show_layout->addWidget (show_veh_groupbox);
        }

        layout->addWidget (show_groupbox);
    }

    //fill options
    {
        QGroupBox *fill_groupbox = new QGroupBox ("Fill Graphs", this);

        QVBoxLayout *fill_layout = new QVBoxLayout (fill_groupbox);


        {
            fill_layout->addWidget (m_clear_button = new QPushButton ("Clear", this));
        }

        {
            QWidget *empty_widget = new QWidget (this);

            QVBoxLayout *random_layout = new QVBoxLayout (empty_widget);

            random_layout->addWidget (m_random_button = new QPushButton ("Random", this));
            random_layout->addWidget (m_random_node_count_spinbox = new QSpinBox (this));
            random_layout->addWidget (m_random_vehicle_count_spinbox = new QSpinBox (this));

            fill_layout->addWidget (empty_widget);

            m_random_node_count_spinbox->setRange (1, 100);
            m_random_vehicle_count_spinbox->setRange (1, 100);
        }

        {
            fill_layout->addWidget (m_load_default_1_button = new QPushButton ("Default 1", this));
            fill_layout->addWidget (m_load_default_2_button = new QPushButton ("Line", this));
            fill_layout->addWidget (m_load_manh_button = new QPushButton ("Manhattan", this));
            fill_layout->addWidget (m_load_default_3_button = new QPushButton ("Tree Graph", this));
            fill_layout->addWidget (m_load_default_4_button = new QPushButton ("Rombe Graph", this));
        }

        layout->addWidget (fill_groupbox);
    }

//    //editor options
//    {
//        QGroupBox *editor_groupbox = new QGroupBox ("Editor Optiopns", this);

//        QGridLayout *editor_layout = new QGridLayout (editor_groupbox);


//        {
//            QToolBar *toolbar = new QToolBar (this);
//            editor_layout->addWidget (toolbar);

//            QAction *move_action = toolbar->addAction ("Moving", this, [this] { Q_EMIT editor_state_changed_signal (editor_state::moving); });
//            move_action->setCheckable (true);
//            move_action->setChecked (true);

//            toolbar->addAction ("Deleting", this, [this] { Q_EMIT editor_state_changed_signal (editor_state::deleting); })->setCheckable (true);
//            toolbar->addAction ("Creating", this, [this] { Q_EMIT editor_state_changed_signal (editor_state::creating); })->setCheckable (true);
//            toolbar->addAction ("Connecting", this, [this] { Q_EMIT editor_state_changed_signal (editor_state::connecting); })->setCheckable (true);
//        }

//        layout->addWidget (editor_groupbox);
//    }

    QObject::connect (m_show_names, &QCheckBox::clicked, this, [this] (bool checked) { Q_EMIT show_name_signal (checked); });
    QObject::connect (m_show_numbers, &QCheckBox::clicked, this, [this] (bool checked) { Q_EMIT show_numbers_signal (checked); });
    QObject::connect (m_show_veh_path, &QCheckBox::clicked, this, [this] (bool checked) { Q_EMIT show_path (checked); });
    QObject::connect (m_veh_num, qOverload<int>(&QSpinBox::valueChanged), this, [this] (int veh_num) { Q_EMIT show_veh_path (veh_num); });
    QObject::connect (m_clear_button, &QPushButton::clicked, this, [this] () { Q_EMIT clear_signal (); });
    QObject::connect (m_random_button, &QPushButton::clicked, this, [this] ()
                     {
                         Q_EMIT fill_random_signal (m_random_node_count_spinbox->value (), m_random_vehicle_count_spinbox->value ());
                     });
    QObject::connect (m_load_default_1_button, &QPushButton::clicked, this, [this] () { Q_EMIT load_default_1_signal (); });
    QObject::connect (m_load_default_2_button, &QPushButton::clicked, this, [this] () { Q_EMIT load_default_2_signal (); });
    QObject::connect (m_load_manh_button, &QPushButton::clicked, this, [this] () { Q_EMIT load_manh_signal (); });
    QObject::connect (m_load_default_3_button, &QPushButton::clicked, this, [this] () { Q_EMIT load_default_3_signal (); });
    QObject::connect (m_load_default_4_button, &QPushButton::clicked, this, [this] () { Q_EMIT load_default_4_signal (); });
}
