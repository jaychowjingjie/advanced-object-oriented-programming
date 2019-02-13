//
//  main.cpp
//  Project2
//
//  Created by Jay Chow on 1/29/17.
//  Copyright © 2017 Jay Chow. All rights reserved.
//

#include "Ordered_list.h"
#include "Utility.h"
#include "String.h"
#include "Record.h"
#include "Collection.h"
#include <iostream>
#include <fstream>
#include <new> // bad alloc
using namespace std;

/* Note: Good function tree, fundamental programming technique for
 organized code. Main contains top level loop that reads and executes
 each input command. Does this by reading two command characters and
 then calling a separate function, one for each command, that peforms
 the actual work of that command. Each command function must be
 implemented by calling a set of helper functions. Reuse helper
 functions across commands if possible.
 */
struct Record_ID_comp {
  bool operator() (const Record *lhs, const Record *rhs) const {
    return lhs->get_ID() < rhs->get_ID(); }
};

typedef Ordered_list<Collection*, Less_than_ptr<Collection*>> Catalog_t;
typedef Ordered_list<Record*, Less_than_ptr<Record*>> Library_title_t;
typedef Ordered_list<Record*, Record_ID_comp> Library_id_t;

static void find_record(Library_title_t& library_title);
static void print_record(Library_id_t& library_id);
static void print_collection(Catalog_t& catalog);
static void print_library(Library_title_t& library_title);
static void print_catalog(Catalog_t& catalog);
static void print_allocation(Library_title_t& library_title, Library_id_t& library_id, Catalog_t& catalog);
static void add_record(Library_title_t& library_title, Library_id_t& library_id);
static void add_collection(Catalog_t& catalog);
static void add_member(Library_id_t& library_id, Catalog_t& catalog);
static void modify_rating(Library_id_t& library_id);
static void delete_record(Library_title_t& library_title, Library_id_t& library_id, Catalog_t& catalog);
static void delete_collection(Catalog_t& catalog);
static void delete_member(Library_id_t& library_id, Catalog_t& catalog);
static void clear_library(Library_title_t& library_title, Library_id_t& library_id, Catalog_t& catalog);
static void clear_catalog(Catalog_t& catalog);
static void clear_all_data(Library_title_t& library_title, Library_id_t& library_id, Catalog_t& catalog);
static void save_all_data(Library_title_t& library_title, Catalog_t& catalog);
static void restore_all_data(Library_title_t& library_title, Library_id_t& library_id, Catalog_t& catalog);

static Library_title_t::Iterator get_record_by_title(Library_title_t& library_title);
static Library_id_t::Iterator get_record_by_id(Library_id_t& library_id);
static Catalog_t::Iterator get_collection_by_name(Catalog_t& catalog);
static void clear_libraries(Library_title_t& library_title, Library_id_t& library_id);
static void print_rec_apply(Record* rec_ptr); // used in apply
static void print_col_apply(Collection* col_ptr); // used in apply
static void read_and_compact_title(istream& is, String& title);

