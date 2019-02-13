//
//  Tanker.cpp
//  Project4
//
//  Created by Jay Chow on 3/26/17.
//  Copyright © 2017 Jay Chow. All rights reserved.
//
#include "Tanker.h"
#include "Ship.h"
#include "Island.h"
#include "Model.h"
#include "Geometry.h"
#include "Utility.h"
#include <string>
#include <iostream>
#include <cassert>

using namespace std;

const char* const CARGO_DEST_SAME_MSG = "Load and unload cargo destinations are the same!";
const char* const TANKER_HAS_DEST_MSG = "Tanker has cargo destinations!";
const double TANKER_INIT_FUEL = 100;
const double TANKER_MAX_SPEED = 10;
const double TANKER_FUEL_CONSUMPTION = 2;
const double TANKER_INIT_RESISTANCE = 0;
const double TANKER_INIT_CARGO = 0;
const double TANKER_CARGO_CAPACITY = 1000;

// initialize, the output constructor message
Tanker::Tanker(const std::string &name_, Point position_) :
Ship(name_, position_, TANKER_INIT_FUEL, TANKER_MAX_SPEED, TANKER_FUEL_CONSUMPTION, TANKER_INIT_RESISTANCE),
load_dest(nullptr), unload_dest(nullptr), cargo(TANKER_INIT_CARGO), cargo_capacity(TANKER_CARGO_CAPACITY),
tanker_state(State_tanker::NO_CARGO_DEST)
{
  cout << "Tanker " << get_name() << " constructed" << endl;
}

// output destructor message
Tanker::~Tanker()
{
  cout << "Tanker " << get_name() << " destructed" << endl;
}

// This class overrides these Ship functions so that it can check if this Tanker has assigned cargo destinations.
// if so, throw an Error("Tanker has cargo destinations!"); otherwise, simply call the Ship functions.
void Tanker::set_destination_position_and_speed(Point destination, double speed)
{
  if (tanker_state != State_tanker::NO_CARGO_DEST) throw Error(TANKER_HAS_DEST_MSG);
  Ship::set_destination_position_and_speed(destination, speed);
}

void Tanker::set_course_and_speed(double course, double speed)
{
  if (tanker_state != State_tanker::NO_CARGO_DEST) throw Error(TANKER_HAS_DEST_MSG);
  Ship::set_course_and_speed(course, speed);
}

// Set the loading and unloading Island destinations
// if both cargo destination are already set, throw Error("Tanker has cargo destinations!").
// if they are the same, leave at the set values, and throw Error("Load and unload cargo destinations are the same!")
// if both destinations are now set, start the cargo cycle
void Tanker::set_load_destination(Island *dest)
{
  assert(dest);
  if (tanker_state != State_tanker::NO_CARGO_DEST) throw Error(TANKER_HAS_DEST_MSG);
  if (unload_dest == dest) throw Error(CARGO_DEST_SAME_MSG);
  load_dest = dest;
  cout << get_name() << " will load at " << dest->get_name() << endl;
  if (unload_dest != nullptr) start_cycle();
}

void Tanker::set_unload_destination(Island *dest)
{
  assert(dest);
  if (tanker_state != State_tanker::NO_CARGO_DEST) throw Error(TANKER_HAS_DEST_MSG);
  if (load_dest == dest) throw Error(CARGO_DEST_SAME_MSG);
  unload_dest = dest;
  cout << get_name() << " will unload at " << dest->get_name() << endl;
  if (load_dest != nullptr) start_cycle();
}

// when told to stop, clear the cargo destinations and stop
void Tanker::stop()
{
  Ship::stop();
  end_cycle();
}

void Tanker::update()
{
  Ship::update();
  if (!can_move())
  {
    end_cycle();
    return;
  }
  double cargo_needed = cargo_capacity - cargo; // used in loading
  switch(tanker_state)
  {
    case State_tanker::NO_CARGO_DEST:
      return;
    case State_tanker::MOVING_TO_LOAD:
      if (is_moving()) return;
      dock(load_dest);
      tanker_state = State_tanker::LOADING;
      return;
    case State_tanker::MOVING_TO_UNLOAD:
      if (is_moving()) return;
      dock(unload_dest);
      tanker_state = State_tanker::UNLOADING;
      return;
    case State_tanker::LOADING:
      refuel();
      if (cargo_needed < REFUEL_MIN)
      {
        cargo = cargo_capacity;
        Ship::set_destination_position_and_speed(unload_dest->get_location(), get_maximum_speed());
        tanker_state = State_tanker::MOVING_TO_UNLOAD;
        return;
      }
      cargo += load_dest->provide_fuel(cargo_needed);
      cout << get_name() << " now has " << cargo << " of cargo" << endl;
      return;
    case State_tanker::UNLOADING:
      if (cargo == 0)
      {
        Ship::set_destination_position_and_speed(load_dest->get_location(), get_maximum_speed());
        tanker_state = State_tanker::MOVING_TO_LOAD;
        return;
      }
      unload_dest->accept_fuel(cargo);
      cargo = 0;
      return;
  }
}

void Tanker::describe() const
{
  cout << "\nTanker ";
  Ship::describe();
  cout << "Cargo: " << cargo << " tons";
  switch(tanker_state)
  {
    case State_tanker::NO_CARGO_DEST:
      cout << ", no cargo destinations" << endl;
      return;
    case State_tanker::LOADING:
      cout << ", loading" << endl;
      return;
    case State_tanker::UNLOADING:
      cout << ", unloading" << endl;
      return;
    case State_tanker::MOVING_TO_LOAD:
      cout << ", moving to loading destination" << endl;
      return;
    case State_tanker::MOVING_TO_UNLOAD:
      cout << ", moving to unloading destination" << endl;
      return;
  }
}

// Starts the tanker's cargo cycle
void Tanker::start_cycle()
{
  if (is_docked())
  {
    if (get_docked_Island() == load_dest)
    {
      tanker_state = State_tanker::LOADING;
      return;
    }
    if (get_docked_Island() == unload_dest)
    {
      tanker_state = State_tanker::UNLOADING;
      return;
    }
  }
  if (!is_moving())
  {
    if (cargo == 0 && can_dock(load_dest))
    {
      dock(load_dest);
      tanker_state = State_tanker::LOADING;
      return;
    }
    if (cargo > 0 && can_dock(unload_dest))
    {
      dock(unload_dest);
      tanker_state = State_tanker::UNLOADING;
      return;
    }
  }
  if (cargo == 0)
  {
    Ship::set_destination_position_and_speed(load_dest->get_location(), get_maximum_speed());
    tanker_state = State_tanker::MOVING_TO_LOAD;
    return;
  }
  if (cargo > 0)
  {
    Ship::set_destination_position_and_speed(unload_dest->get_location(), get_maximum_speed());
    tanker_state = State_tanker::MOVING_TO_UNLOAD;
    return;
  }
}

// Ends the tanker's cargo cycle
void Tanker::end_cycle()
{
  load_dest = unload_dest = nullptr;
  tanker_state = State_tanker::NO_CARGO_DEST;
  cout << get_name() << " now has no cargo destinations" << endl;
}
