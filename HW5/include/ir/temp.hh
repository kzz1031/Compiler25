#ifndef _TEMP
#define _TEMP

#include <set>
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
    //temp equality is just based on the number, not the object!
    //That is: two test if two temp are the same, you need to check if their numbers are the same!
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
    //label equality is just based on the number, not the object!
    //That is: two test if two label are the same, you need to check if their numbers are the same!
    bool operator==(const Label& l) const {
      return num == l.num;
    }
};

class String_Label {
  public:
    string name;
    String_Label(string name) : name(name) {}
    string str() {
      return name;
    }
};

class Temp_map {
  public:
    map<int, bool> t_map; // temp map
    map<int, bool> l_map; // label map
    int next_temp; // next temp
    int next_label; // next label
    set<string> s_labels; //set of string labels

    Temp_map() {
      next_temp = 100; //start from 100
      next_label = 100;
      t_map.clear();
      l_map.clear();
      s_labels.clear();
    }

    Temp* newtemp() {
      while (t_map[next_temp]) { //just to make sure the temp is unique (in terms of nnumber/name)
        next_temp++;
      }
      t_map[next_temp] = true;
      return new Temp(next_temp++);
    }

    Label* newlabel() {
      while (l_map[next_label]) { //just to make sure the label is unique (in terms of nnumber/name)
        next_label++;
      }
      l_map[next_label] = true;
      return new Label(next_label++);
    }

    String_Label* newstringlabel(string name) {
      s_labels.insert(name);
      return new String_Label(name);
    }

    bool in_stringlabel(string name) {
      return s_labels.find(name) != s_labels.end();
    }

    set<string> get_all_stringlabels() {
      return s_labels;
    }

    // get the temp with its name (number) if it exists (else return nullptr)
    Temp* named_temp(int num) {
      if (!t_map[num]) t_map[num] = true;
        return new Temp(num); //even if the same numbered temp exists, we return a new object 
        //but note that the equality of temps are based on the number, not the object!
    }

    Label* named_label(int num) {
      if (!l_map[num]) l_map[num] = true;
      return new Label(num);  //even if the same numbered label exists, we return a new object
        //but note that the equality of labels are based on the number, not the object!
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
