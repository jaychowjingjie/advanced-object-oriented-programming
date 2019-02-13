//
//  Collection.cpp
//  Project3
//
//  Created by Jay Chow on 2/17/17.
//  Copyright © 2017 Jay Chow. All rights reserved.
//

#include "Collection.h"
#include "Record.h"
#include "Utility.h"
#include <algorithm>
#include <limits>
#include <iterator>  
#include <iostream>
using namespace std;

/* Construct a Collection from an input file stream in save format, using the record 
 list, restoring all the Record information. Record list is needed to resolve 
 references to record members. No check made for whether the Collection already 
 exists or not. Throw Error exception if invalid data discovered in file.
 String data input is read directly into the member variable. */

Collection::Collection(ifstream& is, const vector<Record*>& library) {
  int num_records = 0;
  if (!(is >> name >> num_records)) {
    throw_file_exception();
  }
  is.ignore(numeric_limits<streamsize>::max(), '\n');
  for (int i = 0; i < num_records; i++) {
    string title;
    getline(is, title);
    Record record(title);
    /* If matching item is present, lower_bound returns iterator that points to
     matching item. If matching item is not present, iterator points to where the
     sought-for item should be insertes
     */
    auto it = lower_bound(library.begin(), library.end(), &record, Title_less_ptr());
    if (it == library.end() || **it != record) {
      throw_file_exception();
    }
    list_of_record_ptrs.insert(*it);
  }
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
  return (list_of_record_ptrs.find(record_ptr) != list_of_record_ptrs.end());
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
  os << name << " " << list_of_record_ptrs.size() << endl;
  for_each(list_of_record_ptrs.begin(), list_of_record_ptrs.end(),
           [&os](Record* rec_ptr) {os << rec_ptr->get_title() << endl;});
  
}

// combine 2 collections to form new collection
Collection& Collection::operator+= (const Collection& rhs) {
  for_each(rhs.list_of_record_ptrs.begin(), rhs.list_of_record_ptrs.end(),
  [this](Record* rec_ptr) {if (!is_member_present(rec_ptr)) {list_of_record_ptrs.insert(rec_ptr);}});
  return *this;
}

// Print the Collection data
ostream& operator<< (ostream& os, const Collection& collection) {
  os << "Collection " << collection.name << " contains:";
  if (collection.empty()) {
    os << " None" << endl;
  }
  else {
    os << endl;
    // "adds" a newline in between rec_ptrs
    ostream_iterator<Record*> out_it(os, "\n");
    // overloaded << for *Record in record module, just output what rec_ptr points to
    copy(collection.list_of_record_ptrs.begin(), collection.list_of_record_ptrs.end(),
         out_it);
  }
  return os;
}





