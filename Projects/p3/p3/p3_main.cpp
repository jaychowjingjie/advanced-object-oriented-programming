//
//  main.cpp
//  Project3
//
//  Created by Jay Chow on 2/12/17.
//  Copyright © 2017 Jay Chow. All rights reserved.
//

#include "Utility.h"
#include "Record.h"
#include "Collection.h"
#include <iostream>
#include <fstream>
#include <cctype>
#include <algorithm>
#include <functional>
#include <iterator>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <list>
#include <new>
using namespace std;

/* Note: Good function tree, fundamental programming technique for organized code. 
 Main contains top level loop that reads and executes each input command. Does this 
 by reading two command characters and then calling a separate function, one for each 
 command, that peforms the actual work of that command. Each command function must be
 implemented by calling a set of helper functions. Reuse helper functions across 
 commands if possible.
 */

typedef vector<Record*> Record_ptr_container_t;
typedef vector<Collection> Collection_container_t;

struct Data_container {
  Collection_container_t cat;
  Record_ptr_container_t lib_title;
  Record_ptr_container_t lib_id;
};

// internal helpers for main to delegate to the modules, const correctness
static void find_record(Data_container&);
static void print_record(Data_container&);
static void print_collection(Data_container&);
static void print_library(const Data_container&);
static void print_catalog(const Data_container&);
static void print_allocation(const Data_container&);
static void add_record(Data_container&);
static void add_collection(Data_container&);
static void add_member(Data_container&);
static void modify_rating(Data_container&);
static void delete_record(Data_container&);
static void delete_collection(Data_container&);
static void delete_member(Data_container&);
static void clear_library(Data_container&);
static void clear_catalog(Data_container&);
static void clear_all_data(Data_container&);
static void save_all_data(const Data_container&);
static void restore_all_data(Data_container&);
static void find_with_string(const Data_container&);
static void list_ratings(const Data_container&);
static void collection_statistics(const Data_container&);
static void combine_collections(Data_container&);
static void modify_title(Data_container&);

static Record_ptr_container_t::iterator get_rec_it_by_title(Data_container&);
static Record_ptr_container_t::iterator get_rec_it_by_id(Data_container&);
static Collection_container_t::iterator get_col_it_by_name(Data_container&);
static const Record* insert_record(Data_container&, Record*);
static void insert_collection(Data_container&, const Collection&&);
static void clear_libraries(Data_container&);

// reads and compresses bad titles and returns compacted title
static string read_and_compact_title(istream& is);
static void check_title_exist_in_lib(Data_container&, const string&);
static void check_empty_title(const string& title);
static Record_ptr_container_t::iterator lib_title_lower_bound(Data_container&,
                                                              const Record*);
static Record_ptr_container_t::iterator lib_id_lower_bound(Data_container&,
                                                           const Record*);
static Collection_container_t::iterator cat_lower_bound(Data_container&,
                                                        const Collection&);

static int read_integer();

int main(int argc, const char * argv[]) {
  Data_container dc;
  // generates wrappers in the form of functors that call the const-correct functions
  // note syntax within angel brackets of function<...>
  map<string, function<void (Data_container&)>> cmd_container = {
    // all functions pointed have same signature
    {"fr", find_record},
    {"pr", print_record},
    {"pc", print_collection},
    {"pL", print_library},
    {"pC", print_catalog},
    {"pa", print_allocation},
    {"ar", add_record},
    {"ac", add_collection},
    {"am", add_member},
    {"mr", modify_rating},
    {"dr", delete_record},
    {"dc", delete_collection},
    {"dm", delete_member},
    {"cL", clear_library},
    {"cC", clear_catalog},
    {"cA", clear_all_data},
    {"sA", save_all_data},
    {"rA", restore_all_data},
    {"fs", find_with_string},
    {"lr", list_ratings},
    {"cs", collection_statistics},
    {"cc", combine_collections},
    {"mt", modify_title}
  };
  // {f,p,m,a,d,c,s,r}
  char actionLetter;
  // {r,c,m,L,C,A,a}
  char objectLetter;
  cout << "\nEnter command: ";
  while (cin >> actionLetter >> objectLetter) {
    try {
      string command;
      command += actionLetter;
      command += objectLetter;
      if (command == "qq") {
        clear_all_data(dc);
        cout << "Done" << endl;
        return 0;
      }
      if (cmd_container.find(command) == cmd_container.end()) {
        throw Error("Unrecognized command!");
      }
      // real program execution begins here
      else {
        // cmd_container[command] returns <value_type>, a functor which can be called
        // like a function, with dc as input of type <Data_container&>
        cmd_container[command](dc);
      }
    }
    catch (Error& err) {
      cin.clear();
      while (cin.get() != '\n');
      cout << err.msg << endl;
    }
    catch(Error_no_clear& err) {
      cout << err.msg << endl;
    }
    catch (bad_alloc) {
      clear_libraries(dc);
      throw;
    }
    catch (...) {
      clear_libraries(dc);
      throw;
    }
    printf("\nEnter command: ");
  } // while
  return 0;
}

