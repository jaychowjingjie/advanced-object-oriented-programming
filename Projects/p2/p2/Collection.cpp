//
//  Collection.cpp
//  Project2
//
//  Created by Jay Chow on 1/29/17.
//  Copyright © 2017 Jay Chow. All rights reserved.
//

#include "Collection.h"
#include "Utility.h"
#include <fstream>
using namespace std;

/* Construct a Collection from an input file stream in save format,
 using the record list, restoring all the Record information.
 Record list is needed to resolve references to record members.
 No check made for whether the Collection already exists or not.
 Throw Error exception if invalid data discovered in file.
 String data input is read directly into the member variable. */
Collection::Collection(ifstream& is, const Ordered_list<Record*,
                       Less_than_ptr<Record*>>& library) {
  int num_records = 0;
  if (!(is >> name >> num_records)) {
    throw_file_exception();
  }
  for (int i = 0; i < num_records; i++) {
    // sucks out '\n' after num_records, proceed to read next line below
    while (is.get() != '\n');
    String title;
    getline(is, title);
    Record record(title);
    // container of pointers to rec
    auto it = library.find(&record);
    // check that title must exist in library
    if (it == library.end()) {
      throw_file_exception();
    }
    // add the pointer
    list_of_record_ptrs.insert(*it);
  } // for
}

// Add the Record, throw exception if there is already a Record with the same title.
void Collection::add_member(Record* record_ptr) {
  if (is_member_present(record_ptr)) {
    throw Error("Record is already a member in the collection!");
  }
  list_of_record_ptrs.insert(record_ptr);
}

// Return true if the record is present, false if not.
bool Collection::is_member_present(Record* record_ptr) const {
  if (list_of_record_ptrs.find(record_ptr) != list_of_record_ptrs.end()) {
    return true;
  }
  return false;
}

// Remove the specified Record, throw exception if the record was not found.
void Collection::remove_member(Record* record_ptr) {
  auto it = list_of_record_ptrs.find(record_ptr);
  if (it == list_of_record_ptrs.end()) {
    throw Error("Record is not a member in the collection!");
  }
  list_of_record_ptrs.erase(it);
}

// Write a Collections's data to a stream in save format, with endl as specified.
void Collection::save(ostream& os) const {
  os << name << " " << list_of_record_ptrs.size();
  for (auto it = list_of_record_ptrs.begin(); it != list_of_record_ptrs.end(); ++it) {
    // *it is a pointer
    os << endl << (*it)->get_title();
  }
  os << endl;
}

// Print the Collection data
ostream& operator<< (ostream& os, const Collection& collection) {
  os << "Collection " << collection.name << " contains:";
  if (collection.empty()) {
    os << " None";
  }
  else {
    for (auto it = collection.list_of_record_ptrs.begin();
         it != collection.list_of_record_ptrs.end(); ++it) {
      // *it gets the Record ptr, dereferencing it once more gets the Record object
      os << endl << *(*it);
    }
  }
  return os;
}





