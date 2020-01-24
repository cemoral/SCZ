#include <iostream>
#include <unordered_map>
#include <fstream>

#include <json/single_include/nlohmann/json.hpp>
#include <fast-cpp-csv-parser/csv.h>

#include "Entity.h"
#include "Event.h"
#include "EventQueue.h"
#include "utility.h"
#include "EntityData.h"
#include "Simulation.h"


using std::unordered_map;
using std::cout;
using std::endl;
using std::ifstream;
using nlohmann::json;

int main(int argc, char* argv[]) {
  if(argc < 2) {
    cout << "Please provide a buildListFile" << endl
      << "Usage ./<executable-name> <buildListFile> <tectTreesFile>" << endl;
    return 0;
  } else if(argc < 3) {
    cout << "Please provide a techTreesFile" << endl
      << "Usage ./<executable-name> <buildListFile> <tectTreesFile>" << endl;
    return 0;
  }

  /*
  * Read the build list
  */
  ifstream build_list_in(argv[1]);
  vector<string> build_list;
  string token;
  while(build_list_in >> token) {
      build_list.push_back(token);
  }



  /*
  * This structure holds the data of the csv file
  */
  unordered_map<string, EntityData> entity_data_map;

  /*
  * Read csv file into entityDataMap;
  */
  io::CSVReader<11> tech_trees_in(argv[2]);
  tech_trees_in.read_header(io::ignore_extra_column, "id", "minerals", "vespene",
          "build_time", "supply_cost", "supply_provided", "start_energy",
          "max_energy", "race", "producer", "dependencies");

  string name, race, produced_by, dependency;
  double minerals, vespene, supply_cost, supply_provided,
    start_energy, max_energy;
  unsigned int build_time;

  while(tech_trees_in.read_row(name, minerals, vespene, build_time, supply_cost,
        supply_provided, start_energy, max_energy, race, produced_by,
         dependency)) {
    if(race == "Zerg") {
      vector<string> produced_by_tokenized = tokenize(produced_by, '/');
      vector<string> dependency_tokenized = tokenize(dependency, '/');
      entity_data_map.emplace(name, EntityData(name, minerals, vespene,
        build_time, supply_cost, supply_provided, start_energy, max_energy,
        produced_by_tokenized, dependency_tokenized));
    }
  }

  /*
  * Run simulation and print the result
  */
  Simulation sim(build_list, entity_data_map);
  json log = sim.run();
  cout << log << endl;

  return 0;
}
