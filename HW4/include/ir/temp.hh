#ifndef _TEMP
#define _TEMP

#include <map>
#include <string>
#include <cstdio>

using namespace std;

namespace tree {

class Temp {
  public:
    int num;
    Temp(int num) : num(num) {}
    int name() {
      return num;
    }
    string str() {
      return "t" + to_string(num);
    }
    bool operator==(const Temp& t) const {
      return num == t.num;
    }
};

class Label {
  public:
    int num;
    Label(int num) : num(num) {}
    int name() {
      return num;
    }
    string str() {
      return "L" + to_string(num);
    }
    bool operator==(const Label& l) const {
      return num == l.num;
    }
};

class Temp_map {
  public:
    map<int, bool> t_map; // temp map
    map<int, bool> l_map; // label map
    int next_temp; // next temp
    int next_label; // next label

    Temp_map() {
      next_temp = 100;
      next_label = 100;
      t_map.clear();
      l_map.clear();
    }

    Temp* newtemp() {
      while (t_map[next_temp]) {
        next_temp++;
      }
      t_map[next_temp] = true;
      return new Temp(next_temp++);
    }

    Label* newlabel() {
      while (l_map[next_label]) {
        next_label++;
      }
      l_map[next_label] = true;
      return new Label(next_label++);
    }

    // get the temp with its name (number) if it exists (else return nullptr)
    Temp* named_temp(int num) {
      if (!t_map[num]) t_map[num] = true;
        return new Temp(num);
    }

    Label* named_label(int num) {
      if (!l_map[num]) l_map[num] = true;
      return new Label(num);
    }

    bool in_tempmap(int num) {
      return t_map[num];
    }

    bool in_labelmap(int num) {
      return l_map[num];
    }
};

} // namespace tree

#endif
