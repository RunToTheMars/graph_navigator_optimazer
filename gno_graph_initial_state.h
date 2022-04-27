#ifndef GNO_GRAPH_INITIAL_STATE_H
#define GNO_GRAPH_INITIAL_STATE_H

#include "gno_graph_fwd.h"
#include <vector>

namespace graph
{

struct Directional_Vehicle
{
    Vehicle vehicle;

    //start time
    double t = 0.;

    //node uids
    graph::uid src;
    graph::uid dst;

    //edge uids
    std::vector<graph::uid> path;
};

class graph_initial_state_base
{
public:
    virtual ~graph_initial_state_base () = default;

    virtual graph::uid vehicle_count () const = 0;

    virtual Directional_Vehicle &vehicle (const uid vehicle_uid) = 0;
    virtual const Directional_Vehicle &vehicle (const uid vehicle_uid) const = 0;

    virtual void add_vehicle (const Directional_Vehicle &directional_vehicle) = 0;
    virtual void remove_vehicle (const uid vehicle_uid) = 0;

    virtual void clear () = 0;

    virtual bool is_correct_vehicle_uid (uid vehicle_uid)
    {
        return vehicle_uid != invalid_uid && vehicle_uid < vehicle_count ();
    }
};

class graph_initial_state_impl : public graph_initial_state_base
{
    using parent_t = graph_initial_state_base;

public:
    virtual ~graph_initial_state_impl () = default;

    virtual uid vehicle_count () const override
    {
        return static_cast<uid> (m_directional_vehicles.size ());
    }

    virtual Directional_Vehicle &vehicle (const uid vehicle_uid) override
    {
        return m_directional_vehicles[vehicle_uid];
    }

    virtual const Directional_Vehicle &vehicle (const uid vehicle_uid) const override
    {
        return m_directional_vehicles[vehicle_uid];
    }

    virtual void add_vehicle (const Directional_Vehicle &directional_vehicle) override
    {
        return m_directional_vehicles.push_back (directional_vehicle);
    }

    virtual void remove_vehicle (const uid vehicle_uid) override
    {
        m_directional_vehicles.erase (m_directional_vehicles.begin () + vehicle_uid);
    }

    virtual void clear () override
    {
        m_directional_vehicles.clear ();
    }

private:
    std::vector<Directional_Vehicle> m_directional_vehicles;
};
}

#endif // GNO_GRAPH_INITIAL_STATE_H