int main(int argc, const char * argv[]) {
  // {f,p,m,a,d,c,s,r}
  char actionLetter;
  // {r,c,m,L,C,A,a}
  char objectLetter;
  // 3 containers
  Catalog_t catalog;
  Library_title_t library_title;
  Library_id_t library_id;
  cout << "\nEnter command: ";
  try {
    while (cin >> actionLetter >> objectLetter) {
      try {
        switch (actionLetter) {
          case 'f': {
            switch (objectLetter) {
              case 'r':
                find_record(library_title);
                break;
              default:
                throw Error("Unrecognized command!");
                break;
            }
            break;
          }
          case 'p': {
            switch (objectLetter) {
              case 'r':
                print_record(library_id);
                break;
              case 'c':
                print_collection(catalog);
                break;
              case 'L':
                print_library(library_title);
                break;
              case 'C':
                print_catalog(catalog);
                break;
              case 'a':
                print_allocation(library_title, library_id, catalog);
                break;
              default:
                throw Error("Unrecognized command!");
                break;
            }
            break;
          }
          case 'a': {
            switch (objectLetter) {
              case 'r':
                add_record(library_title, library_id);
                break;
              case 'c':
                add_collection(catalog);
                break;
              case 'm':
                add_member(library_id, catalog);
                break;
              default:
                throw Error("Unrecognized command!");
                break;
            }
            break;
          }
          case 'm': {
            switch (objectLetter) {
              case 'r':
                modify_rating(library_id);
                break;
              default:
                throw Error("Unrecognized command!");
                break;
            }
            break;
          }
          case 'd': {
            switch (objectLetter) {
              case 'r':
                delete_record(library_title, library_id, catalog);
                break;
              case 'c':
                delete_collection(catalog);
                break;
              case 'm':
                delete_member(library_id, catalog);
                break;
              default:
                throw Error("Unrecognized command!");
                break;
            }
            break;
          }
          case 'c': {
            switch (objectLetter) {
              case 'L':
                clear_library(library_title, library_id, catalog);
                break;
              case 'C':
                clear_catalog(catalog);
                cout << "All collections deleted" << endl;
                break;
              case 'A':
                clear_all_data(library_title, library_id, catalog);
                break;
              default:
                throw Error("Unrecognized command!");
                break;
            }
            break;
          }
          case 's': {
            switch (objectLetter) {
              case 'A':
                save_all_data(library_title, catalog);
                break;
              default:
                throw Error("Unrecognized command!");
                break;
            }
            break;
          }
          case 'r': {
            switch (objectLetter) {
              case 'A':
                // restore all data structures
                restore_all_data(library_title, library_id, catalog);
                break;
              default:
                throw Error("Unrecognized command!");
                break;
            }
            break;
          }
          case 'q': {
            switch (objectLetter) {
              case 'q':
                clear_libraries(library_title, library_id);
                clear_catalog(catalog);
                cout << "All data deleted" << endl;
                cout << "Done" << endl;
                return 0;
              default:
                throw Error("Unrecognized command!");
                break;
            }
            break;
          }
          default:
            throw Error("Unrecognized command!");
            break;
        } // outer switch
      } // inner try
      catch (Error& err) {
        String garbage;
        cout << err.msg << endl;
        cin.clear();
        while (cin.get() != '\n');
      } // inner catch
      printf("\nEnter command: ");
    } // while
  } // outer try
  catch (bad_alloc& err) {
    cout << "Bad allocation" << endl;
    clear_libraries(library_title, library_id);
    clear_catalog(catalog);
    throw;
  }
  catch (String_exception& err) {
    cout << err.msg << endl;
    clear_libraries(library_title, library_id);
    clear_catalog(catalog);
  }
  catch (...) {
    cout << "Unknown Error" << endl;
    clear_libraries(library_title, library_id);
    clear_catalog(catalog);
  }
  return 0;
}

static void find_record(Library_title_t& library_title) {
  auto it = get_record_by_title(library_title);
  cout << **it << endl;
}

static void print_record(Library_id_t& library_id) {
  auto it = get_record_by_id(library_id);
  cout << **it << endl;
}

static void print_collection(Catalog_t& catalog) {
  auto it = get_collection_by_name(catalog);
  cout << *it << endl;
}

static void print_library(Library_title_t& library_title) {
  if (library_title.empty()) {
    cout << "Library is empty" << endl;
    return;
  }
  cout << "Library contains " << library_title.size() << " records:\n";
  apply(library_title.begin(), library_title.end(), print_rec_apply);
}

static void print_catalog(Catalog_t& catalog) {
  if (catalog.empty()) {
    cout << "Catalog is empty" << endl;
    return;
  }
  cout << "Catalog contains " <<  catalog.size() << " collections:\n";
  apply(catalog.begin(), catalog.end(), print_col_apply);
}

static void print_allocation(Library_title_t& library_title, Library_id_t& library_id, Catalog_t& catalog) {
  cout << "Memory allocations:" << endl;
  cout << "Records: " << library_id.size() << endl;
  cout << "Collections: " << catalog.size() << endl;
  cout << "Lists: "  << g_Ordered_list_count << endl;
  cout << "List Nodes: "  << g_Ordered_list_Node_count << endl;
  cout << "Strings: "     << String::get_number() << " with " <<
  String::get_total_allocation() << " bytes total" << endl;
}