static void find_record(Data_container& dc) {
  auto it = get_rec_it_by_title(dc);
  // it points to a rec_ptr, which points to a record on the heap
  cout << **it << endl;
}

static void print_record(Data_container& dc) {
  auto it = get_rec_it_by_id(dc);
  cout << **it << endl;
}

static void print_collection(Data_container& dc) {
  auto it = get_col_it_by_name(dc);
  cout << *it;
}

static void print_library(const Data_container& dc) {
  if (dc.lib_title.empty()) {
    cout << "Library is empty" << endl;
    return;
  }
  cout << "Library contains " << dc.lib_title.size() << " records:\n";
  ostream_iterator<Record*> out_it(cout, "\n");
  copy(dc.lib_title.begin(), dc.lib_title.end(), out_it);
}

static void print_catalog(const Data_container& dc) {
  if (dc.cat.empty()) {
    cout << "Catalog is empty" << endl;
    return;
  }
  cout << "Catalog contains " <<  dc.cat.size() << " collections:\n";
  ostream_iterator<Collection> out_it(cout);
  copy(dc.cat.begin(), dc.cat.end(), out_it);
}

static void print_allocation(const Data_container& dc) {
  cout << "Memory allocations:" << endl;
  cout << "Records: " << dc.lib_title.size() << endl;
  cout << "Collections: " << dc.cat.size() << endl;
}

static void add_record(Data_container& dc) {
  string medium, title;
  cin >> medium;
  title = read_and_compact_title(cin);
  check_empty_title(title);
  check_title_exist_in_lib(dc, title);
  const Record* rec_ptr = insert_record(dc, new Record(medium, title));
  cout << "Record "<< rec_ptr->get_ID() << " added" << endl;
}

static void add_collection(Data_container& dc) {
  string name;
  cin >> name;
  insert_collection(dc, Collection(name));
  cout << "Collection " << name <<  " added" << endl;
}

static void add_member(Data_container& dc) {
  // dereference to get collection object
  Collection& col = *get_col_it_by_name(dc);
  // dereference to get rec_ptr
  Record* rec_ptr = *get_rec_it_by_id(dc);
  col.add_member(rec_ptr);
  cout << "Member "<< rec_ptr->get_ID() << " " << rec_ptr->get_title()
  << " added" << endl;
}

static void modify_rating(Data_container& dc) {
  Record* rec_ptr = *get_rec_it_by_id(dc);
  int rating = read_integer();
  rec_ptr->set_rating(rating);
  cout << "Rating for record " << rec_ptr->get_ID() <<  " changed to " << rating << endl;
}

static void delete_record(Data_container& dc) {
  // need both after checking for error
  auto it = get_rec_it_by_title(dc);
  if (find_if(dc.cat.begin(), dc.cat.end(), bind(&Collection::is_member_present, placeholders::_1, *it)) != dc.cat.end()) {
    throw Error_no_clear("Cannot delete a record that is a member of a collection!");
  }
  auto rec_ptr = *it;
  dc.lib_title.erase(it);
  dc.lib_id.erase(lib_id_lower_bound(dc, rec_ptr));
  cout << "Record " << rec_ptr->get_ID() << " " << rec_ptr->get_title()
  << " deleted" << endl;
  // delete dynamic record on the heap
  delete rec_ptr;
}

static void delete_collection(Data_container& dc) {
  auto it = get_col_it_by_name(dc);
  Collection& col = *it;
  cout << "Collection " << col.get_name() <<  " deleted" << endl;
  dc.cat.erase(it);
}

static void delete_member(Data_container& dc){
  Collection& col = *get_col_it_by_name(dc);
  auto rec_ptr = *get_rec_it_by_id(dc);
  col.remove_member(rec_ptr);
  cout << "Member "<< rec_ptr->get_ID() << " " << rec_ptr->get_title()
  << " deleted" << endl;
}

