#include "Simulation.h"
#include <unordered_set>
#include <iostream>
#include "utility.h"
#include <iterator>
#include <algorithm>


using std::unordered_map;
using std::vector;
using std::string;
using nlohmann::json;
using std::unordered_set;
using std::cout;
using std::endl;
using std::prev;
using std::max;
using std::min;


void Simulation::SimulationState::addEntity(const Entity& entity, const string& in_entity = string()) {
	if (entity._name == "Larva") {
		// add it to hatchery with id in_entity.
		_id_to_entity[in_entity]->_larvas.push_back(entity);
		_id_to_entity[entity._id] = &(*prev(_id_to_entity[in_entity]->_larvas.end()));
	}
	else {
		_name_to_entities[entity._name].push_back(entity);
		_id_to_entity[entity._id] = &(*prev(_name_to_entities[entity._name].end()));
		if (entity._name == "Drone") {
			_drones_assigned_to_mineral++;
		}
	}

	_discovered.insert(entity._name);
}

void Simulation::SimulationState::removeEntity(Entity entity, const string& in_entity = string()) {
	if (entity._name == "Larva") {
		// add it to hatchery with id in_entity.
		_id_to_entity.erase(entity._id);
		for (auto it = _id_to_entity[in_entity]->_larvas.begin();
			it != _id_to_entity[in_entity]->_larvas.end(); it++) {
			if (*it == entity) {
				_id_to_entity[in_entity]->_larvas.erase(it);
				break;
			}
		}
		for (auto it = _id_to_entity[in_entity]->_queen_larvas.begin();
			it != _id_to_entity[in_entity]->_queen_larvas.end(); it++) {
			if (*it == entity) {
				_id_to_entity[in_entity]->_queen_larvas.erase(it);
				break;
			}
		}
	}
	else {
		if (entity._name == "Drone") {
			if (_drones_assigned_to_mineral > 0)
				_drones_assigned_to_mineral--;
			else
				_drones_assigned_to_vespene--;
		}
		_id_to_entity.erase(entity._id);
		for (auto it = _name_to_entities[entity._name].begin();
			it != _name_to_entities[entity._name].end(); it++) {
			if (*it == entity) {
				_name_to_entities[entity._name].erase(it);
				break;
			}
		}
	}

}

Simulation::SimulationState::SimulationState(double m, double v, double s,
	double su, const vector<string>& build_list,
	const unordered_map<string, EntityData>& edm) : _minerals(m), _vespene(v),
	_supply(s), _supply_used(su), _build_list(build_list), _time_step(0),
	_entity_data_map(edm), _drones_assigned_to_mineral(0), _drones_assigned_to_vespene(0) {

}

Simulation::Simulation(const vector<string>& bl,
	const unordered_map<string, EntityData>& edm) : _build_list(bl),
	_entity_data_map(edm) {

}

