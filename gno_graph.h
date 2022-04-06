#ifndef GNO_GRAPH_H
#define GNO_GRAPH_H

#include <functional>
#include <vector>
#include <math.h>

#include "gno_graph_fwd.h"
#include <string>

namespace graph
{
template <typename Node = Node_Base, typename Edge = Edge_Base>
class graph_base
{
    static_assert (std::is_base_of<Node_Base, Node>::value,
                  "Node must derive from Node_Base");
    static_assert (std::is_base_of<Edge_Base, Edge>::value,
                  "Edge must derive from Edge_Base");

public:
    using self_node = Node;
    using self_edge = Edge;

    virtual ~graph_base () = default;

    virtual graph::uid node_count () const = 0;
    virtual graph::uid edge_count () const = 0;

    virtual self_node &node (const uid node_uid) = 0;
    virtual self_edge &edge (const uid edge_uid) = 0;

    virtual const self_node &node (const uid node_uid) const = 0;
    virtual const self_edge &edge (const uid edge_uid) const = 0;

    virtual const std::vector<graph::uid> &edges_started_from (const uid node_uid) const = 0;
    virtual const std::vector<graph::uid> &edges_ended_on (const uid node_uid) const = 0;

    virtual void add_node (const self_node &node) = 0;
    virtual void remove_node (const uid node_uid)
    {
        remove_node_impl (node_uid);
        for (uid edge_uid = 0; edge_uid < static_cast<int> (edge_count ()); edge_uid ++)
        {
            const self_edge &e = edge (edge_uid);
            if (e.start == node_uid || e.end == node_uid)
            {
                remove_edge (edge_uid);
            }

        }
    }

    virtual void add_edge (const self_edge &edge) = 0;
    virtual void remove_edge (const uid edge_uid) = 0;

    virtual double length (const uid edge_uid) const = 0;

    virtual void calculate_bounds () = 0;

    virtual double min_x () const = 0;
    virtual double max_x () const = 0;
    virtual double min_y () const = 0;
    virtual double max_y () const = 0;

    virtual void clear () = 0;

    virtual bool is_correct_node_uid (uid node_uid)
    {
        return node_uid != invalid_uid && node_uid < node_count ();
    }

private:
    virtual void remove_node_impl (const uid node_uid) = 0;

};

template <typename Node = Node_Base, typename Edge = Edge_Base>
class graph_impl : public graph_base<Node, Edge>
{
    using parent_t = graph_base<Node_Base, Edge_Base>;

public:
    virtual ~graph_impl () = default;

    virtual graph::uid node_count () const override { return static_cast<graph::uid> (m_nodes.size ()); }
    virtual graph::uid edge_count () const override { return static_cast<graph::uid> (m_edges.size ()); }

    virtual Node &node (const uid node_uid) override { return m_nodes[node_uid]; }
    virtual Edge &edge (const uid edge_uid) override { return m_edges[edge_uid]; }

    virtual const Node &node (const uid node_uid) const override { return m_nodes[node_uid]; }
    virtual const Edge &edge (const uid edge_uid) const override { return m_edges[edge_uid]; }

    virtual const std::vector<graph::uid> &edges_started_from (const uid node_uid) const { return m_start_map[node_uid]; }
    virtual const std::vector<graph::uid> &edges_ended_on   (const uid node_uid) const { return m_end_map[node_uid]; }

    virtual void add_node (const Node &node) override
    {
        m_nodes.push_back (node);

        // todo: do 1 time on create
        rebuild_maps ();
    }

    virtual void add_edge (const Edge &edge) override
    {
        m_edges.push_back (edge);

        //todo: do 1 time on create
        rebuild_maps ();
    }

    virtual void remove_edge (const uid edge_uid) override
    {
        m_edges.erase (m_edges.begin () + edge_uid);

        //todo: do 1 time on create
        rebuild_maps ();
    }

    virtual double length (const uid edge_uid) const override
    {
        const Edge &e = edge (edge_uid);
        if (!m_use_geometry_length)
        {
            return e.length;
        }
        else
        {
            const Node &n1 = node (e.start);
            const Node &n2 = node (e.start);

            double dx = n1.x - n2.x;
            double dy = n1.y - n2.y;

            return sqrt (dx * dx + dy * dy);
        }
    }

    void set_use_geometry_length (bool use) { m_use_geometry_length = use; }

    virtual void calculate_bounds () override
    {
        bool count = false;

        clear_bounds ();

        for (graph::uid i = 0; i < node_count (); i++)
        {
            const Node &n = node (i);
            if (count)
            {
                if (n.x < m_min_x)
                    m_min_x = n.x;
                if (m_max_x < n.x)
                    m_max_x = n.x;

                if (n.y < m_min_y)
                    m_min_y = n.y;
                if (m_max_y < n.y)
                    m_max_y = n.y;
            }
            else
            {
                m_min_x = m_max_x = n.x;
                m_min_y = m_max_y = n.y;
                count = true;
            }
        }
    }

    virtual double min_x () const override { return m_min_x; }
    virtual double max_x () const override { return m_max_x; }
    virtual double min_y () const override { return m_min_y; }
    virtual double max_y () const override { return m_max_y; }

    virtual void clear () override
    {
        m_nodes.clear ();
        m_edges.clear ();

        clear_bounds ();
    }


private:
    virtual void rebuild_maps ()
    {
        m_start_map.clear ();
        m_end_map.clear ();

        m_start_map.resize (node_count ());
        m_end_map.resize (node_count ());

        for (graph::uid i = 0; i < edge_count (); i++)
        {
            const Edge &e = edge (i);

            m_start_map[e.start].push_back (i);
            m_end_map[e.end].push_back (i);
        }
    }
    virtual void remove_node_impl (const uid node_uid) override
    {
        m_nodes.erase (m_nodes.begin () + node_uid);

        //todo: do 1 time on create
        rebuild_maps ();
    }
    virtual void clear_bounds ()
    {

        m_min_x = 0.;
        m_max_x = 1.;
        m_min_y = 0.;
        m_max_y = 1.;
    }

private:
    bool m_use_geometry_length = false;

    std::vector<Node> m_nodes;
    std::vector<Edge> m_edges;

    //node uid to edge uid maps
    std::vector<std::vector<graph::uid>> m_start_map;
    std::vector<std::vector<graph::uid>> m_end_map;

    double m_min_x = 0.;
    double m_max_x = 1.;
    double m_min_y = 0.;
    double m_max_y = 1.;
};
} // namespace graph

#endif // GNO_GRAPH_H