static void clear_library(Data_container& dc) {
  if (find_if(dc.cat.begin(), dc.cat.end(), [](Collection& col) {return !col.empty();}) != dc.cat.end()) {
    throw Error("Cannot clear all records unless all collections are empty!");
  }
  Record::reset_ID_counter();
  clear_libraries(dc);
  cout << "All records deleted" << endl;
}

static void clear_catalog(Data_container& dc) {
  dc.cat.clear();
  cout << "All collections deleted" << endl;
}

static void clear_all_data(Data_container& dc) {
  Record::reset_ID_counter();
  clear_libraries(dc);
  dc.cat.clear();
  cout << "All data deleted" << endl;
}

static void save_all_data(const Data_container& dc) {
  string file_name;
  cin >> file_name;
  ofstream saving_file(file_name.c_str());
  if(!saving_file) {
    throw Error("Could not open file!");
  }
  saving_file << dc.lib_title.size() << endl;
  // used 2 lambdas separately
  for_each(dc.lib_title.begin(), dc.lib_title.end(), [&saving_file](Record* rec_ptr) {
    rec_ptr->save(saving_file);
  });
  saving_file << dc.cat.size() << endl;
  for_each(dc.cat.begin(), dc.cat.end(), [&saving_file](const Collection& col) {
    col.save(saving_file);
  });
  cout << "Data saved" << endl;
}

static void restore_all_data(Data_container& dc) {
  string file_name;
  cin >> file_name;
  ifstream restoring_file(file_name.c_str());
  if(!restoring_file) {
    throw Error("Could not open file!");
  }
  int num_rec = 0;
  if (!(restoring_file >> num_rec)) {
    throw_file_exception();
  }
  // fresh containers
  Data_container new_dc;
  try {
    Record::save_ID_counter();
    Record::reset_ID_counter();
    for (int i = 0; i < num_rec ; i++) {
      insert_record(new_dc, new Record(restoring_file));
    }
    int num_col = 0;
    if (!(restoring_file >> num_col)) {
      throw_file_exception();
    }
    for (int i = 0; i < num_col; i++) {
      insert_collection(new_dc, Collection(restoring_file, new_dc.lib_title));
    }
    // clear dc passed in
    dc.cat.clear();
    clear_libraries(dc);
    // safe to assign back to dc
    dc = new_dc;
    cout << "Data loaded\n";
    restoring_file.close();
  }
  catch (Error& err) {
    // something went wrong with new_dc
    new_dc.cat.clear();
    clear_libraries(new_dc);
    Record::restore_ID_counter();
    throw_file_exception();
  }
}

class String_finder {
public:
  // constructor
  String_finder(string key_) : key(to_lower_case(key_)) {}
  // note we have vector<Record*> in our libraries
  void operator()(Record* rec_ptr) {
    string title(to_lower_case(rec_ptr->get_title()));
    // match is found
    if (title.find(key) != string::npos) {
      matching_rec_ptrs.push_back(rec_ptr);
    }
    
  }
  vector<Record*> get_matches() {
    return matching_rec_ptrs;
  }
private:
  vector<Record*> matching_rec_ptrs;
  string key;
  // converts a string to its corresponding lower_case
  static string to_lower_case(const string& original) {
    string new_string;
    // allocate space
    new_string.resize(original.size());
    transform(original.begin(), original.end(), new_string.begin(), ::tolower);
    return new_string;
  }
};

static void find_with_string(const Data_container& dc) {
  string key;
  cin >> key;
  String_finder helper(key);
  helper = for_each(dc.lib_title.begin(), dc.lib_title.end(), helper);
  auto matching_rec_ptrs = helper.get_matches();
  if (matching_rec_ptrs.empty()) {
    throw Error("No records contain that string!");
  }
  ostream_iterator<Record*> out_it(cout, "\n");
  copy(matching_rec_ptrs.begin(), matching_rec_ptrs.end(), out_it);
}

static void list_ratings(const Data_container& dc) {
  if (dc.lib_title.empty()) {
    cout << "Library is empty" << endl;
    return;
  }
  Record_ptr_container_t sorted_by_rating = dc.lib_title;
  // sort by rating first, then title
  sort(sorted_by_rating.begin(), sorted_by_rating.end(), [](const Record* lhs, const Record* rhs) {
    // if ratings are equal, look at title to decide
    // if ratings are not equal, sort by highest rating to the left
    return lhs->get_rating() == rhs->get_rating() ? *lhs < *rhs : lhs->get_rating() > rhs->get_rating();
  });
  ostream_iterator<Record*> out_it(cout, "\n");
  copy(sorted_by_rating.begin(), sorted_by_rating.end(), out_it);
}

