#include "dice.h"
#include "rng.h"
#include "window.h"
#include <sstream>

int Dice::roll()
{
  if (number == 0 || sides == 0) {
    return 0;
  }
  int ret = dice(number, sides) + bonus;
  for (int i = 0; i < others.size(); i++) {
    ret += others[i].roll();
  }
  return ret;
}

Dice Dice::base() const
{
  return Dice(number, sides, bonus);
}

Dice& Dice::operator=(const Dice& rhs)
{
  if (this == &rhs) {
    return (*this);
  }
  number = rhs.number;
  sides  = rhs.sides;
  bonus  = rhs.bonus;
  others.clear();
  for (int i = 0; i < rhs.others.size(); i++) {
    others.push_back( rhs.others[i] );
  }
  return (*this);
}

Dice& Dice::operator+=(const Dice& rhs)
{
  if ((rhs.sides > 0 && rhs.number > 0) || rhs.bonus > 0) {
    others.push_back(rhs.base());
  }
  for (int i = 0; i < rhs.others.size(); i++) {
    (*this) += rhs.others[i].base();
  }
  return *this;
}

Dice& Dice::operator-=(const int& rhs)
{
  bonus += rhs;
  return *this;
}

std::string Dice::str()
{
  std::stringstream ret;
  ret << number << "d" << sides;
  if (bonus < 0) {
    ret << " - " << int(0 - bonus);
  } else {
    ret << " + " << bonus;
  }
  for (int i = 0; i < others.size(); i++) {
    ret << " + " << others[i].str();
  }
  return ret.str();
}

bool Dice::load_data(std::istream &data, std::string owner_name)
{
  if (!data.good()) {
    return false;
  }

// Set all values to 0, in case they're not loaded here.
  number = 0;
  sides  = 0;
  bonus  = 0;

  int current_number = 0;
  bool set_number = false, set_sides = false, negative_bonus = false;
  bool done = false;
  while (!done && data.good()) {
    char ch = data.get();
    //debugmsg("Dice '%c'", ch);
    if (data.good()) {
      if (ch >= '0' && ch <= '9') {
        current_number = current_number * 10 + (ch - '0');
        //debugmsg("Dice Current %d", current_number);
      } else if (ch == 'd' || ch == 'D') {
        if (set_number) {
          debugmsg("Two 'd' characters in dice spec (%s)", owner_name.c_str());
          return false;
        }
        set_number = true;
        number = current_number;
        current_number = 0;
      } else if (ch == '+' || ch == '-') {
        if (set_sides) {
          debugmsg("More than one bonus in dice spec (%s)", owner_name.c_str());
          return false;
        }
        if (!set_number) {
          debugmsg("Encountered + before d in dice spec (%s)",
                   owner_name.c_str());
          return false;
        }
        set_sides = true;
        sides = current_number;
        current_number = 0;
        if (ch == '-') {
          negative_bonus = true;
        }
      } else if (ch == '\n' || ch == ';') {
        done = true;
      } else if (ch != ' ') {
        debugmsg("Extraneous character '%c' in dice spec (%s)", ch,
                 owner_name.c_str());
        return false;
      }
    } // if (data.good())
  } // while (!done && data.good())

/* If we've set our sides, then anything left is the bonus
 * If we haven't set our sides OR our number of dice, then it's just a number,
 *  i.e., it's our bonus - just a flat value.
 */
  if (set_sides || !set_number) {
    if (negative_bonus) {
      bonus = 0 - current_number;
    } else {
      bonus = current_number;
    }
  } else if (!set_sides) {
// If we haven't set our sides, then it's the number of sides our dice have
    sides = current_number;
  }
  return true;
}

