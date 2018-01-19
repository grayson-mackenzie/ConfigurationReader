#ifndef CONFIGINT_H
#define CONFIGINT_H

#include <iostream>
#include "../LuaScript.h"
#include "ConfigInterface.h"

class ConfigInt : public ConfigInterface {
 public:
  ConfigInt(string key_name);
  ConfigInt(string key_name, int upper_bound, int lower_bound);
  int getVal();
  bool setVal(LuaScript* script);

 protected:
  int u_bound;
  int l_bound;
  int val;
};

#endif
