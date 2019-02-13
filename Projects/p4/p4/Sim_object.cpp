//
//  Sim_object.cpp
//  Project4
//
//  Created by Jay Chow on 3/26/17.
//  Copyright © 2017 Jay Chow. All rights reserved.
//
#include "Sim_object.h"
#include <string>
#include <iostream>

using namespace std;

Sim_object::Sim_object(const std::string& name_) : name(name_)
{
  cout << "Sim_object " << get_name() << " constructed" << endl;
}

Sim_object::~Sim_object()
{
  cout << "Sim_object " << get_name() << " destructed" << endl;
}
