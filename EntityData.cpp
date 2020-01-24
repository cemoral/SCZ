#include "EntityData.h"

EntityData::EntityData(string n, double m, double v, unsigned int bt,
          double sc, double sp, double se, double me,
          const vector<string>& pb, const vector<string>& d): _name(n),
           _minerals(m), _vespene(v), _build_time(bt), _supply_cost(sc),
           _supply_provided(sp), _start_energy(se), _max_energy(me),
           _produced_by(pb), _dependency(d) {

}