static void add_record(Library_title_t& library_title, Library_id_t& library_id) {
  String medium, title;
  cin >> medium;
  read_and_compact_title(cin, title);
  Record rec(title);
  if (library_title.find(&rec) != library_title.end()) {
    throw Error("Library already has a record with this title!");
  }
  Record* rec_ptr = new Record(Record::ID_counter, medium, title);
  library_id.insert(rec_ptr);
  library_title.insert(rec_ptr);
  cout << "Record "<< rec_ptr->get_ID() << " added" << endl;
}

static void add_collection(Catalog_t& catalog) {
  String name;
  cin >> name;
  Collection collection(name);
  if (catalog.find(&collection) != catalog.end()) {
    throw Error("Catalog already has a collection with this name!");
  }
  catalog.insert(new Collection(name));
  cout << "Collection " << name <<  " added" << endl;
}

static void add_member(Library_id_t& library_id, Catalog_t& catalog) {
  auto col_iter = get_collection_by_name(catalog);
  auto rec_iter = get_record_by_id(library_id);
  (*col_iter)->add_member(*rec_iter);
  cout << "Member "<< (*rec_iter)->get_ID() << " " << (*rec_iter)->get_title()
  << " added" << endl;
}

static void modify_rating(Library_id_t& library_id) {
  auto it = get_record_by_id(library_id);
  int rating;
  cin >> rating;
  if (!cin)
  {
    throw Error("Could not read an integer value!");
  }
  (*it)->set_rating(rating);
  cout << "Rating for record " << (*it)->get_ID() <<  " changed to " << rating << endl;
}

static void delete_record(Library_title_t& library_title, Library_id_t& library_id, Catalog_t& catalog) {
  auto title_iter = get_record_by_title(library_title);
  Record* rec_ptr = *title_iter;
  for (auto& collection: catalog) {
    if(collection->is_member_present(*title_iter)) {
      throw Error("Cannot delete a record that is a member of a collection!");
    }
  }
  Record rec((*title_iter)->get_ID());
  auto id_iter = library_id.find(&rec);
  cout << "Record "<< (*id_iter)->get_ID() << " " << (*id_iter)->get_title() <<
  " deleted" << endl;
  library_title.erase(title_iter);
  library_id.erase(id_iter);
  delete rec_ptr;
}

static void delete_collection(Catalog_t& catalog) {
  auto it = get_collection_by_name(catalog);
  Collection* col_ptr = *it;
  cout << "Collection " << (*it)->get_name() <<  " deleted" << endl;
  catalog.erase(it);
  delete col_ptr;
}

static void delete_member(Library_id_t& library_id, Catalog_t& catalog){
  auto col_iter = get_collection_by_name(catalog);
  auto rec_iter = get_record_by_id(library_id);
  (*col_iter)->remove_member(*rec_iter);
  cout << "Member "<< (*rec_iter)->get_ID() << " " << (*rec_iter)->get_title()
  << " deleted" << endl;
}

static void clear_library(Library_title_t& library_title, Library_id_t& library_id, Catalog_t& catalog) {
  for (auto it = catalog.begin(); it != catalog.end(); ++it) {
    if (!(*it)->empty()) {
      throw Error("Cannot clear all records unless all collections are empty!");
    }
  }
  Record::reset_ID_counter();
  clear_libraries(library_title, library_id);
  cout << "All records deleted" << endl;
}

static void clear_catalog(Catalog_t& catalog) {
  for (auto col_ptr: catalog) {
    delete col_ptr;
  }
  catalog.clear();
}

static void clear_all_data(Library_title_t& library_title, Library_id_t& library_id, Catalog_t& catalog) {
  Record::reset_ID_counter();
  clear_libraries(library_title, library_id);
  clear_catalog(catalog);
  cout << "All data deleted" << endl;
}

