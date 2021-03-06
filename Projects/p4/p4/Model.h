#ifndef MODEL_H
#define MODEL_H

#include <string>
#include <map>
#include <vector>
#include "Utility.h"

class Sim_object;
class Ship;
class Island;
class View;
struct Point;

/*
 Model is part of a simplified Model-View-Controller pattern.
 Model keeps track of the Sim_objects in our little world. It is the only
 component that knows how many Islands and Ships there are, but it does not
 know about any of their derived classes, nor which Ships are of what kind of Ship.
 It has facilities for looking up objects by name, and removing Ships.  When
 created, it creates an initial group of Islands and Ships using the Ship_factory.
 Finally, it keeps the system's time.
 Controller tells Model what to do; Model in turn tells the objects what do, and
 when asked to do so by an object, tells all the Views whenever anything changes that might be relevant.
 Model also provides facilities for looking up objects given their name.
 */

// Declare the global model pointer
class Model;
extern Model* g_Model_ptr;

class Model {
public:
  // create the initial objects, output constructor message
  Model();
  
  // destroy all objects, output destructor message
  ~Model();
  
  // return the current time
  int get_time() {return time;}
  
  // is name already in use for either ship or island?
  // either the identical name, or identical in first two characters counts as in-use
  bool is_name_in_use(const std::string& name) const
  {
    return objects.find(name.substr(0, SHORTEN_NAME_LENGTH)) != objects.end();
  }
  
  // is there such an island?
  bool is_island_present(const std::string& name) const
  {
    return islands.find(name.substr(0, SHORTEN_NAME_LENGTH)) != islands.end();
  }
  // will throw Error("Island not found!") if no island of that name
  Island* get_island_ptr(const std::string& name) const;
  
  // is there such an ship?
  bool is_ship_present(const std::string& name) const
  {
    return ships.find(name.substr(0, SHORTEN_NAME_LENGTH)) != ships.end();
  }
  // add a new ship to the list, and update the view
  void add_ship(Ship*);
  // will throw Error("Ship not found!") if no ship of that name
  Ship* get_ship_ptr(const std::string& name) const;
  
  // tell all objects to describe themselves
  void describe() const;
  // increment the time, and tell all objects to update themselves
  void update();
  
  /* View services */
  // Attaching a View adds it to the container and causes it to be updated
  // with all current objects' locations (or other state information).
  void attach(View*);
  // Detach the View by discarding the supplied pointer from the container of Views
  // - no updates sent to it thereafter.
  void detach(View*);
  
  // notify the views about an object's location
  void notify_location(const std::string& name, Point location);
  // notify the views that an object is now gone
  void notify_gone(const std::string& name);
  
  // disallow copy/move construction or assignment
  Model(const Model&) = delete;
  Model& operator=(const Model&) = delete;
  
private:
  int time;		// the simulated time
  
  std::map<std::string, Island*> islands;
  std::map<std::string, Ship*> ships;
  std::map<std::string, Sim_object*> objects;
  
  std::vector<View*> views;
};

#endif
