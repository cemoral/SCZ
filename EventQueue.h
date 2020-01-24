#ifndef ZERG_EVENT_QUEUE_H
#define ZERG_EVENT_QUEUE_H
#include <queue>
#include <string>
#include <vector>
#include "Event.h"

using std::priority_queue;
using std::string;
using std::vector;

class EventPointerCompare {
public:
  bool operator()(Event*& e1, Event*& e2);
};

class EventQueue {
public:

  EventQueue();

  int special_action_event_count;
  int total_event_count;

  /*
  * Returns true if queue is empty
  */
  bool isEmpty() const;

  /*
  * Returns the min finish time of all events in the queue
  */
  unsigned int getMinFinishTime() const;

  /*
  * Return the next event from the queue
  */
  Event* getNextEvent() const;

  /*
  * Delete the next event from the queue
  */
  void deleteNextEvent();


  /*
  * add a MORPH event where producer with producer_id morphs into entity_name
  * at iteration complete_at.
  */
  void addBuildEvent(const string& entity_name,
    const string& producer_id, unsigned int complete_at);

  // with carryover
  void addBuildEvent(const string& entity_name,
    const string& producer_id, unsigned int complete_at, const vector<Entity>& cl,
    const vector<Entity>& cql);


  /*
  * add a SPECIAL_ACTION event where queen with id triggered_by produces 4
  * larva in a building with id target_building
  */
  void addSpecialActionEvent(const string& triggered_by,
    const string& target_building, unsigned int complete_at);
private:
  priority_queue<Event*, vector<Event*>, EventPointerCompare> _pq;
};

#endif
