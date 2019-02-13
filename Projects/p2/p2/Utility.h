//
//  Utility.h
//  Project2
//
//  Created by Jay Chow on 1/29/17.
//  Copyright © 2017 Jay Chow. All rights reserved.
//

#ifndef UTILITY_H
#define UTILITY_H

// self-implemented error class
struct Error {
  Error(const char* msg_ = "") : msg(msg_) {}
  const char* msg;
};

void throw_file_exception();

#endif
