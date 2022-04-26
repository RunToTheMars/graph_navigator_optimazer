#ifndef DEFAULT_GRAPHS_H
#define DEFAULT_GRAPHS_H

#include "gno_graph.h"
#include "gno_graph_initial_state.h"
#include "gno_graph_initial.h"

#include <array>
#include <set>
#include <utility>
#include <random>

namespace graph
{
    void set_graph (graph::graph_initial *graph_initial,
                    const std::vector<Node> &nodes,
                    const std::vector<Edge> &edges,
                    const std::vector<Directional_Vehicle> &dir_vehs)
    {
      graph::graph_base *graph = graph_initial->get_graph ();
      graph->clear ();

      for (auto &node : nodes)
        graph->add_node (node);

      for (auto &edge : edges)
        graph->add_edge (edge);

      graph_initial_state_base *initial_state = graph_initial->get_initial_state ();
      initial_state->clear ();

      for (auto dir_veh: dir_vehs)
        initial_state->add_vehicle (dir_veh);

      graph->rebuild_maps ();
      graph->calculate_bounds ();
    }

    void fill_random (graph::graph_initial *graph_initial, unsigned int node_count, unsigned int vehicle_count, int seed = 0,
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
      for (unsigned int i = 0; i < node_count - 1; i++)
      {
          Edge edge;
          edge.start = i;
          edge.end = i + 1;

          edges.push_back (edge);
      }

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

      std::vector<Directional_Vehicle> dir_vehs;
      for (unsigned int i = 0; i < vehicle_count; i++)
      {
        graph::uid src_node_uid = static_cast<graph::uid> (node_random (gen));
        graph::uid dst_node_uid = static_cast<graph::uid> (node_random (gen));
        while (src_node_uid == (dst_node_uid = static_cast<graph::uid> (node_random (gen))));

        double w = w_random (gen);
        Directional_Vehicle dir_veh;
        dir_veh.vehicle.weight = w;
        dir_veh.path = {src_node_uid, dst_node_uid};
        dir_vehs.push_back (dir_veh);
      }

     set_graph (graph_initial, nodes, edges, dir_vehs);
    }

    void set_graph_manh (graph::graph_initial *graph_initial)
    {
        const int n = 20;
        const int m = 20;

        std::vector<Node> nodes;

        for (int i = 0; i < n; i++)
        {
          for (int j = 0; j < m; j++)
          {
            Node n = {(double) j, (double) i, ""};
            nodes.push_back (n);
          }
        }

        std::vector<Edge> edges;
        for (int i = 0; i < n; i++)
        {
            for (int j = 0; j < m; j++)
            {
                std::vector<int> indexes;

                int top = (i + 1) * m + j;
                int left = i * m + (j - 1);
                int right = i * m + (j + 1);
                int bottom = (i - 1) * m + j;

                if (i != n - 1)
                    indexes.push_back (top);

                if (i != 0)
                    indexes.push_back (bottom);

                if (j != 0)
                    indexes.push_back (left);

                if (j != m - 1)
                    indexes.push_back (right);

                for (int ind : indexes)
                {
                  if (ind < 0 || ind >= n * m)
                    continue;
                  Edge e = {i * m + j, ind, 3 * graph::D};
                  edges.push_back (e);
                }
            }
        }

        std::vector<Directional_Vehicle> vehs;

        const int veh_count = 100;

        std::mt19937 gen (0);
        std::uniform_real_distribution<double> node_random (0, n * m);
        std::uniform_real_distribution<double> path_count_random (3, 10);
        std::uniform_real_distribution<double> dir_4_random (0, 3);
        std::uniform_real_distribution<double> dir_3_random (0, 2);
        std::uniform_real_distribution<double> dir_2_random (0, 1);

        std::set<graph::uid> nodes_set;
        for (int veh_i = 0; veh_i < veh_count; veh_i ++)
        {
            Directional_Vehicle veh;

            int count = static_cast<int> (path_count_random (gen));
            graph::uid v = static_cast<graph::uid> (node_random (gen));
            veh.src = v;
            veh.path.push_back (v);
            nodes_set.insert (v);

            for (int step = 0; step < count; step++)
            {
              int i = veh.path.back () / m;
              int j = veh.path.back () % m;

              if (i > 0 && i < n - 1 && j > 0 && j < m - 1)
              {
                int dir = static_cast<int> (dir_4_random (gen));
                int ind;

                if (dir == 0)
                  ind = (i + 1) * m + j;
                else if (dir == 1)
                  ind = (i - 1) * m + j;
                else if (dir == 2)
                  ind = i * m + j + 1;
                else if (dir == 3)
                  ind = i * m + j - 1;

                if (!nodes_set.insert (ind).second)
                  continue;

                veh.path.push_back (ind);
              }
            }
            veh.dst = veh.path.back ();

            if (veh.path.size () < 2)
              continue;

            vehs.push_back (veh);
        }

        set_graph (graph_initial, nodes, edges, vehs);
    }

    void set_default_graph_1 (graph::graph_initial *graph_initial)
    {
        std::vector<Node> nodes = {{-1, 0.5, "A"}, {0., 0.5, ""}, {0.5, 1., ""}, {0.5, 0., ""}, {1, 0.5, ""}, {2, 0.5, "B"}};

        std::vector<Edge> edges = {{0, 1, graph::D}, {0, 1, graph::D}, {1, 2, graph::D}, {1, 3, graph::D}, {2, 4, graph::D}, {3, 4, graph::D}, {4, 5, graph::D}};

        Directional_Vehicle v1;
        v1.src = 0;
        v1.dst = 5;
        v1.path = {0, 1, 2, 4, 5};

        Directional_Vehicle v2;
        v2.src = 0;
        v2.dst = 5;
        v2.path = {0, 1, 3, 4, 5};


        std::vector<Directional_Vehicle> dir_vehs = {v1, v2};

        set_graph (graph_initial, nodes, edges, dir_vehs);
    }
}

#endif // DEFAULT_GRAPHS_H
