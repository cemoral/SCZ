#include "Entity.h"
#include <random>
#include <string>
#include <unordered_set>

using std::string;
using std::mt19937;
using std::random_device;
using std::uniform_int_distribution;
using std::unordered_set;
using std::string;

unordered_set<string> Entity::_used_ids;

string Entity::generate_random_unused_id() {
  static const string alphanum(
    "0123456789");
  static mt19937 rng;
  rng.seed(random_device()());
  static uniform_int_distribution<mt19937::result_type> dist(0,
                                                  alphanum.size() - 1);
  do {
    _id.clear();
    int c = 10;
    while(c--) {
      _id.push_back(alphanum[dist(rng)]);
    }
  } while(Entity::_used_ids.find(_id) != Entity::_used_ids.end());

  Entity::_used_ids.insert(_id);
  return _id;
}

Entity::Entity(const string& name): _name(name), _time_to_next_larva(15),
  _energy(25), _queen_constructing_in(false), _producing_queen(false),
  _producing_nydus_worm(false), _queen_larvae_injected_before(false) {
  _id = generate_random_unused_id();
}

Entity::Entity(const char* name): Entity(string(name)) {

}

bool Entity::operator==(const Entity& rhs) const {
  return _id == rhs._id;
}
