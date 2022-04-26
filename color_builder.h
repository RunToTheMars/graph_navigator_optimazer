#ifndef COLOR_BUILDER_H
#define COLOR_BUILDER_H

class QColor;

class color_builder_base
{
public:
    virtual QColor get_color (double val) const = 0;
    virtual void set_val_range (double min_val, double max_val) = 0;
};

class default_color_builder final : public color_builder_base
{
  double m_min_val = 0.;
  double m_max_val = 1.;
  double m_range_val_reverse = 1.;

public:
  QColor get_color (double val) const override;
  void set_val_range (double min_val, double max_val) override
  {
    m_min_val = min_val;
    m_max_val = max_val;

    m_range_val_reverse = 1. / (m_max_val - m_min_val);
  }
};

#endif // COLOR_BUILDER_H