static void save_all_data(Library_title_t& library_title, Catalog_t& catalog) {
  String file_name;
  cin >> file_name;
  ofstream out_file_stream(file_name.c_str());
  if(!out_file_stream.is_open()) {
    throw Error("Could not open file!");
  }
  try {
    out_file_stream << library_title.size() << endl;
    for (auto rec_ptr: library_title) {
      rec_ptr->save(out_file_stream);
    }
    out_file_stream << catalog.size() << endl;;
    for (auto col_ptr: catalog ) {
      col_ptr->save(out_file_stream) ;
    }
  }
  catch ( ... ) {
    out_file_stream.close();
    throw;
  }
  out_file_stream.close();
  cout << "Data saved" << endl;
}

static void restore_all_data(Library_title_t& library_title, Library_id_t& library_id, Catalog_t& catalog) {
  String file_name;
  cin >> file_name;
  ifstream in_file_stream(file_name.c_str());
  if(!in_file_stream.is_open()) {
    throw Error("Could not open file!");
  }
  int num;
  if (!(in_file_stream >> num)) {
    throw_file_exception();
  }
  if (num < 0) {
    throw_file_exception();
  }
  Catalog_t new_catalog;
  Library_title_t new_library_title;
  Library_id_t new_library_id;
  try {
    Record::save_ID_counter();
    Record::reset_ID_counter();
    for (int i = 0; i < num ; i++) {
      Record *rec_ptr = new Record(in_file_stream);
      new_library_title.insert(rec_ptr);
      new_library_id.insert(rec_ptr);
    }
    if (!(in_file_stream >> num)) {
      throw_file_exception();
    }
    if (num < 0) {
      throw_file_exception();
    }
    for (int i = 0; i < num; i++) {
      Collection *col_ptr = new Collection(in_file_stream, new_library_title);
      new_catalog.insert(col_ptr);
    }
    clear_libraries(library_title, library_id);
    clear_catalog(catalog);
    library_title = new_library_title;
    library_id = new_library_id;
    catalog = new_catalog;
    cout << "Data loaded\n";
    in_file_stream.close();
  }
  catch (Error& err) {
    clear_catalog(new_catalog);
    clear_libraries(new_library_title, new_library_id);
    Record::restore_ID_counter();
    throw_file_exception();
  }
}



static Library_title_t::Iterator get_record_by_title(Library_title_t& library_title) {
  String title;
  read_and_compact_title(cin, title);
  Record rec(title);
  auto it = library_title.find(&rec);
  if (it == library_title.end()) {
    throw Error("No record with that title!");
  }
  return it;
}

static Library_id_t::Iterator get_record_by_id(Library_id_t& library_id) {
  int id;
  cin >> id;
  if (!cin) {
    throw Error("Could not read an integer value!");
  }
  Record rec(id);
  auto it = library_id.find(&rec);
  if (it == library_id.end()) {
    throw Error("No record with that ID!");
  }
  return it;
}

static Catalog_t::Iterator get_collection_by_name(Catalog_t& catalog) {
  String name;
  cin >> name;
  Collection collection(name);
  auto it = catalog.find(&collection);
  if (it == catalog.end()) {
    throw Error("No collection with that name!");
  }
  return it;
}

static void print_rec_apply(Record* rec_ptr) {
  cout << *rec_ptr << endl;
}

static void print_col_apply(Collection* col_ptr) {
  cout << *col_ptr << endl;
}


static void clear_libraries(Library_title_t& library_title,
                            Library_id_t& library_id) {
  for (auto rec_ptr: library_title) {
    delete rec_ptr;
  }
  library_title.clear();
  // dynamic records are already deleted, just clear library_id container
  library_id.clear();
}

static void read_and_compact_title(istream& is, String& title) {
  title.clear();
  char reading_char;
  bool last_space = true;
  while(is.good() && is.peek() != '\n') {
    if (isspace(reading_char = is.get())) {
      if (!last_space) {
        title += reading_char;
      }
      last_space = true;
    }
    // non white-space
    else {
      last_space = false;
      title += reading_char;
    }
  }
  if (!title.size()) {
    throw Error("Could not read a title!");
  }
  if (isspace(title[title.size() - 1])) {
    title.remove(title.size() - 1 ,1);
  }
}
