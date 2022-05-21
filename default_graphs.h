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
    static constexpr int veh_on_rombe = 50;
    static constexpr int rombe_depth = 4;

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

//     set_graph (graph_initial, nodes, edges, dir_vehs);
    }

    void set_graph_manh (graph::graph_initial *graph_initial)
    {
        const int n = 10;
        const int m = 10;

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

        //for edges manipulation
        set_graph (graph_initial, nodes, edges, vehs);

        const int veh_count = 50;

        std::mt19937 gen (0);
        std::uniform_real_distribution<double> node_random (0, nodes.size ());
        std::uniform_real_distribution<double> path_count_random (5, nodes.size () / 5);
        std::uniform_real_distribution<double> dir_random (0, 4);

        for (int veh_i = 0; veh_i < veh_count; veh_i ++)
        {
            std::set<graph::uid> nodes_set;

            Directional_Vehicle veh;

            const size_t path_size = static_cast<size_t> (path_count_random (gen));
            const graph::uid src = static_cast<graph::uid> (node_random (gen));
            veh.src = src;

            graph::uid prev_node = src;
            for (size_t step = 0; step < path_size; step++)
            {
                const std::vector<graph::uid> edges = graph_initial->get_graph ()->edges_started_from (prev_node);
                const size_t random_dir = static_cast<size_t> (dir_random (gen)) % edges.size ();
                const graph::uid edge_uid = edges[random_dir];

                const graph::uid node_end = graph_initial->get_graph ()->edge (edge_uid).end;
                if (nodes_set.insert (node_end).second == false)
                  continue;
                veh.path.push_back (edge_uid);
                prev_node = node_end;
            }

            veh.dst = prev_node;
            veh.t = veh_i;

            if (veh.path.size () == 0)
              continue;

            vehs.push_back (veh);
        }

        set_graph (graph_initial, nodes, edges, vehs);
    }

    void set_default_graph_1 (graph::graph_initial *graph_initial)
    {
        const double l = 3 * graph::D;
        const std::vector<Node> nodes = {{-1, 0.5, "A"}, {0., 0.5, ""}, {0.5, 1., ""}, {0.5, 0., ""}, {1, 0.5, ""}, {2, 0.5, "B"}};
        const std::vector<Edge> edges = {{0, 1, l}, {0, 1, l}, {1, 2, l}, {1, 3, l}, {2, 4, l}, {3, 4, l}, {4, 5, l}};

        Directional_Vehicle v1;
        v1.src = 0;
        v1.dst = 5;
        v1.path = {0, 2, 4, 6};
        v1.t = 0.;

        Directional_Vehicle v2;
        v2.src = 0;
        v2.dst = 5;
        v2.path = {1, 3, 5, 6};
        v2.t = 10.;

        const std::vector<Directional_Vehicle> dir_vehs = {v1, v2};

        set_graph (graph_initial, nodes, edges, dir_vehs);
    }

    void set_default_graph_2 (graph::graph_initial *graph_initial)
    {
        const double l = 10 * graph::D;
        const std::vector<Node> nodes = {{0, 0, "A"}, {1, 1, "B"}};
        const std::vector<Edge> edges = {{0, 1, l}};

        std::vector<Directional_Vehicle> dir_vehs;
        for (int i = 0; i < 10; i++)
        {
            Directional_Vehicle v;
            v.src = 0;
            v.dst = 1;
            v.path = {0};
            v.t = i * i;

            dir_vehs.push_back (v);
        }

        set_graph (graph_initial, nodes, edges, dir_vehs);
    }

    void set_default_graph_3 (graph::graph_initial *graph_initial)
    {
        std::mt19937 gen (0);
        std::uniform_real_distribution<double> node_height (graph::D, 10 * graph::D);
        const int depth = 4;
        std::vector<Node> nodes = {{0., 0., "A"}};
        std::vector<Edge> edges;

        int pow = 1;
        for (int i = 1; i < 6; i++)
        {
            const double dist = 1. / (pow * 2);
            const int nodes_count = isize (nodes);
            for (int j = nodes_count - pow; j < nodes_count; j++)
            {
                const Node prev_node = nodes[j];
                nodes.push_back ({prev_node.x - dist, prev_node.y + 1, ""});
                edges.push_back ({j, isize (nodes) - 1, graph::D});
                nodes.push_back ({prev_node.x + dist, prev_node.y + 1, ""});
                edges.push_back ({j, isize (nodes) - 1, graph::D});
            }
            pow *= 2;
        }

        std::uniform_real_distribution<double> dir_random (0, 2);
        std::uniform_real_distribution<double> node_random (0, isize (nodes));
        std::vector<Directional_Vehicle> dir_vehs;

        set_graph (graph_initial, nodes, edges, dir_vehs);
        for (int i = 0; i < 100; i++)
        {
            Directional_Vehicle v;

            graph::uid src = static_cast<graph::uid> (node_random (gen));
            const graph::uid dst = isize (nodes);
            if (src == dst)
                continue;

            src = 0;
            v.src = src;

            int cur_node = src;

            while (1)
            {
                const auto &edges = graph_initial->get_graph ()->edges_started_from (cur_node);
                if (edges.size () == 0)
                  break;

                const int next_edge_ind = static_cast<int> (dir_random (gen)) % edges.size ();
                v.path.push_back (edges[next_edge_ind]);
                cur_node = graph_initial->get_graph ()->edge (edges[next_edge_ind]).end;
            }

            if (v.path.size () <= 1)
              continue;

            v.dst = cur_node;
            v.t = i;

            dir_vehs.push_back (v);
        }

        set_graph (graph_initial, nodes, edges, dir_vehs);
    }

    void set_default_graph_4 (graph::graph_initial *graph_initial)
    {
        std::mt19937 gen (0);
        std::uniform_real_distribution<double> node_height (graph::D, 25 * graph::D);
        const int depth = 4;
        std::vector<Node> nodes = {{0., 0., "A"}};
        std::vector<Edge> edges;

        auto get_length = [&node_height, &gen] () { return node_height (gen); };

        const int ddepth = rombe_depth;

        int pow = 1;
        for (int i = 1; i < ddepth; i++)
        {
            const double dist = 1. / (pow * 2);
            const int nodes_count = isize (nodes);
            for (int j = nodes_count - pow; j < nodes_count; j++)
            {
                const Node prev_node = nodes[j];
                nodes.push_back ({prev_node.x - dist, prev_node.y + 1, ""});
                edges.push_back ({j, isize (nodes) - 1, get_length ()});
                nodes.push_back ({prev_node.x + dist, prev_node.y + 1, ""});
                edges.push_back ({j, isize (nodes) - 1, get_length ()});
            }
            pow *= 2;
        }

        for (int i = 1; i < ddepth; i++)
        {
            const int nodes_count = isize (nodes);
            for (int j = nodes_count - pow; j < nodes_count; j += 2)
            {
                const Node prev_node = nodes[j];
                const Node prev_node_next = nodes[j + 1];
                nodes.push_back ({(prev_node.x + prev_node_next.x) / 2., prev_node.y + 1, ""});

                Node node = nodes.back ();
                edges.push_back ({j, isize (nodes) - 1, get_length ()});
                edges.push_back ({j + 1, isize (nodes) - 1, get_length ()});
            }
            pow /= 2;
        }

        nodes.back ().name = "B";

        std::uniform_real_distribution<double> dir_random (0, 2);
        std::uniform_real_distribution<double> node_random (0, isize (nodes));
        std::vector<Directional_Vehicle> dir_vehs;

        set_graph (graph_initial, nodes, edges, dir_vehs);
        for (int i = 0; i < veh_on_rombe; i++)
        {
            Directional_Vehicle v;

            graph::uid src = static_cast<graph::uid> (node_random (gen));
            const graph::uid dst = isize (nodes);
            if (src == dst)
                continue;

            src = 0;
            v.src = src;

            int cur_node = src;


//            if (i == 0 || i == veh_on_rombe - 1 || i == veh_on_rombe / 2)
//            {
//                const auto &edges = graph_initial->get_graph ()->edges_started_from (cur_node);
//                v.path.push_back (edges[0]);
//                cur_node = graph_initial->get_graph ()->edge (edges[0]).end;
//            }
//            else
//            {
//                const auto &edges = graph_initial->get_graph ()->edges_started_from (cur_node);
//                v.path.push_back (edges[1]);
//                cur_node = graph_initial->get_graph ()->edge (edges[1]).end;
//            }


            while (1)
            {
                const auto &edges = graph_initial->get_graph ()->edges_started_from (cur_node);
                if (edges.size () == 0)
                    break;

                const int next_edge_ind = static_cast<int> (dir_random (gen)) % edges.size ();
                v.path.push_back (edges[next_edge_ind]);
                cur_node = graph_initial->get_graph ()->edge (edges[next_edge_ind]).end;
            }

            if (v.path.size () <= 1)
                continue;

            v.dst = cur_node;
            v.t = (double) i / 5;

            dir_vehs.push_back (v);
        }
//        Directional_Vehicle v;
//        v.src = 0;
//        v.dst = isize (nodes) - 1;
//        v.t = 40.;
//        v.path = {1, 4, 10, 22, 46, 78, 102, 114, 120, 123};

//        dir_vehs.push_back (v);

        set_graph (graph_initial, nodes, edges, dir_vehs);
    }

    void set_default_graph_5 (graph::graph_initial *graph_initial)
    {
        std::mt19937 gen (0);
        std::uniform_real_distribution<double> node_height (graph::D, 25 * graph::D);
        std::vector<Node> nodes = {{0., 0., "A"}};
        std::vector<Edge> edges;

        auto get_length = [&node_height, &gen] () { return node_height (gen); };

        const int ddepth = rombe_depth;

        int pow = 1;
        for (int i = 1; i < ddepth; i++)
        {
            const double dist = 1. / (pow * 2);
            const int nodes_count = isize (nodes);
            for (int j = nodes_count - pow; j < nodes_count; j++)
            {
                const Node prev_node = nodes[j];
                nodes.push_back ({prev_node.x - dist, prev_node.y + 1, ""});
                edges.push_back ({j, isize (nodes) - 1, get_length ()});
                nodes.push_back ({prev_node.x + dist, prev_node.y + 1, ""});
                edges.push_back ({j, isize (nodes) - 1, get_length ()});
            }
            pow *= 2;
        }

        for (int i = 1; i < ddepth; i++)
        {
            const int nodes_count = isize (nodes);
            for (int j = nodes_count - pow; j < nodes_count; j += 2)
            {
                const Node prev_node = nodes[j];
                const Node prev_node_next = nodes[j + 1];
                nodes.push_back ({(prev_node.x + prev_node_next.x) / 2., prev_node.y + 1, ""});

                Node node = nodes.back ();
                edges.push_back ({j, isize (nodes) - 1, get_length ()});
                edges.push_back ({j + 1, isize (nodes) - 1, get_length ()});
            }
            pow /= 2;
        }

        nodes.back ().name = "B";

        std::uniform_real_distribution<double> dir_random (0, 2);
        std::uniform_real_distribution<double> node_random (0, isize (nodes));
        std::vector<Directional_Vehicle> dir_vehs;

        edges.push_back ({isize (nodes) / 2, isize (nodes) / 2 - 1, 1.});
        edges.push_back ({isize (nodes) / 2 - 1, isize (nodes) / 2, 1.});

        set_graph (graph_initial, nodes, edges, dir_vehs);
        for (int i = 0; i < veh_on_rombe; i++)
        {
            Directional_Vehicle v;

            graph::uid src = static_cast<graph::uid> (node_random (gen));
            const graph::uid dst = isize (nodes);
            if (src == dst)
                continue;

            src = 0;
            v.src = src;

            int cur_node = src;

//            if (i == 0 || i == veh_on_rombe - 1 || i == veh_on_rombe / 2)
//            {
//                const auto &edges = graph_initial->get_graph ()->edges_started_from (cur_node);
//                v.path.push_back (edges[0]);
//                cur_node = graph_initial->get_graph ()->edge (edges[0]).end;
//            }
//            else
//            {
//                const auto &edges = graph_initial->get_graph ()->edges_started_from (cur_node);
//                v.path.push_back (edges[1]);
//                cur_node = graph_initial->get_graph ()->edge (edges[1]).end;
//            }

            while (1)
            {
                const auto &edges = graph_initial->get_graph ()->edges_started_from (cur_node);
                if (edges.size () == 0)
                    break;

                const int next_edge_ind = static_cast<int> (dir_random (gen)) % edges.size ();
                v.path.push_back (edges[next_edge_ind]);
                cur_node = graph_initial->get_graph ()->edge (edges[next_edge_ind]).end;
            }

            if (v.path.size () <= 1)
                continue;

            v.dst = cur_node;
            v.t = (double) i / 5;

            dir_vehs.push_back (v);
        }

        set_graph (graph_initial, nodes, edges, dir_vehs);
    }

    void set_default_graph_6 (graph::graph_initial *graph_initial)
    {
        std::mt19937 gen (0);
        std::uniform_real_distribution<double> node_height (graph::D, 25 * graph::D);
        std::vector<Node> nodes = {{0., 0., "A"}};
        std::vector<Edge> edges;

        auto get_length = [&node_height, &gen] () { return node_height (gen); };

        const int ddepth = rombe_depth;

        int pow = 1;
        for (int i = 1; i < ddepth; i++)
        {
            const double dist = 1. / (pow * 2);
            const int nodes_count = isize (nodes);
            for (int j = nodes_count - pow; j < nodes_count; j++)
            {
                const Node prev_node = nodes[j];
                nodes.push_back ({prev_node.x - dist, prev_node.y + 1, ""});
                edges.push_back ({j, isize (nodes) - 1, get_length ()});
                nodes.push_back ({prev_node.x + dist, prev_node.y + 1, ""});
                edges.push_back ({j, isize (nodes) - 1, get_length ()});
            }
            pow *= 2;
        }

        for (int i = 1; i < ddepth; i++)
        {
            const int nodes_count = isize (nodes);
            for (int j = nodes_count - pow; j < nodes_count; j += 2)
            {
                const Node prev_node = nodes[j];
                const Node prev_node_next = nodes[j + 1];
                nodes.push_back ({(prev_node.x + prev_node_next.x) / 2., prev_node.y + 1, ""});

                Node node = nodes.back ();
                edges.push_back ({j, isize (nodes) - 1, get_length ()});
                edges.push_back ({j + 1, isize (nodes) - 1, get_length ()});
            }
            pow /= 2;
        }

//        edges.push_back ({isize (nodes) / 2, isize (nodes) / 2 - 1, 1.});
//        edges.push_back ({isize (nodes) / 2 - 1, isize (nodes) / 2, 1.});

        pow = 1;
        for (int i = 1; i < ddepth; i++)
        {
            const double dist = 1. / (pow * 2);
            const int nodes_count = isize (nodes);
            for (int j = nodes_count - pow; j < nodes_count; j++)
            {
                const Node prev_node = nodes[j];
                nodes.push_back ({prev_node.x - dist, prev_node.y + 1, ""});
                edges.push_back ({j, isize (nodes) - 1, get_length ()});
                nodes.push_back ({prev_node.x + dist, prev_node.y + 1, ""});
                edges.push_back ({j, isize (nodes) - 1, get_length ()});
            }
            pow *= 2;
        }

        for (int i = 1; i < ddepth; i++)
        {
            const int nodes_count = isize (nodes);
            for (int j = nodes_count - pow; j < nodes_count; j += 2)
            {
                const Node prev_node = nodes[j];
                const Node prev_node_next = nodes[j + 1];
                nodes.push_back ({(prev_node.x + prev_node_next.x) / 2., prev_node.y + 1, ""});

                Node node = nodes.back ();
                edges.push_back ({j, isize (nodes) - 1, get_length ()});
                edges.push_back ({j + 1, isize (nodes) - 1, get_length ()});
            }
            pow /= 2;
        }


        nodes.back ().name = "B";

        std::uniform_real_distribution<double> dir_random (0, 2);
        std::uniform_real_distribution<double> node_random (0, isize (nodes));
        std::vector<Directional_Vehicle> dir_vehs;

//        edges.push_back ({isize (nodes) / 2, isize (nodes) / 2 - 1, 1.});
//        edges.push_back ({isize (nodes) / 2 - 1, isize (nodes) / 2, 1.});

        set_graph (graph_initial, nodes, edges, dir_vehs);
        for (int i = 0; i < veh_on_rombe; i++)
        {
            Directional_Vehicle v;

            graph::uid src = static_cast<graph::uid> (node_random (gen));
            const graph::uid dst = isize (nodes);
            if (src == dst)
                continue;

            src = 0;
            v.src = src;

            int cur_node = src;

            //            if (i == 0 || i == veh_on_rombe - 1 || i == veh_on_rombe / 2)
            //            {
            //                const auto &edges = graph_initial->get_graph ()->edges_started_from (cur_node);
            //                v.path.push_back (edges[0]);
            //                cur_node = graph_initial->get_graph ()->edge (edges[0]).end;
            //            }
            //            else
            //            {
            //                const auto &edges = graph_initial->get_graph ()->edges_started_from (cur_node);
            //                v.path.push_back (edges[1]);
            //                cur_node = graph_initial->get_graph ()->edge (edges[1]).end;
            //            }

            while (1)
            {
                const auto &edges = graph_initial->get_graph ()->edges_started_from (cur_node);
                if (edges.size () == 0)
                    break;

                const int next_edge_ind = static_cast<int> (dir_random (gen)) % edges.size ();
                v.path.push_back (edges[next_edge_ind]);
                cur_node = graph_initial->get_graph ()->edge (edges[next_edge_ind]).end;
            }

            if (v.path.size () <= 1)
                continue;

            v.dst = cur_node;
            v.t = (double) i / 5;

            dir_vehs.push_back (v);
        }

        set_graph (graph_initial, nodes, edges, dir_vehs);
    }
}

#endif // DEFAULT_GRAPHS_H
