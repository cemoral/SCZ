#ifndef ZERG_EVENT_H
#define ZERG_EVENT_H
#include <string>
#include "Entity.h"

using std::string;

/*
* Base class for events
*/
class Event {
public:
  enum EventType {
    BUILD,
    SPECIAL_ACTION
  };

  /*
  * Gives the iteration number that the event will be completed
  */
  unsigned int _complete_at;

  /*
  * Stores the event type.
  * Used for polymorphism
  */
  EventType _event_type;

  /*
  * Constructor
  */
  Event(unsigned int in, EventType et);
};

/*
* event when entity morphs into something else or produces etc
*/
class BuildEvent: public Event {
public:
  string _entity_name;
  string _producer_id;
  // if a lair or hive is getting built, carryover these larva to new
  // structure
  vector<Entity> larva_carryover;
  vector<Entity> queen_larva_carryover;
  BuildEvent(unsigned int in, const string& en, const string& pi);
  BuildEvent(unsigned int in, const string& en, const string& pi,
    const vector<Entity>& lc, const vector<Entity>& qlc);
};



/*
* Event for special action
*/
class SpecialActionEvent: public Event {
public:
  string _triggered_by;
  string _target_building;
  SpecialActionEvent(unsigned int in, const string& tby, const string& tb);
};

#endif
