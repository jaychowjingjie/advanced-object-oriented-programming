//
//  Ship_factory.cpp
//  Project4
//
//  Created by Jay Chow on 3/26/17.
//  Copyright © 2017 Jay Chow. All rights reserved.
//

#include "Ship_factory.h"
#include "Geometry.h"
#include "Ship.h"
#include "Tanker.h"
#include "Cruiser.h"
#include "Utility.h"
#include <string>

using namespace std;

const string FACTORY_TANKER_TYPE = "Tanker";
const string FACTORY_CRUISER_TYPE = "Cruiser";

// may throw Error("Trying to create ship of unknown type!")
Ship * create_ship(const string& name, const string& type, Point initial_position)
{
  if (type == FACTORY_TANKER_TYPE)
  {
    return new Tanker(name, initial_position);
  } else if (type == FACTORY_CRUISER_TYPE)
  {
    return new Cruiser(name, initial_position);
  }
  throw Error("Trying to create ship of unknown type!");
}
