//
//  Cruiser.cpp
//  Project4
//
//  Created by Jay Chow on 3/26/17.
//  Copyright © 2017 Jay Chow. All rights reserved.
//
#include "Cruiser.h"
#include "Warship.h"
#include "Ship.h"
#include "Geometry.h"
#include <string>
#include <iostream>

using namespace std;

const double CRUISER_INIT_FUEL = 1000;
const double CRUISER_MAX_SPEED = 20;
const double CRUISER_FUEL_CONSUMPTION = 10;
const double CRUISER_INIT_RESISTANCE = 6;
const double CRUISER_FIREPOWER = 3;
const double CRUISER_MAX_RANGE = 15;

// initialize, then output constructor message
Cruiser::Cruiser(const string &name_, Point position_) :
Warship(name_, position_, CRUISER_INIT_FUEL, CRUISER_MAX_SPEED, CRUISER_FUEL_CONSUMPTION,
        CRUISER_INIT_RESISTANCE, CRUISER_FIREPOWER, CRUISER_MAX_RANGE)
{
  cout << "Cruiser " << get_name() << " constructed" << endl;
}

// output destructor message
Cruiser::~Cruiser()
{
  cout << "Cruiser " << get_name() << " destructed" << endl;
}

void Cruiser::update()
{
  Warship::update();
  if (!is_attacking()) return;
  if (target_in_range())
  {
    fire_at_target();
  }
  else
  {
    cout << get_name() << " target is out of range" << endl;
    stop_attack();
  }
}

void Cruiser::describe() const
{
  cout << "\nCruiser ";
  Warship::describe();
}

void Cruiser::receive_hit(int hit_force, Ship *attacker_ptr)
{
  Ship::receive_hit(hit_force, attacker_ptr);
  if (!is_attacking()) attack(attacker_ptr);
}
