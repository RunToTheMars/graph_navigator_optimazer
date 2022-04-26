#ifndef GNO_MODELING_DIJKSTRA_H
#define GNO_MODELING_DIJKSTRA_H

#include "gno_modeling.h"

namespace graph
{
class gno_dijkstra_modeling: public gno_discrete_modeling_base
{
public:
   int run (const graph_initial &initial_state) override;
};


}

#endif // GNO_MODELING_DIJKSTRA_H
