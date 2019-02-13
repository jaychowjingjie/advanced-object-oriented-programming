//
//  Collection.h
//  Project2
//
//  Created by Jay Chow on 1/29/17.
//  Copyright © 2017 Jay Chow. All rights reserved.
//

#ifndef COLLECTION_H
#define COLLECTION_H
#include "String.h"
#include "Ordered_list.h"
#include "Record.h"

class Collection {
  /* Collections contain a name and a container of members, represented as 
   pointers to Records. Collection objects manage their own Record container.
   The container of Records is not available to clients.
   */
  
public:
  // Construct a collection with the specified name and no members
  Collection(const String& name_) : name(name_) {}

  /* Construct a Collection from an input file stream in save format, using the 
   record list, restoring all the Record information. Record list is needed to resolve 
   references to record members. No check made for whether the Collection already 
   exists or not. Throw Error exception if invalid data discovered in file.
   String data input is read directly into the member variable. */
  Collection(std::ifstream& is, const Ordered_list<Record*,
             Less_than_ptr<Record*>>& library);
  
  // Accessors
  const String& get_name() const
		{return name;}
		
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
  
  // This operator defines the order relation between Collections, based just on the name
  bool operator< (const Collection& rhs) const
		{return name < rhs.name;}
  
  /* *** fill in a friend declaration for the output operator */
  friend std::ostream& operator<< (std::ostream& os, const Collection& collection);
		
private:
  /* *** the member information must be kept in a container of Record* - name is your choice */
	 Ordered_list<Record*, Less_than_ptr<Record*>> list_of_record_ptrs;
    /* *** other private members are your choice */
  String name;
};

// Print the Collection data
std::ostream& operator<< (std::ostream& os, const Collection& collection);

#endif
