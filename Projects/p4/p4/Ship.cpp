#include "Ship.h"
#include "Model.h"
#include "Geometry.h"
#include "Navigation.h"
#include "Utility.h"
#include "Island.h"
#include <string>
#include <sstream>
#include <iostream>
#include <cassert>

using namespace std;

const char* const CANNOT_ATTACK_MSG = "Cannot attack!";
const int SHIP_DOCK_DISTANCE = .1;

Ship::Ship(const string &name_, Point position_, double fuel_capacity_,
           double maximum_speed_, double fuel_consumption_, int resistance_) :
Sim_object(name_), Track_base(position_, Course_speed(0, 0)), fuel(fuel_capacity_),
fuel_consumption(fuel_consumption_), fuel_capacity(fuel_capacity_), max_speed(maximum_speed_),
resistance(resistance_), ship_state(State_ship::STOPPED), docked_at(nullptr)
{
  cout << "Ship " << get_name() << " constructed" << endl;
}

/*
 Define the destructor function even if it was declared as a pure virtual function.
 This seems odd, because pure functions are usually not defined in the class
 that declares them. But this is often done as a way to make a class abstract,
 if there is no other function that makes sense to mark as pure. Here we
 are defining it just to get the destructor message output.
 */

Ship::~Ship()
{
  cout << "Ship "  << get_name() << " destructed" << endl;
}

// Return true if the ship is Stopped and the distance to the supplied island
// is less than or equal to 0.1 nm
bool Ship::can_dock(Island *island_ptr) const
{
  assert(island_ptr);
  return ship_state == State_ship::STOPPED && cartesian_distance(get_location(), island_ptr->get_location()) <= SHIP_DOCK_DISTANCE;
}

/*** Interface to derived classes ***/
// Update the state of the Ship
void Ship::update()
{
  if (is_afloat() && resistance < 0)
  {
    ship_state = State_ship::SINKING;
    docked_at = nullptr;
    set_speed(0);
    cout << get_name() << " sinking" << endl;
    return;
  }
  switch(ship_state)
  {
    case State_ship::SINKING:
      ship_state = State_ship::SUNK;
      g_Model_ptr->notify_gone(get_name());
      cout << get_name() << " sunk" << endl;
      break;
    case State_ship::SUNK:
      ship_state = State_ship::ON_THE_BOTTOM;
    case State_ship::ON_THE_BOTTOM:
      cout << get_name() << " on the bottom" << endl;
      break;
    case State_ship::MOVING_ON_COURSE:
    case State_ship::MOVING_TO_POSITION:
      calculate_movement();
      g_Model_ptr->notify_location(get_name(), get_location());
      cout << get_name() << " now at " << get_location() << endl;
      break;
    case State_ship::STOPPED:
      cout << get_name() << " stopped at " << get_location() << endl;
      break;
    case State_ship::DOCKED:
      cout << get_name() << " docked at " << get_docked_Island()->get_name() << endl;
      break;
    case State_ship::DEAD_IN_THE_WATER:
      cout << get_name() << " dead in the water at " << get_location() << endl;
      break;
  }
}

// output a description of current state to cout
void Ship::describe() const
{
  cout << get_name() << " at " << get_location();
  switch(ship_state)
  {
    case State_ship::SINKING:
      cout << " sinking" << endl;
      return;
    case State_ship::SUNK:
      cout << " sunk" << endl;
      return;
    case State_ship::ON_THE_BOTTOM:
      cout << " on the bottom" << endl;
      return;
    default:
      cout << ", fuel: " << fuel << " tons, resistance: " << resistance << endl;
      break;
  }
  switch(ship_state)
  {
    case State_ship::MOVING_TO_POSITION:
      cout << "Moving to " << destination << " on ";
      print_course_and_speed();
      cout << endl;
      break;
    case State_ship::MOVING_ON_COURSE:
      cout << "Moving on ";
      print_course_and_speed();
      cout << endl;
      break;
    case State_ship::DOCKED:
      cout << "Docked at " << get_docked_Island()->get_name() << endl;
      break;
    case State_ship::STOPPED:
      cout << "Stopped" << endl;
      break;
    case State_ship::DEAD_IN_THE_WATER:
      cout << "Dead in the water" << endl;
      break;
    default:
      // this should never happen, because the other states are covered in the previous switch
      assert(false);
      break;
  }
}

void Ship::broadcast_current_state()
{
  g_Model_ptr->notify_location(get_name(), get_location());
}

/*** Command functions ***/
// Start moving to a destination position at a speed
// may throw Error("Ship cannot move!")
// may throw Error("Ship cannot go that fast!")
void Ship::set_destination_position_and_speed(Point destination_position, double speed)
{
  check_movement_and_speed(speed);
  destination = destination_position;
  Compass_vector compass(get_location(), destination);
  set_course(compass.direction);
  set_speed(speed);
  ship_state = State_ship::MOVING_TO_POSITION;
  docked_at = nullptr;
  cout << get_name() << " will sail on ";
  print_course_and_speed();
  cout << " to " << destination << endl;
}

// Start moving on a course and speed
// may throw Error("Ship cannot move!")
// may throw Error("Ship cannot go that fast!");
void Ship::set_course_and_speed(double course, double speed)
{
  check_movement_and_speed(speed);
  set_course(course);
  set_speed(speed);
  ship_state = State_ship::MOVING_ON_COURSE;
  docked_at = nullptr;
  cout << get_name() << " will sail on ";
  print_course_and_speed();
  cout << endl;
}

