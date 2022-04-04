#ifndef GNO_GRAPH_FWD_H
#define GNO_GRAPH_FWD_H

#include <string>

namespace graph
{
//graph
using uid = int;
static constexpr uid invalid_uid = -1;

//time_state
using time = double;
static constexpr time undef_time = -1.;

struct Node_Base
{
    double x = 0.;
    double y = 0.;

    std::string name = "";
};

struct Edge_Base
{
    uid start = invalid_uid;
    uid end = invalid_uid;
    double length = 1.;
};

struct Vehicle_Base
{
    double weight = 1.;
};
}

//namespace graph
//{
//struct Node_Base;
//struct Edge_Base;

//template <typename Node = Node_Base, typename Base = Edge_Base>
//class graph_base;
//}

#endif // GNO_GRAPH_FWD_H
