#include "Event.h"

using std::string;

Event::Event(unsigned int in, EventType et):
    _complete_at(in), _event_type(et)  {

}

BuildEvent::BuildEvent(unsigned int in, const string& en, const string& pi):
  Event(in, Event::EventType::BUILD), _entity_name(en), _producer_id(pi) {

}

BuildEvent::BuildEvent(unsigned int in, const string& en, const string& pi,
  const vector<Entity>& lc, const vector<Entity>& qlc):
  Event(in, Event::EventType::BUILD), _entity_name(en), _producer_id(pi),
  larva_carryover(lc), queen_larva_carryover(qlc) {

}


SpecialActionEvent::SpecialActionEvent(unsigned int in
  , const string& tby, const string& tb): Event(in, Event::EventType::SPECIAL_ACTION),
  _triggered_by(tby), _target_building(tb) {

}
