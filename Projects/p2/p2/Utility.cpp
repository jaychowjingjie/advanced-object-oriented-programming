//
//  Utility.cpp
//  Project2
//
//  Created by Jay Chow on 1/29/17.
//  Copyright © 2017 Jay Chow. All rights reserved.
//

#include "Utility.h"
#include <iostream>
using namespace std;

// used in various modules
void throw_file_exception() {
  throw Error("Invalid data found in file!");
}
