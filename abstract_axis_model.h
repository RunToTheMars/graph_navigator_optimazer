#ifndef ABSTRACT_AXIS_MODEL_H
#define ABSTRACT_AXIS_MODEL_H

class QVariant;

enum class axis_settings
{
    draw_grid,        /// bool
    draw_axis,        /// bool
    min_x,            /// double
    min_y,            /// double
    max_x,            /// double
    max_y,            /// double
    background_color, /// QColor
    name_x,           /// QString
    name_y,           /// QString
    precision,        /// int
};

class abstract_axis_model
{
public:
    virtual ~abstract_axis_model () = default;

    virtual QVariant data (axis_settings setting) const = 0;
};

#endif // ABSTRACT_AXIS_MODEL_H
