#ifndef GNO_GRAPH_INITIAL_STATE_H
#define GNO_GRAPH_INITIAL_STATE_H

#include "gno_graph_fwd.h"
#include <vector>

namespace graph
{

template <typename Vehicle = Vehicle_Base>
struct Directional_Vehicle
{
    Vehicle vehicle;
    std::vector<graph::uid> path;
};

template <typename Vehicle = Vehicle_Base>
class graph_initial_state_base
{
public:
    using dir_veh = Directional_Vehicle<Vehicle>;

    virtual ~graph_initial_state_base () = default;

    virtual graph::uid vehicle_count () const = 0;

    virtual Vehicle &vehicle (const uid vehicle_uid) = 0;
    virtual const Vehicle &vehicle (const uid vehicle_uid) const = 0;

    virtual std::vector<graph::uid> &path (const uid vehicle_uid) = 0;
    virtual const std::vector<graph::uid> &path (const uid vehicle_uid) const = 0;

    virtual void add_vehicle (const dir_veh &directional_vehicle) = 0;
    virtual void remove_vehicle (const uid vehicle_uid) = 0;

    virtual void clear () = 0;

    virtual bool is_correct_vehicle_uid (uid vehicle_uid)
    {
        return vehicle_uid != invalid_uid && vehicle_uid < vehicle_count ();
    }
};

template <typename Vehicle = Vehicle_Base>
class graph_initial_state_impl : public graph_initial_state_base<Vehicle>
{
    using parent_t = graph_initial_state_base<Vehicle>;

public:
    virtual ~graph_initial_state_impl () = default;

    virtual uid vehicle_count () const override
    {
      return m_directional_vehicles.size ();
    }

    virtual Vehicle &vehicle (const uid vehicle_uid) override
    {
        return m_directional_vehicles[vehicle_uid].vehicle;
    }

    virtual const Vehicle &vehicle (const uid vehicle_uid) const override
    {
        return m_directional_vehicles[vehicle_uid].vehicle;
    }

    virtual std::vector<uid> &path (const uid vehicle_uid) override
    {
        return m_directional_vehicles[vehicle_uid].path;
    }

    virtual const std::vector<uid> &path (const uid vehicle_uid) const override
    {
        return m_directional_vehicles[vehicle_uid].path;
    }

    virtual void add_vehicle (const Directional_Vehicle<Vehicle> &directional_vehicle) override
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
    std::vector<Directional_Vehicle<Vehicle>> m_directional_vehicles;
};
}

#endif // GNO_GRAPH_INITIAL_STATE_H
