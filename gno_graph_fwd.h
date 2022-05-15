#ifndef GNO_GRAPH_FWD_H
#define GNO_GRAPH_FWD_H

#include <string>
#include <vector>
#include "utils.h"

namespace graph
{
//graph
using uid = int;
static constexpr uid invalid_uid = -1;

//time_state
using time = double;
static constexpr time undef_time = -1.;

static constexpr double V_MAX = 60.;
static constexpr double V_MIN = 10.;
static constexpr double D = 500.;
static constexpr double A_MAX = V_MAX * V_MAX / (2 * D);
static constexpr double L = 100;

static constexpr bool MINIMIZE = true;

struct Node
{
    double x = 0.;
    double y = 0.;

    std::string name = "";
};

struct Edge
{
    uid start = invalid_uid;
    uid end = invalid_uid;
    double length = 1.;
};

struct Vehicle
{
    double weight = 1.;
};

struct vehicle_discrete_state
{
    double part = 0.;
    size_t edge_num = 0;
};

struct vehicle_continuous_state
{
    graph::uid edge_uid_start = graph::invalid_uid;
    graph::uid edge_uid_end = graph::invalid_uid;

    double part_start = 0.;
    double part_end = 0.;

    size_t edge_num_start = -1;
    size_t edge_num_end = -1;


    std::pair<int, double> get_edge_and_par (const double time, const double t1, const double t2) const
    {
        if (fuzzy_eq (t2, t1))
          return {1, part_end};

        if (edge_uid_start == edge_uid_end)
          return  {1, (part_end - part_start) * (time - t1) / (t2 - t1) + part_start};
        else
        {
            double p = (part_end - part_start + 1) * (time - t1) / (t2 - t1);
            if (p < 1 - part_start)
              return  {0, part_start + p};
            else
              return {1, p - 1 + part_start};
        }
    };
};

struct vehicle_continuous_line_states
{
    double t1;
    double t2;

    std::vector<vehicle_continuous_state> states;

    vehicle_continuous_line_states (double t1_, double t2_,
                                   std::vector<vehicle_continuous_state> &&states_)
    {
        t1 = t1_;
        t2 = t2_;
        states = std::move (states_);
    }

    bool contains (double t) const
    {
      return fuzzy_eq (t, t1) || fuzzy_eq (t, t2) || (t1 < t && t < t2);
    }
};

class graph_base;
class graph_initial_state_base;
class graph_initial;

class gno_discrete_modeling_base;
class gno_continuous_modeling;

class gno_path_finder_base;
class gno_multipath_finder_base;
}

#endif // GNO_GRAPH_FWD_H