bool Simulation::buildable() const {
	/*
	* Add entity names to this set if they are discovered before
	*/
	unordered_set<string> discovered;
	discovered.insert(string("Drone"));
	discovered.insert(string("Hatchery"));
	discovered.insert(string("Overlord"));
	discovered.insert(string("Larva"));

	/*
	* Store counts of entities in this map
	*/
	unordered_map<string, unsigned int> counts;

	counts["Drone"] = 12U;
	counts["Hatchery"] = 1U;
	counts["Overlord"] = 1U;
	counts["Larva"] = 3U;

	bool extractor_built = false;
	double current_supply =
		(_entity_data_map.at("Drone")._supply_provided
			- _entity_data_map.at("Drone")._supply_cost) * 12 +
			(_entity_data_map.at("Hatchery")._supply_provided
				- _entity_data_map.at("Hatchery")._supply_cost) * 1 +
				(_entity_data_map.at("Overlord")._supply_provided
					- _entity_data_map.at("Overlord")._supply_cost) * 1 +
					(_entity_data_map.at("Larva")._supply_provided
						- _entity_data_map.at("Larva")._supply_cost) * 3;

	for (const string& entity_name : _build_list) {
		const EntityData& entity_data = _entity_data_map.at(entity_name);

		/*
		* If entity needs vespene to built, and if there is no extractor yet
		* we cant have any vespene to build this
		*/
		if (entity_data._vespene != 0.0 && !extractor_built) {
			return false;
		}

		// we can have maximum 2 extractors
		if (entity_name == "Extractor" && counts["Extractor"] == 2U) {
			return false;
		}

		/*
		* If there is not enough supply, you can't build this
		*/
		//if (double_less_with_epsilon(current_supply, entity_data._supply_cost)) {
		if(current_supply < entity_data._supply_cost) {
			return false;
		}

		// if entity is discovered before, mark it as discovered
		bool dependency_discovered = entity_data._dependency.empty()
			|| discovered.find(entity_name) != discovered.end();

		/*
		* Check if any of the dependencies are discovered
		*/
		for (const string& dep : entity_data._dependency) {
			if (discovered.find(dep) != discovered.end()) {
				dependency_discovered = true;
				break;
			}
		}
		if (!dependency_discovered) {
			return false;
		}


		bool producable = false;

		/*
		* Check producability
		*/
		for (const string& producer : entity_data._produced_by) {
			if (entity_name == "Larva") {
				// larva is always producable
				producable = true;
				break;
			}
			else if (producer == "Larva") {
				// larva will become available at some point
				producable = true;
				break;
			}
			else {
				if (entity_name == "Queen" || entity_name == "NydusCanal") {
					// these entities are produced by something (not morphed)
					if (counts[producer] > 0) {
						producable = true;
						break;
					}
				}
				else {
					// these entities are morphed from something
					if (counts[producer] > 0) {
						producable = true;
						counts[producer]--; // decrease producer count as it is morphed
						// give back the supply used by the producer
						current_supply += _entity_data_map.at(producer)._supply_cost;
						// take the supply provided by the produer
						current_supply -= _entity_data_map.at(producer)._supply_provided;
						break;
					}
				}
			}
		}

		if (!producable) {
			return false;
		}

		if (entity_name == "Extractor")
			extractor_built = true;
		/*
		* Update the supply
		*/
		current_supply += entity_data._supply_provided;
		current_supply -= entity_data._supply_cost;
		discovered.insert(entity_name);
		counts[entity_name]++;
		if (entity_name == "Zergling")
			counts[entity_name]++;
	}

	return true;
}

json Simulation::run() {
	json log;
	log["game"] = "Zerg";

	if (buildable()) {
		log["buildlistValid"] = "1";

		/*
		* Initialize the structures
		*/
		double initial_supply =
			_entity_data_map.at("Drone")._supply_provided * 12 +
			_entity_data_map.at("Hatchery")._supply_provided +
			_entity_data_map.at("Overlord")._supply_provided +
			_entity_data_map.at("Larva")._supply_provided * 3;

		double initial_supply_used =
			_entity_data_map.at("Drone")._supply_cost * 12
			+ _entity_data_map.at("Hatchery")._supply_cost
			+ _entity_data_map.at("Overlord")._supply_cost
			+ _entity_data_map.at("Larva")._supply_cost * 3;

		SimulationState state(50, 0, initial_supply, initial_supply_used,
			_build_list, _entity_data_map);
		for (int i = 0; i < 12; i++) {
			Entity drone("Drone");
			state.addEntity(drone);
			log["initialUnits"]["Drone"].push_back(drone._id);
		}

		Entity hatchery("Hatchery");
		state.addEntity(hatchery);
		log["initialUnits"]["Hatchery"].push_back(hatchery._id);

		Entity overlord("Overlord");
		state.addEntity(overlord);
		log["initialUnits"]["Overlord"].push_back(overlord._id);

		for (int i = 0; i < 3; i++) {
			Entity larva("Larva");
			state.addEntity(larva, hatchery._id);
			log["initialUnits"]["Larva"].push_back(larva._id);
		}

		// if there is only special action events in the queue
		// and there is nothing to build
		// just end it.
		// if not continue
		while (state._event_queue.total_event_count != state._event_queue.special_action_event_count || !state._build_list.empty()) {
			json cycle_log = tictoc(state);
			if (cycle_log != json()) {
				log["messages"].push_back(cycle_log);
			}

			if (state._time_step == 1000) {
				// it seems we couldnt catch this case in buildable function
				// this is only for last resort!
				cout << log << endl;
				json res;
				res["game"] = "Zerg";
				res["buildlistValid"] = "0";
				return res;
			}
		}

	}
	else {
		log["buildlistValid"] = "0";
	}

	return log;
}

