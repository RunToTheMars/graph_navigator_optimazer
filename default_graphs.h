#ifndef DEFAULT_GRAPHS_H
#define DEFAULT_GRAPHS_H

#include "gno_graph.h"
#include "gno_graph_initial_state.h"
#include "gno_graph_initial.h"

#include <array>
#include <utility>
#include <random>

namespace graph
{
    template <typename Node, typename Edge, typename Vehicle>
    void set_graph (graph::graph_initial <Node, Edge, Vehicle> *graph_initial,
                    const std::vector<Node_Base> &nodes,
                    const std::vector<Edge_Base> &edges,
                    const std::vector<Directional_Vehicle<Vehicle_Base>> &dir_vehs)
    {
      graph::graph_base <Node, Edge> *graph = graph_initial->get_graph ();
      graph->clear ();

      for (auto &node : nodes)
        {
          Node n;
          n.x = node.x;
          n.y = node.y;
          n.name = node.name;
          graph->add_node (n);
        }

      for (auto &edge : edges)
      {
          Edge e;
          e.start = edge.start;
          e.end = edge.end;
          e.length = edge.length;
          graph->add_edge (e);
      }

      graph_initial_state_base<Vehicle> *initial_state = graph_initial->get_initial_state ();
      initial_state->clear ();

      for (auto dir_veh: dir_vehs)
      {
          Directional_Vehicle<Vehicle> d_v;
          d_v.vehicle.weight = dir_veh.vehicle.weight;
          d_v.path = dir_veh.path;

          initial_state->add_vehicle (d_v);
      }

      graph->calculate_bounds ();
    }

    template <typename Node, typename Edge, typename Vehicle>
    void fill_random (graph::graph_initial <Node, Edge, Vehicle> *graph_initial, unsigned int node_count, unsigned int vehicle_count, int seed = 0,
                      double min_x = 0, double max_x = 1, double min_y = 0, double max_y = 1, double edge_probability = 0.1,
                      double min_w = 0.1, double max_w = 10.)
    {
      std::mt19937 gen (seed);
      std::uniform_real_distribution<double> x_unif (min_x, max_x);
      std::uniform_real_distribution<double> y_unif (min_y, max_y);

      std::vector<Node> nodes;
      nodes.reserve (node_count);

      for (unsigned int i = 0; i < node_count; i++)
      {
        Node n;
        n.x = x_unif (gen);
        n.y = y_unif (gen);
        n.name = std::to_string (i);

        nodes.push_back (n);
      }


      std::uniform_real_distribution<double> edge_random (0, 1);

      std::vector<Edge> edges;
      for (unsigned int i = 0; i < node_count; i++)
      {
          for (unsigned int j = i; j < node_count; j++)
          {
            double p = edge_random (gen);
            if (p < edge_probability)
            {
              Edge edge;
              edge.start = i;
              edge.end = j;

              edges.push_back (edge);
            }
          }
      }

      std::uniform_real_distribution<double> w_random (min_w, max_w);
      std::uniform_real_distribution<double> node_random (0, node_count);

      if (node_count <= 1)
      {
        set_graph (graph_initial, nodes, edges, {});
        return;
      }

      std::vector<Directional_Vehicle<Vehicle_Base>> dir_vehs;
      for (unsigned int i = 0; i < vehicle_count; i++)
      {
        graph::uid src_node_uid = static_cast<graph::uid> (node_random (gen));
        graph::uid dst_node_uid = static_cast<graph::uid> (node_random (gen));
        while (src_node_uid == (dst_node_uid = static_cast<graph::uid> (node_random (gen))));

        double w = w_random (gen);
        Directional_Vehicle<Vehicle> dir_veh;
        dir_veh.vehicle.weight = w;
        dir_veh.path = {src_node_uid, dst_node_uid};
        dir_vehs.push_back (dir_veh);
      }

     set_graph (graph_initial, nodes, edges, dir_vehs);
    }

    template <typename Node, typename Edge, typename Vehicle>
    void set_default_graph_1 (graph::graph_initial <Node, Edge, Vehicle> *graph_initial)
    {
        std::vector<Node_Base> nodes = {{-1, 0.5, "A"}, {0., 0.5, ""}, {0.5, 1., ""}, {0.5, 0., ""}, {1, 0.5, ""}, {2, 0.5, "B"}};

        std::vector<Edge_Base> edges = {{0, 1}, {1, 2}, {1, 3}, {2, 4}, {3, 4}, {4, 5}};

        std::vector<Directional_Vehicle<Vehicle_Base>> dir_vehs = {{{1.}, {{0, 5}}}};

        set_graph (graph_initial, nodes, edges, dir_vehs);
    }
}

#endif // DEFAULT_GRAPHS_H
