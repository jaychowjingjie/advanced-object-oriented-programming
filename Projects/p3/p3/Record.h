//
//  Record.h
//  Project3
//
//  Created by Jay Chow on 2/17/17.
//  Copyright © 2017 Jay Chow. All rights reserved.
//

#ifndef RECORD_H
#define RECORD_H
#include <fstream>
#include <ostream>
#include <string>
class Record {
  /*
   A Record ontains a unique ID number, a rating, and a title and medium name as 
   strings. */
public:
  // Create a Record object initialized with the supplied values. Rating is set to 0.
  Record(const std::string& medium_, const std::string& title_);
  
  // Create a Record object suitable for use as a probe containing the supplied
  // title. The ID and rating are set to 0, and the medium is an empty String.
  Record(const std::string& title_) : title(title_), ID(0), rating(0) {}
  
  // Create a Record object suitable for use as a probe containing the supplied ID
  // number. The rating is set to 0, and the medium and title are empty Strings.
  Record(int ID_) : ID(ID_), rating(0) {}
  
  // Construct a Record object from a file stream in save format.
  // Throw Error exception if invalid data discovered in file.
  // No check made for whether the Record already exists or not.
  // Input string data is read directly into the member variables.
  // The record number will be set from the saved data.
  Record(std::ifstream& is);
  
  // These declarations help ensure that Record objects are unique
  Record(const Record&) = delete;	// disallow copy construction
  Record(Record&&) = delete;	// disallow move construction
  Record& operator= (const Record&) = delete; // disallow copy assignment
  Record& operator= (Record&&) = delete; // disallow move assignment
  
  // Accessors
  int get_ID() const
		{return ID;}
  const std::string& get_title() const
		{return title;}
  int get_rating() const {
    return rating;
  }
  // if the rating is not between 1 and 5 inclusive, an exception is thrown
  void set_rating(int rating_);
  // modifies title of a record
  void set_title(const std::string& title_);
  /* 
   By declaring member function static, you make it independent of any particular
   object of the class. A static member function can be called even if no objects of
   the class exist and the static functions are accessed using only the class name
   and the scope resolution operator. A static member function can only access
   static data member, other static member functions and any other functions from 
   outside the class. Static member functions have a class scope and they do not have 
   access to the this pointer of the class.
   */
  static void reset_ID_counter() {
    ID_counter = 0;
  }
  // save ID counter into a private static member variable
  static void save_ID_counter() {
    ID_back_up = ID_counter;
  }
  // recover id counter from backup in a static member variable
  static void restore_ID_counter() {
    ID_counter = ID_back_up;
  }

  // Write a Record's data to a stream in save format with final endl.
  // The record ID number is saved.
  void save(std::ostream& os) const;
  
  // This operator defines the order relation between Records, based just on the title.
  bool operator< (const Record& rhs) const
		{return title < rhs.title;}
  
  // checks if title or id matches for both records to be same
  bool operator== (const Record& rhs) const {
    return (title == rhs.title || ID == rhs.ID);
  }
  // both ID and title have to be different for 2 records to be different
  bool operator!= (const Record& rhs) const {
    return (title != rhs.title && ID != rhs.ID);
  }
  /* *** fill in a friend declaration for the output operator */
  friend std::ostream& operator<< (std::ostream& os, const Record& record);
private:
  std::string medium;
  std::string title;
  int ID;
  int rating;
  static int ID_counter;
  static int ID_back_up;
  static const int min_rating;
  static const int max_rating;
};

// Print a Record's data to the stream without a final endl.
// Output order is ID number followed by a ':' then medium, rating, title, separated by one space.
// If the rating is zero, a 'u' is printed instead of the rating.
std::ostream& operator<< (std::ostream& os, const Record& record);

// Print a Record pointer's data to stream without a final edl
std::ostream& operator<< (std::ostream& os, const Record* rec_ptr);
#endif