class Collection_statistican {
public:
  // constructor
  Collection_statistican() :
  num_rec_in_at_least_one_col(0), num_rec_in_many_col(0), num_rec_in_all_col(0) {}
  
  void operator()(const Collection& col) {
    // the one range for
    for (auto rec_ptr: col.get_list_of_record_ptrs()) {
      // add key-value pair if it did not exist yet
      if (map_of_id_to_num_col.find(rec_ptr->get_ID()) == map_of_id_to_num_col.end()) {
        map_of_id_to_num_col[rec_ptr->get_ID()] = 0;
      }
      // now check current value, which is num of col for a particular id while iterating
      int& current_count = map_of_id_to_num_col[rec_ptr->get_ID()];
      if (!current_count) {
        // if current_count is 0, it must be in at least 1 collection
        ++num_rec_in_at_least_one_col;
      }
      else if (current_count == 1) {
        // if current_count is 1, it must be in more than 1 collection
        ++num_rec_in_many_col;
      }
      ++num_rec_in_all_col;
      ++current_count;
    }
  }
  
  int get_one() {
    return num_rec_in_at_least_one_col;
  }
  int get_many() {
    return num_rec_in_many_col;
  }
  int get_all() {
    return num_rec_in_all_col;
  }
  
private:
  map<int, int> map_of_id_to_num_col;
  int num_rec_in_at_least_one_col;
  int num_rec_in_many_col;
  int num_rec_in_all_col;
};

static void collection_statistics(const Data_container& dc) {
  Collection_statistican helper;
  helper = for_each(dc.cat.begin(), dc.cat.end(), helper);
  int lib_size = (int)dc.lib_title.size();
  cout << helper.get_one() << " out of " << lib_size << " Records appear in at least one Collection" << endl;
  cout << helper.get_many() << " out of " << lib_size << " Records appear in more than one Collection" << endl;
  cout << "Collections contain a total of " << helper.get_all() << " Records" << endl;
}

static void combine_collections(Data_container& dc) {
  Collection& first_col = *get_col_it_by_name(dc);
  string first_name = first_col.get_name();
  Collection& second_col = *get_col_it_by_name(dc);
  string second_name = second_col.get_name();
  string new_name;
  cin >> new_name;
  Collection new_col(new_name, first_col);
  // concatenate
  new_col += second_col;
  insert_collection(dc, move(new_col));
  cout << "Collections " << first_name << " and " <<
  second_name << " combined into new collection " << new_name << endl;
}

static void modify_title(Data_container& dc) {
  auto it = get_rec_it_by_id(dc);
  auto rec_ptr = *it;
  // ensure new title is not in library
  string title = read_and_compact_title(cin);
  check_empty_title(title);
  check_title_exist_in_lib(dc, title);
  // remove record from all collections, and remember which collection it is
  list<Collection*> list_col_ptrs;
  for_each(dc.cat.begin(), dc.cat.end(), [&list_col_ptrs, rec_ptr](Collection& col) {
    if (col.is_member_present(rec_ptr)) {
      col.remove_member(rec_ptr);
      list_col_ptrs.push_back(&col);
    }
  });
  // remove record from the libraries
  dc.lib_id.erase(it);
  dc.lib_title.erase(lib_title_lower_bound(dc, rec_ptr));
  // change record's title and insert it back
  string last_title = rec_ptr->get_title();
  rec_ptr->set_title(title);
  insert_record(dc, rec_ptr);
  // add record back to all the collections it was originally in, using list
  for_each(list_col_ptrs.begin(), list_col_ptrs.end(), [rec_ptr](Collection* col_ptr) {col_ptr->add_member(rec_ptr);});
  cout << "Title for record " << rec_ptr->get_ID() << " changed to " << title << endl;
}

static Record_ptr_container_t::iterator get_rec_it_by_title(Data_container& dc) {
  string title = read_and_compact_title(cin);
  check_empty_title(title);
  Record rec(title);
  auto it = lib_title_lower_bound(dc, &rec);
  if (it == dc.lib_title.end() || **it != rec) {
    throw Error_no_clear("No record with that title!");
  }
  return it;
}

static Record_ptr_container_t::iterator get_rec_it_by_id(Data_container& dc) {
  int id = read_integer();
  Record rec(id);
  auto it = lib_id_lower_bound(dc, &rec);
  if (it == dc.lib_id.end() || **it != rec) {
    throw Error("No record with that ID!");
  }
  return it;
}

