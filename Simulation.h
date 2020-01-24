#ifndef ZERG_SIMULATION_H
#define ZERG_SIMULATION_H
#include <unordered_map>
#include <vector>
#include <string>

#include <json/single_include/nlohmann/json.hpp>

#include "EntityData.h"
#include "EventQueue.h"
#include "Entity.h"

using std::unordered_map;
using std::vector;
using std::string;
using nlohmann::json;

/*
* Simulation class, manages the simulation
*/
class Simulation {
public:
    Simulation(const vector<string>& bl,
        const unordered_map<string, EntityData>& edm);

    /*
    * Run the simulation and return the json result
    */
    json run();
private:
  class SimulationState {
  public:
    unsigned int _time_step;

    double _minerals;
    double _vespene;
    double _supply;
    double _supply_used;

    EventQueue _event_queue;
    /*
    * entity name to entity list mapping
    */
    unordered_map<string, vector<Entity> > _name_to_entities;

    /*
    * eneity id to entity mapping.
    *
    * Pointers are to the objects in _name_to_entities map
    */
    unordered_map<string, Entity*> _id_to_entity;


    /*
    * counts
    */
    unsigned int _drones_assigned_to_mineral;
    unsigned int _drones_assigned_to_vespene;

    vector<string> _build_list;


    /*
    * put names of the discovered entities here
    */
    unordered_set<string> _discovered;



    const unordered_map<string, EntityData>& _entity_data_map;


    SimulationState(double m, double v, double s, double su,
      const vector<string>& build_list,
      const unordered_map<string, EntityData>& edm);

    /*
    * Add entity to state
    *
    * in_entity is only for larvas. it denotes the hatchery/lair/hive it is in.
    *
    *
    */
    void addEntity(const Entity& entity, const string& in_entity);

    /*
    * Remove entity from the state
    *
    * in_entity is only for larvas. it denotes the hatchery/lair/hive it is in.
    */
    void removeEntity(Entity entity, const string& in_entity);

  };



  /*
  * Returns true if the build List is buildable,
  * returns false otherwise
  */
  bool buildable() const;

  /*
  * Run 1 iteration
  */
  json tictoc(SimulationState& state);

  /*
  * contains the build list
  */
  vector<string> _build_list;

  /*
  * constains the entity data map
  */
  const unordered_map<string, EntityData>& _entity_data_map;
};

#endif
