//
//  Island.cpp
//  Project4
//
//  Created by Jay Chow on 3/26/17.
//  Copyright © 2017 Jay Chow. All rights reserved.
//
#include "Island.h"
#include "Model.h"
#include "Geometry.h"
#include <iostream>
#include <string>
using namespace std;

// initialize then output constructor message
Island::Island(const string &name_, Point position_, double fuel_, double production_rate_) :
Sim_object(name_), position(position_), fuel(fuel_), production_rate(production_rate_)
{
  cout << "Island " << get_name() << " constructed" << endl;
}

// output destructor message
Island::~Island()
{
  cout << "Island " << get_name() << " destructed" << endl;
}

// Return whichever is less, the request or the amount left,
// update the amount on hand accordingly, and output the amount supplied.
double Island::provide_fuel(double request)
{
  double min = request < fuel ? request : fuel;
  fuel -= min;
  cout << "Island " << get_name() << " supplied " << min << " tons of fuel" << endl;
  return min;
}

// Add the amount to the amount on hand, and output the total as the amount the Island now has.
void Island::accept_fuel(double amount)
{
  fuel += amount;
  cout << "Island " << get_name() << " now has " << fuel << " tons" << endl;
}

// if production_rate > 0, compute production_rate * unit time, and add to amount, and print an update message
void Island::update()
{
  if (production_rate <= 0) return;
  fuel += production_rate;
  cout << "Island " << get_name() << " now has " << fuel << " tons" << endl;
}

// output information about the current state
void Island::describe() const
{
  cout << "\nIsland " << get_name() << " at position " << position << "\nFuel available: " << fuel << " tons" << endl;
}

// ask model to notify views of current state
void Island::broadcast_current_state()
{
  g_Model_ptr->notify_location(get_name(), position);
}
