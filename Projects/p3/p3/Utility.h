//
//  Utility.h
//  Project3
//
//  Created by Jay Chow on 2/17/17.
//  Copyright © 2017 Jay Chow. All rights reserved.
//

#ifndef UTILITY_H
#define UTILITY_H
struct Error {
  Error(const char* msg_ = "") : msg(msg_) {}
  const char* msg;
};

// error that does not require rest of line to be cleared
struct Error_no_clear {
  Error_no_clear(const char* msg_ = "") : msg(msg_) {}
  const char* msg;
};


void throw_file_exception();


#endif
