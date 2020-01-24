#ifndef ZERG_ENTITY_ENTITYDATA_H
#define ZERG_ENTITY_ENTITYDATA_H
#include <string>
#include <vector>


using std::string;
using std::vector;

/*
* Holds the entity data for an entity as it is read from techtrees.csv
*/
class EntityData {
public:
  string _name;
  double _minerals;
  double _vespene;
  unsigned int _build_time;
  double _supply_cost;
  double _supply_provided;
  double _start_energy;
  double _max_energy;
  vector<string> _produced_by; // can be produced by and of these
  vector<string> _dependency;
  EntityData(string n, double m, double v, unsigned int bt,
          double sc, double sp, double se, double me,
          const vector<string>& pb, const vector<string>& d);
private:
};

#endif