static Collection_container_t::iterator get_col_it_by_name(Data_container& dc) {
  string name;
  cin >> name;
  Collection col(name);
  auto it = cat_lower_bound(dc, col);
  if (it == dc.cat.end() || *it != col) {
    throw Error("No collection with that name!");
  }
  return it;
}

static void check_empty_title(const string& title) {
  if (title.empty()) {
    throw Error_no_clear("Could not read a title!");
  }
}

static void check_title_exist_in_lib(Data_container& dc, const string& title) {
  Record rec(title);
  auto it = lib_title_lower_bound(dc, &rec);
  if ((it != dc.lib_title.end()) && (**it == rec)) {
    throw Error_no_clear("Library already has a record with this title!");
  }
}

static const Record* insert_record(Data_container& dc, Record* rec_ptr) {
  // get right position to insert
  auto it_title = lib_title_lower_bound(dc, rec_ptr);
  // insert into lib_title container
  try {
    dc.lib_title.insert(it_title, rec_ptr);
  } catch (...) {
    delete rec_ptr;
    throw;
  }
  // insert into lib_id container, need separate try catch cause of possible failure
  // of insertion here. In this case, need to remove rec_ptr from lib_title
  try {
    auto it_id = lib_id_lower_bound(dc, rec_ptr);
    dc.lib_id.insert(it_id, rec_ptr);
  } catch (...) {
    // recall that items could still be erased from a const iterator, but NOT modified
    dc.lib_title.erase(it_title);
    delete rec_ptr;
    throw;
  }
  return rec_ptr;
}

static void insert_collection(Data_container& dc, const Collection&& col) {
  auto it = cat_lower_bound(dc, col);
  if ((it != dc.cat.end()) && (*it == col)) {
    throw Error("Catalog already has a collection with this name!");
  }
  dc.cat.insert(it, col);
}


static void clear_libraries(Data_container& dc) {
  // clear dynamic records first
  for_each(dc.lib_title.begin(), dc.lib_title.end(), [](Record* rec_ptr) {delete rec_ptr;});
  dc.lib_title.clear();
  dc.lib_id.clear();
}

// custom functor to help me compact title correctly
struct Title_Builder {
  // default constructor, set to be true to consider leading whitespace
  Title_Builder() : current_char_is_space(true) {};
  // used in STL algorithms
  void operator() (char c) {
    if (isspace(c)) {
      if (!current_char_is_space) {
        title.push_back(' ');
      }
      // NEED to reset bool to be true, for first whitespace after a word
      current_char_is_space = true;
    }
    // encounter useful char, concatenate to construct the title
    else {
      title.push_back(c);
      current_char_is_space = false;
    }
  }
  
  void remove_last_whitespace() {
    if (isspace(title.back())) {
      title.pop_back();
    }
  }
  
  string get_title() const {
    return title;
  }
  bool is_empty() const {
    return title.empty();
  }
  
private:
  string title;
  bool current_char_is_space;
};

static string read_and_compact_title(istream& is) {
  string bad_title;
  getline(is, bad_title);
  // pass in functor, to be applied to the result of derefencing every iterator in
  // range, in this case the signature is void func(const Type& a);
  Title_Builder title_helper =
  for_each(bad_title.begin(), bad_title.end(), Title_Builder());
  title_helper.remove_last_whitespace();
  return title_helper.get_title();
}

// lower_bound does a binary search and returns an iterator but it doesn't necessarily
// tell whether the item is present!

// Note: If matching item is present, lower_bound returns an iterator that points
// to the matching item in sequence. If the matching item is not present, the iterator
// points to where the sought-for-tem should be inserted
static Record_ptr_container_t::iterator lib_title_lower_bound(Data_container& dc,
                                                       const Record* rec_ptr) {
  return lower_bound(dc.lib_title.begin(), dc.lib_title.end(), rec_ptr, Title_less_ptr());
}

struct ID_comp {
  bool operator() (const Record *lhs, const Record *rhs) const {
    return lhs->get_ID() < rhs->get_ID(); }
};

static Record_ptr_container_t::iterator lib_id_lower_bound(Data_container& dc,
                                                    const Record* rec_ptr) {
  return lower_bound(dc.lib_id.begin(), dc.lib_id.end(), rec_ptr, ID_comp());
}

static Collection_container_t::iterator cat_lower_bound(Data_container& dc,
                                                 const Collection& col) {
  return lower_bound(dc.cat.begin(), dc.cat.end(), col);
}

static int read_integer() {
  int id;
  if (!(cin >> id)) {
    throw Error("Could not read an integer value!");
  }
  return id;
}
