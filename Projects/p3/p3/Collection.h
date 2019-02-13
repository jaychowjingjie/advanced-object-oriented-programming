//
//  Collection.h
//  Project3
//
//  Created by Jay Chow on 2/17/17.
//  Copyright © 2017 Jay Chow. All rights reserved.
//

#ifndef COLLECTION_H
#define COLLECTION_H
#include "Record.h"
#include "Utility.h"
#include <fstream>
#include <ostream>
#include <string>
#include <set>
#include <vector>


struct Title_less_ptr {
  bool operator() (const Record* ptr_lhs, const Record* ptr_rhs) const {
    // uses < from Record by referencing
    return (*ptr_lhs < *ptr_rhs);
  }
};

typedef std::set<Record*, Title_less_ptr> Record_set_t;

class Collection {
  /* Collections contain a name and a container of members, represented as 
   pointers to Records. Collection objects manage their own Record container.
   The container of Records is not available to clients.
   */
public:
  // Construct a collection with the specified name and no members
  Collection(const std::string& name_) : name(name_) {}
  
  Collection(const std::string& name_, const Collection& col_) :
  list_of_record_ptrs(col_.list_of_record_ptrs), name(name_) {}

  /* Construct a Collection from an input file stream in save format, using the 
   record list, restoring all the Record information. Record list is needed to resolve 
   references to record members. No check made for whether the Collection already 
   exists or not. Throw Error exception if invalid data discovered in file.
   string data input is read directly into the member variable. */
  Collection(std::ifstream& is, const std::vector<Record*>& library);
  
  // Accessors
  const std::string& get_name() const
		{return name;}
  const Record_set_t get_list_of_record_ptrs() const
  {return list_of_record_ptrs;}
  // Add the Record, throw exception if there is already a Record with the same title.
  void add_member(Record* record_ptr);
  // Return true if there are no members; false otherwise
  bool empty() const
		{return list_of_record_ptrs.empty();}
  // Return true if the record is present, false if not.
  bool is_member_present(Record* record_ptr) const;
  // Remove the specified Record, throw exception if the record was not found.
  void remove_member(Record* record_ptr);
  // discard all members
  void clear()
		{list_of_record_ptrs.clear();}
  // Write a Collections's data to a stream in save format, with endl as specified.
  void save(std::ostream& os) const;
  // combine 2 collections to form new collection
  Collection& operator+= (const Collection& rhs);
  // This operator defines the order relation between Collections, based just on the name
  bool operator< (const Collection& rhs) const
		{return name < rhs.name;}
  // additional overloading of operators to compare collection objects
  bool operator== (const Collection& rhs) const
  {return name == rhs.name;}
  bool operator!= (const Collection& rhs) const
  {return name != rhs.name;}
  /* *** fill in a friend declaration for the output operator */
  friend std::ostream& operator<< (std::ostream& os, const Collection& collection);
		
private:
  Record_set_t list_of_record_ptrs;
  std::string name;
};

// Print the Collection data
std::ostream& operator<< (std::ostream& os, const Collection& collection);

#endif