json Simulation::tictoc(SimulationState& state) {

	static const vector<string> larva_queen_producers{ "Hatchery", "Hive", "Lair" };

	// increase time step
	state._time_step++;

	json cycle_log;
	cycle_log["time"] = state._time_step;

	/*
	* Update resources
	*/
	state._minerals += state._drones_assigned_to_mineral * 0.7;
	state._vespene += state._drones_assigned_to_vespene * 0.35;

	/*
	* Increase larva counters, produce larva
	* It seems we do not report larva production in the log
	*/
	for (Entity& hatchery : state._name_to_entities["Hatchery"]) {
		if (hatchery._larvas.size() < 3) {
			hatchery._time_to_next_larva--;
			if (hatchery._time_to_next_larva == 0) {
				hatchery._time_to_next_larva = 15;
				Entity larva("Larva");
				state.addEntity(larva, hatchery._id);
			}
		}
	}
	for (Entity& hive : state._name_to_entities["Hive"]) {
		if (hive._larvas.size() < 3) {
			hive._time_to_next_larva--;
			if (hive._time_to_next_larva == 0) {
				hive._time_to_next_larva = 15;
				Entity larva("Larva");
				state.addEntity(larva, hive._id);
			}
		}
	}
	for (Entity& lair : state._name_to_entities["Lair"]) {
		if (lair._larvas.size() < 3) {
			lair._time_to_next_larva--;
			if (lair._time_to_next_larva == 0) {
				lair._time_to_next_larva = 15;
				Entity larva("Larva");
				state.addEntity(larva, lair._id);
			}
		}
	}

	/*
	* Increase queen energies by 0.5625
	*/
	for (Entity& queen : state._name_to_entities["Queen"]) {
		if (queen._energy < 200)
			queen._energy += 0.5625;
		else
			queen._energy = 200;
	}

	/*
	* check finished events
	*/
	bool build_ended = false;
	while (!state._event_queue.isEmpty() && state._event_queue.getMinFinishTime() == state._time_step) {
		Event* event = state._event_queue.getNextEvent();


		if (event->_event_type == Event::EventType::BUILD) {
			build_ended = true;
			BuildEvent& build = *((BuildEvent*)event);
			const string& entity_name = build._entity_name;
			const string& producer_id = build._producer_id;
			json build_log;
			build_log["type"] = "build-end";
			build_log["name"] = entity_name;
			build_log["producerID"] = producer_id;
			if (entity_name == "Zergling") {
				// if entity is zergling, produce two of them.
				Entity z1("Zergling");
				Entity z2("Zergling");
				state.addEntity(z1);
				state.addEntity(z2);
				build_log["producedIDs"].push_back(z1._id);
				build_log["producedIDs"].push_back(z2._id);
			}
			else if (entity_name == "Hatchery"
				|| entity_name == "BroodLord" || entity_name == "Hatchery") {
				// these provide supply when they get built
				Entity entity(entity_name);
				state.addEntity(entity);
				build_log["producedIDs"].push_back(entity._id);
				state._supply += _entity_data_map.at(entity_name)._supply_provided;
			}
			else if (entity_name == "Hive" || entity_name == "Lair") {
				// these can get carryover larvas from their previous forms
				Entity entity(entity_name);
				state.addEntity(entity);
				for (auto& larva : build.larva_carryover) {
					state.addEntity(larva, entity._id);
				}
				for (auto& larva : build.queen_larva_carryover) {
					Entity* larva_producer = state._id_to_entity[entity._id];
					larva_producer->_queen_larvas.push_back(larva);
					state._id_to_entity[larva._id] = &(*prev(larva_producer->_queen_larvas.end()));
				}
				build_log["producedIDs"].push_back(entity._id);
			}
			else if (entity_name == "Queen") {
				Entity entity(entity_name);
				state.addEntity(entity);
				build_log["producedIDs"].push_back(entity._id);
				state._id_to_entity[producer_id]->_producing_queen = false;
			}
			else if (entity_name == "NydusCanal") {
				Entity entity(entity_name);
				state.addEntity(entity);
				build_log["producedIDs"].push_back(entity._id);
				state._id_to_entity[producer_id]->_producing_nydus_worm = false;
			}
			else {
				Entity entity(entity_name);
				state.addEntity(entity); //if it is drone, addEntity assigns it to minerals, but it will change anyway.
				build_log["producedIDs"].push_back(entity._id);
			}
			cycle_log["events"].push_back(build_log);
		}
		else if (event->_event_type == Event::EventType::SPECIAL_ACTION) {
			SpecialActionEvent& SpecAcEvent = *((SpecialActionEvent*)event);
			const string& triggered_by = SpecAcEvent._triggered_by;
			const string& target_building = SpecAcEvent._target_building;
			Entity* larva_producer = state._id_to_entity[target_building];
			larva_producer->_queen_constructing_in = false;
			for (int i = 0; i < 4; i++) {
				Entity larva("Larva");
				larva_producer->_queen_larvas.push_back(larva);
				state._id_to_entity[larva._id] = &(*prev(larva_producer->_queen_larvas.end()));
			}
		}
		state._event_queue.deleteNextEvent();
	}


	/*
	* select an action to perform
	*/
	bool action_done = false;
	if (!state._build_list.empty()) {
		const string& next_build_entity = state._build_list[0];
		const auto& entity_data = _entity_data_map.at(next_build_entity);
		bool dependency_discovered = entity_data._dependency.empty()
			|| state._discovered.find(next_build_entity) != state._discovered.end();

		for (const string& dep : entity_data._dependency) {
			if (state._discovered.find(dep) != state._discovered.end()) {
				dependency_discovered = true;
				break;
			}
		}

		if (dependency_discovered) {
			// check if resources sufficient
			if (double_less_with_epsilon(entity_data._minerals, state._minerals) &&
				double_less_with_epsilon(entity_data._vespene, state._vespene) &&
				double_less_with_epsilon(entity_data._supply_cost, state._supply - state._supply_used)) {
				if (next_build_entity == "Queen") {
					//these are produced

					// find a queen producer for this guy
					for (const string& container_name : larva_queen_producers) {
						bool queen_produced = false;
						for (Entity& producer : state._name_to_entities[container_name]) {
							if (!producer._producing_queen) {
								queen_produced = true;
								action_done = true;
								// voila!
								state._minerals -= entity_data._minerals;
								state._vespene -= entity_data._vespene;
								state._supply_used += entity_data._supply_cost;

								producer._producing_queen = true;

								json build_log;
								build_log["type"] = "build-start";
								build_log["name"] = next_build_entity;
								build_log["producerID"] = producer._id;
								cycle_log["events"].push_back(build_log);

								state._event_queue.addBuildEvent(next_build_entity, producer._id, state._time_step + entity_data._build_time);
								break;
							}
						}
						if (queen_produced)
							break;
					}
				}
				else if (next_build_entity == "NydusCanal") {
					for (Entity& producer : state._name_to_entities["NydusNetwork"]) {
						if (!producer._producing_nydus_worm) {
							action_done = true;
							// voila!
							state._minerals -= entity_data._minerals;
							state._vespene -= entity_data._vespene;
							state._supply_used += entity_data._supply_cost;

							producer._producing_nydus_worm = true;

							json build_log;
							build_log["type"] = "build-start";
							build_log["name"] = next_build_entity;
							build_log["producerID"] = producer._id;
							cycle_log["events"].push_back(build_log);

							state._event_queue.addBuildEvent(next_build_entity, producer._id, state._time_step + entity_data._build_time);
							break;
						}
					}
				}
				else if (next_build_entity == "Hive" || next_build_entity == "Lair") {
					// there will be a larva carryoverconst string& produced_by = entity_data._produced_by[0];
					const string& produced_by = entity_data._produced_by[0];
					for (int i = 0; i < state._name_to_entities[produced_by].size(); i++) {
						if (state._name_to_entities[produced_by][i]._queen_constructing_in ||
							state._name_to_entities[produced_by][i]._producing_queen)
							continue;
						// voila!
						action_done = true;
						state._minerals -= entity_data._minerals;
						state._vespene -= entity_data._vespene;
						state._supply_used += entity_data._supply_cost;

						const Entity& producer = state._name_to_entities[produced_by][0];
						json build_log;
						build_log["type"] = "build-start";
						build_log["name"] = next_build_entity;
						build_log["producerID"] = producer._id;
						cycle_log["events"].push_back(build_log);

						state._event_queue.addBuildEvent(next_build_entity, producer._id,
							state._time_step + entity_data._build_time,
							producer._larvas, producer._queen_larvas);
						for (auto& l : producer._larvas) {
							state.removeEntity(l, producer._id);
						}
						for (auto& l : producer._queen_larvas) {
							state.removeEntity(l, producer._id);
						}
						state.removeEntity(producer);
					}
				}
				else if (entity_data._produced_by[0] == "Larva") {
					//these are morphed from a larva

					double supcost = entity_data._supply_cost;
					// this is only for supply. others seem ok
					if (next_build_entity == "Zergling")
						supcost += entity_data._supply_cost;

					// find a larva for this guy
					for (const string& container_name : larva_queen_producers) {
						bool larva_found = false;
						for (Entity& entity : state._name_to_entities[container_name]) {
							if (!entity._larvas.empty()) {
								larva_found = true;
								action_done = true;
								// voila!
								state._minerals -= entity_data._minerals;
								state._vespene -= entity_data._vespene;
								state._supply_used += supcost;


								const Entity& producer = entity._larvas[0];

								json build_log;
								build_log["type"] = "build-start";
								build_log["name"] = next_build_entity;
								build_log["producerID"] = producer._id;
								cycle_log["events"].push_back(build_log);

								state._event_queue.addBuildEvent(next_build_entity, producer._id, state._time_step + entity_data._build_time);
								state.removeEntity(producer, entity._id);
								break;
							}
							else if (!entity._queen_larvas.empty()) {
								larva_found = true;
								action_done = true;
								// voila!
								state._minerals -= entity_data._minerals;
								state._vespene -= entity_data._vespene;
								state._supply_used += supcost;


								const Entity& producer = entity._queen_larvas[0];

								json build_log;
								build_log["type"] = "build-start";
								build_log["name"] = next_build_entity;
								build_log["producerID"] = producer._id;
								cycle_log["events"].push_back(build_log);

								state._event_queue.addBuildEvent(next_build_entity, producer._id, state._time_step + entity_data._build_time);
								state.removeEntity(producer, entity._id);
								break;
							}
						}
						if (larva_found)
							break;
					}

				}
				else {
					//these are morphed from something (including greater spire)

					// it is for sure that there will be 1 producer
					// (queen has 3, larva has 3, others have 1)
					const string& produced_by = entity_data._produced_by[0];
					if (!state._name_to_entities[produced_by].empty()) {
						// voila!
						action_done = true;
						state._minerals -= entity_data._minerals;
						state._vespene -= entity_data._vespene;
						state._supply_used += entity_data._supply_cost;

						const Entity& producer = state._name_to_entities[produced_by][0];
						if (produced_by == "Drone") {
							state._supply_used--;
						}
						json build_log;
						build_log["type"] = "build-start";
						build_log["name"] = next_build_entity;
						build_log["producerID"] = producer._id;
						cycle_log["events"].push_back(build_log);

						state._event_queue.addBuildEvent(next_build_entity, producer._id, state._time_step + entity_data._build_time);
						state.removeEntity(producer);
					}
				}
			}
		}
	}
	if (action_done)
		state._build_list.erase(state._build_list.begin());

	if (!action_done) {
		//try special action
		for (auto& queen : state._name_to_entities["Queen"]) {
			bool injected_larvae = false;
			if (queen._energy >= 25) {
				for (const string& container_name : larva_queen_producers) {
					for (Entity& entity : state._name_to_entities[container_name]) {
						if (!entity._queen_constructing_in && entity._queen_larvas.size() <= 12) {
							//voila!
							injected_larvae = true;
							action_done = true;
							entity._queen_constructing_in = true;
							queen._energy -= 25;
							queen._queen_larvae_injected_before = true;
							json action_log;
							action_log["type"] = "special";
							action_log["name"] = "injectlarvae";
							action_log["triggeredBy"] = queen._id;
							action_log["targetBuilding"] = entity._id;

							cycle_log["events"].push_back(action_log);
							state._event_queue.addSpecialActionEvent(queen._id, entity._id, state._time_step + 40);
							break;
						}
					}
					if (injected_larvae)
						break;
				}
			}
			if (injected_larvae)
				break;
		}
	}

	/*
	* Redistribute workers
	*/

	if (action_done || (state._build_list.size() > 0 && _entity_data_map.at(state._build_list[0])._vespene > 0 && state._drones_assigned_to_vespene == 0)) {		
		if (!state._build_list.empty()) {
			unsigned int drone_count = state._name_to_entities["Drone"].size();
			double mineral_need = _entity_data_map.at(state._build_list[0])._minerals - state._minerals;
			double vespene_need = _entity_data_map.at(state._build_list[0])._vespene - state._vespene;

			if (vespene_need <= 0) {
				state._drones_assigned_to_mineral = drone_count;
				state._drones_assigned_to_vespene = 0;
			}
			else if (mineral_need <= 0) {
				state._drones_assigned_to_vespene = min((unsigned int)state._name_to_entities["extractor"].size() * 3U, drone_count);
				state._drones_assigned_to_mineral = drone_count - state._drones_assigned_to_vespene;
			}
			else {
				double min_steps = 123123123;		
				for (int assign_to_mineral = drone_count - 1; assign_to_mineral > 0; assign_to_mineral--) {
					int assign_to_vespene = drone_count - assign_to_mineral;
					if (assign_to_vespene > state._name_to_entities["Extractor"].size() * 3)
						break;

					double req_steps = max(vespene_need / (0.35 * assign_to_vespene),
						mineral_need / (0.7 * assign_to_mineral));

					if (req_steps < min_steps) {
						state._drones_assigned_to_vespene = assign_to_vespene;
						state._drones_assigned_to_mineral = assign_to_mineral;
						min_steps = req_steps;
					}
				}
			}
		}
	}

	cycle_log["status"]["resources"]["minerals"] = (int)(state._minerals + 1e-10);
	cycle_log["status"]["resources"]["vespene"] = (int)(state._vespene + 1e-10);
	cycle_log["status"]["resources"]["supply"] = (int)(state._supply + 1e-10);
	cycle_log["status"]["resources"]["supply-used"] = (int)(state._supply_used + 1e-10);
	cycle_log["status"]["workers"]["minerals"] = (int)(state._drones_assigned_to_mineral);
	cycle_log["status"]["workers"]["vespene"] = (int)(state._drones_assigned_to_vespene);

	if (action_done || build_ended)
		return cycle_log;

	return json();
}