// Stop moving
// may throw Error("Ship cannot move!");
void Ship::stop()
{
  if (!can_move())
  {
    throw Error("Ship cannot move!");
  }
  set_speed(0);
  ship_state = State_ship::STOPPED;
  docked_at = nullptr;
  cout << get_name() << " stopping at " << get_location() << endl;
}

// dock at an Island - set our position = Island's position, go into Docked state
// may throw Error("Can't dock!");
void Ship::dock(Island *island_ptr)
{
  assert(island_ptr);
  if (!can_dock(island_ptr))
  {
    throw Error("Can't dock!");
  }
  Track_base::set_position(island_ptr->get_location());
  docked_at = island_ptr;
  ship_state = State_ship::DOCKED;
  g_Model_ptr->notify_location(get_name(), get_location());
  cout << get_name() << " docked at " << island_ptr->get_name() << endl;
}

// Refuel - must already be docked at an island; fill takes as much as possible
// may throw Error("Must be docked!");
void Ship::refuel()
{
  if (ship_state != State_ship::DOCKED)
  {
    throw Error("Must be docked!");
  }
  double fuel_needed = fuel_capacity - fuel;
  if (fuel_needed < REFUEL_MIN)
  {
    fuel = fuel_capacity;
    return;
  }
  fuel += docked_at->provide_fuel(fuel_needed);
  cout << get_name() << " now has " << fuel << " tons of fuel" << endl;
}

/*** Fat interface command functions ***/
// These functions throw an Error exception for this class
// will always throw Error("Cannot load at a destination!");
void Ship::set_load_destination(Island *)
{
  throw Error("Cannot load at a destination!");
}

// will always throw Error("Cannot unload at a destination!");
void Ship::set_unload_destination(Island *)
{
  throw Error("Cannot unload at a destination!");
}

// will always throw Error("Cannot attack!");
void Ship::attack(Ship *in_target_ptr)
{
  throw Error(CANNOT_ATTACK_MSG);
}

// will always throw Error("Cannot attack!");
void Ship::stop_attack()
{
  throw Error(CANNOT_ATTACK_MSG);
}

// interactions with other objects
// receive a hit from an attacker
void Ship::receive_hit(int hit_force, Ship *attacker_ptr)
{
  resistance -= hit_force;
  cout << get_name() << " hit with " << hit_force << ", resistance now " << resistance << endl;
}

/* Private Function Definitions */

/*
 Calculate the new position of a ship based on how it is moving, its speed, and
 fuel state. This function should be called only if the state is
 MOVING_TO_POSITION or MOVING_ON_COURSE.
 Track_base has an update_position(double time) function that computes the new position
 of an object after the specified time has elapsed. If the Ship is going to move
 for a full time unit (one hour), then it will get go the "full step" distance,
 so update_position would be called with time = 1.0. If we can move less than that,
 e.g. due to not enough fuel, update position  will be called with the corresponding
 time less than 1.0.
 For clarity in specifying the computation, this code assumes the specified private variable names,
 but you may change the variable names or enum class names, or state names if you wish (e.g. movement_state).
 */
void Ship::calculate_movement()
{
  // Compute values for how much we need to move, and how much we can, and how long we can,
  // given the fuel state, then decide what to do.
  double time = 1.0;	// "full step" time
  // get the distance to destination
  double destination_distance = cartesian_distance(get_location(), destination);
  // get full step distance we can move on this time step
  double full_distance = get_speed() * time;
  // get fuel required for full step distance
  double full_fuel_required = full_distance * fuel_consumption;	// tons = nm * tons/nm
  // how far and how long can we sail in this time period based on the fuel state?
  double distance_possible, time_possible;
  if(full_fuel_required <= fuel)
  {
    distance_possible = full_distance;
    time_possible = time;
  }
  else
  {
    distance_possible = fuel / fuel_consumption;	// nm = tons / tons/nm
    time_possible = (distance_possible / full_distance) * time;
  }
  
  // are we are moving to a destination, and is the destination within the distance possible?
  if(ship_state == State_ship::MOVING_TO_POSITION && destination_distance <= distance_possible)
  {
    // yes, make our new position the destination
    set_position(destination);
    // we travel the destination distance, using that much fuel
    double fuel_required = destination_distance * fuel_consumption;
    fuel -= fuel_required;
    set_speed(0.);
    ship_state = State_ship::STOPPED;
  }
  else
  {
    // go as far as we can, stay in the same movement state
    // simply move for the amount of time possible
    update_position(time_possible);
    // have we used up our fuel?
    if(full_fuel_required >= fuel)
    {
      fuel = 0.0;
      set_speed(0.);
      ship_state = State_ship::DEAD_IN_THE_WATER;
    }
    else
    {
      fuel -= full_fuel_required;
    }
  }
}

// Check if the ship can move and the speed is within the max, and throws errors otherwise
void Ship::check_movement_and_speed(double speed)
{
  if (!can_move())
  {
    throw Error("Ship cannot move!");
  }
  if (speed > get_maximum_speed())
  {
    throw Error("Ship cannot go that fast!");
  }
}

// Prints the course and speed
void Ship::print_course_and_speed() const
{
  cout << "course " << get_course() << " deg, speed " << get_speed() << " nm/hr";
}
