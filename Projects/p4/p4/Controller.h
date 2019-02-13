#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <string>
#include <map>

class Model;
class View;
class Ship;
class Island;

/* Controller
 This class is responsible for controlling the Model and View according to interactions
 with the user.
 */
class Controller {
public:
  // output constructor message
  Controller();
  // output destructor message
  ~Controller();
  
  // create View object, run the program by accepting user commands, then destroy View object
  void run();
  
private:
  // helper functions
  int read_int();
  double read_double();
  double read_speed();
  Ship* read_ship();
  Island* read_island();
  
  // command functions
  typedef bool (Controller::*command_func)(View *);
  // these functions return true if execution is to be ended, false otherwise
  bool quit(View *view);
  
  // view functions
  bool view_default(View *view);
  bool view_size(View *view);
  bool view_zoom(View *view);
  bool view_pan(View *view);
  bool view_show(View *view);
  
  // model functions
  bool model_status(View *view);
  bool model_go(View *view);
  bool model_create(View *view);
  
  // ship functions
  typedef void (Controller::*ship_func)(Ship *);
  void ship_course(Ship *ship);
  void ship_position(Ship *ship);
  void ship_destination(Ship *ship);
  void ship_load_at(Ship *ship);
  void ship_unload_at(Ship *ship);
  void ship_dock_at(Ship *ship);
  void ship_attack(Ship *ship);
  void ship_refuel(Ship *ship);
  void ship_stop(Ship *ship);
  void ship_stop_attack(Ship *ship);
  
  std::map<std::string, command_func> command_func_map {
    {"quit", &Controller::quit},
    
    {"default", &Controller::view_default},
    {"size", &Controller::view_size},
    {"zoom", &Controller::view_zoom},
    {"pan", &Controller::view_pan},
    {"show", &Controller::view_show},
    
    {"status", &Controller::model_status},
    {"go", &Controller::model_go},
    {"create", &Controller::model_create}
  };
  
  std::map<std::string, ship_func> ship_func_map {
    {"course", &Controller::ship_course},
    {"position", &Controller::ship_position},
    {"destination", &Controller::ship_destination},
    {"load_at", &Controller::ship_load_at},
    {"unload_at", &Controller::ship_unload_at},
    {"dock_at", &Controller::ship_dock_at},
    {"attack", &Controller::ship_attack},
    {"refuel", &Controller::ship_refuel},
    {"stop", &Controller::ship_stop},
    {"stop_attack", &Controller::ship_stop_attack}
  };
};

#endif
