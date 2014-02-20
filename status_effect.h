#ifndef _STATUS_EFFECT_H_
#define _STATUS_EFFECT_H_

#include <string>
#include <istream>
#include <vector>

enum Status_effect_type
{
  STATUS_NULL = 0,
  STATUS_BLIND,     // "blind" - lose sense of sight
  STATUS_CAFFEINE,  // "caffeine" - minor speed & stat boost
  STATUS_PAINKILL_MILD, // "painkill_mild" - lift painkill to 10
  STATUS_PAINKILL_MED,  // "painkill_med" - lift painkill to 50
  STATUS_PAINKILL_HEAVY,// "painkill_heavy" - lift painkill to 100
  STATUS_MAX
};

Status_effect_type lookup_status_effect(std::string name);
std::string status_effect_name(Status_effect_type type);

struct Status_effect
{
  Status_effect();
  Status_effect(Status_effect_type _type, int _duration, int _level = 1);
  ~Status_effect();

  bool load_data(std::istream& data, std::string owner_name);
  std::string get_name();

  void boost(int dur, int lev = 1);
  void boost(const Status_effect& rhs);
// Returns true if timed out
  bool decrement();

  Status_effect_type type;
  int duration;
  std::vector<int> step_down;  // The duration(s) at which we lose a level.
  int level;
};

#endif
