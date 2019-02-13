//
//  Utility.cpp
//  Project3
//
//  Created by Jay Chow on 2/17/17.
//  Copyright © 2017 Jay Chow. All rights reserved.
//

#include "Utility.h"
#include <iostream>
using namespace std;

// used in various modules
void throw_file_exception() {
  throw Error("Invalid data found in file!");
}
