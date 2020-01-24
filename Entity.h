#ifndef ZERG_ENTITY_H
#define ZERG_ENTITY_H
#include <unordered_set>
#include <string>
#include <vector>


using std::vector;
using std::unordered_set;
using std::string;

/*
* Normally this should be a base class and we should have child classes
* for each unit and building seperately.
*
* I am kinda hacking it to not to complicate things for an homework
*/
class Entity {
private:
	static unordered_set<string> _used_ids;

	/*
	* adds it to used ids as well
	*/
	string generate_random_unused_id();
public:
	Entity(const string& name);
	Entity(const char* name);

	string _name;
	string _id;


	// only for hatcheries, lairs, and hives
	vector<Entity> _larvas;
	// larvas built by special action
	vector<Entity> _queen_larvas;
	/*
	* this holds at 15 if larva count is 3
	*
	* otherwise
	* this starts from 15 and goes to 0.
	* whenever it hits zero increase larva count by 1
	*/
	unsigned char _time_to_next_larva;
	/*
	* if queen used special action in this entity
	* this is set to true, otherwise it is false
	*/
	bool _queen_constructing_in;

	/*
	* If this entity(hatchery, hive, lair) is producing a queen this is set to
	* true
	*/
	bool _producing_queen;

	/*
	* If this entity(nydus network) is producing a nydus worm, this is set to
	* true
	*/
	bool _producing_nydus_worm;
	
	/*
	 * This will be used to make sure larvae is injected before increasing 
	 * queen's energy. (error management)
	*/
	bool _queen_larvae_injected_before;

	/*
	* Only for queen
	* starts at 50, goes upto 200
	* how does it go up?
	*/
	double _energy;

	bool operator==(const Entity& rhs) const;


};

#endif
