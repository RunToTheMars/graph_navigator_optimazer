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
    QObject::connect (m_clear_button, &QPushButton::clicked, this, [this] () { Q_EMIT clear_signal (); });
    QObject::connect (m_random_button, &QPushButton::clicked, this, [this] ()
                     {
                         Q_EMIT fill_random_signal (m_random_node_count_spinbox->value (), m_random_vehicle_count_spinbox->value ());
                     });
    QObject::connect (m_load_default_1_button, &QPushButton::clicked, this, [this] () { Q_EMIT load_default_1_signal (); });
}
