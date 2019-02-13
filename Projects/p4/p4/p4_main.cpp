//
//  p4_main.cpp
//  Project4
//  Main module. Your project must use this file as-is
//
//  Created by Jay Chow on 3/7/17.
//  Copyright © 2017 Jay Chow. All rights reserved.
//

#include "Model.h"
#include "Controller.h"

#include <iostream>

using namespace std;

// The main function creates the Model and Controller objects, 
// then tells the Controller to run. 

int main ()
{
  // Set output to show two decimal places for floating point numbers
  //	cout << fixed << setprecision(2) << endl;
  cout.setf(ios::fixed, ios::floatfield);
  cout.precision(2);
  
  // create a Model object;
  g_Model_ptr = new Model;
  // create the Controller object and go
  Controller controller;
  
  controller.run();
  
  delete g_Model_ptr;
}

