//
//  Record.cpp
//  Project2
//
//  Created by Jay Chow on 1/29/17.
//  Copyright © 2017 Jay Chow. All rights reserved.
//

#include "Record.h"
#include "Utility.h"
#include <fstream>
using namespace std;

int Record::ID_counter = 0;
int Record::ID_back_up = 0;
// Construct a Record object from a file stream in save format.
// Throw Error exception if invalid data discovered in file.
// No check made for whether the Record already exists or not.
// Input string data is read directly into the member variables.
// The record number will be set from the saved data.
Record::Record(ifstream& is) {
  if (!(is >> ID >> medium >> rating)) {
    throw_file_exception();
  }
  if (ID < 0 || rating < 0) {
    throw_file_exception();
  }
  if (ID > ID_counter) {
    ID_counter = ID;
    std::cout << ID_counter << endl;
  }
  // now care about 1 leading whitespace
  if (!(is.get())) {
    throw_file_exception();
  }
  getline(is, title);
}

// if the rating is not between 1 and 5 inclusive, an exception is thrown
void Record::set_rating(int rating_) {
  int min_rating = 1;
  int max_rating = 5;
  if (rating_ < min_rating || rating_ > max_rating) {
    throw Error("Rating is out of range!");
  }
  rating = rating_;
}

// Write a Record's data to a stream in save format with final endl.
// The record ID number is saved.
void Record::save(ostream& os) const {
  os << ID << " " << medium << " " << rating << " " << title << endl;
}

// Print a Record's data to the stream without a final endl.
// Output order is ID number followed by a ':' then medium, rating, title, separated by one space.
// If the rating is zero, a 'u' is printed instead of the rating.
ostream& operator<< (ostream& os, const Record& record) {
  os << record.ID << ": " << record.medium << " ";
  if (!(record.rating)) {
    os << "u";
  }
  else {
    os << record.rating;
  }
  os << " " << record.title;
  return os;
}

