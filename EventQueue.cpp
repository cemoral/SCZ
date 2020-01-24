#include "EventQueue.h"
#include <cassert>
#include <iostream>

using std::cout;
using std::endl;
EventQueue::EventQueue(): special_action_event_count(0), total_event_count(0) {

}

bool EventPointerCompare::operator()(Event*& e1, Event*& e2) {
  return e1->_complete_at > e2->_complete_at;
}

bool EventQueue::isEmpty() const {
  return _pq.empty();
}

unsigned int EventQueue::getMinFinishTime() const {
  assert(!isEmpty());
  auto& top = _pq.top();
  return top->_complete_at;
}

Event* EventQueue::getNextEvent() const {
  assert(!isEmpty());
  return _pq.top();
}

void EventQueue::deleteNextEvent() {
  assert(!isEmpty());
  if(_pq.top()->_event_type == Event::EventType::SPECIAL_ACTION)
    special_action_event_count--;
  total_event_count--;
  delete getNextEvent();
  _pq.pop();
}



void EventQueue::addBuildEvent(const string& entity_name,
  const string& producer_id, unsigned int complete_at) {
  _pq.push((Event *) new BuildEvent(complete_at, entity_name, producer_id));
  total_event_count++;
}

void EventQueue::addBuildEvent(const string& entity_name,
  const string& producer_id, unsigned int complete_at, const vector<Entity>& cl,
  const vector<Entity>& cql) {
  _pq.push((Event *) new BuildEvent(complete_at, entity_name, producer_id, cl, cql));
  total_event_count++;

}


void EventQueue::addSpecialActionEvent(const string& triggered_by,
  const string& target_building, unsigned int complete_at) {
  special_action_event_count++;
  _pq.push((Event *) new SpecialActionEvent(complete_at,
    triggered_by, target_building));
  total_event_count++;
}
