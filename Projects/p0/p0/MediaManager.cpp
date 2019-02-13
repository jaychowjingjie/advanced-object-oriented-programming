//
//  MediaManager.cpp
//  Project0
//
//  Created by Jay Chow on 1/5/17.
//  Copyright © 2017 Jay Chow. All rights reserved.
//

#include "MediaManager.h"
#include <iostream>
#include <cctype>
#include <deque>

using namespace std;

/* Put function definitions in a readable order. A readable order
 * for the functons is a TOP-DOWN, BREADTH-FIRST order, with the
 * top-level function coming first and the lowest-level functions coming
 * last. Definitions of sub-functions/helper functions should appear
 * after the first function that calls them, or possibly all functions 
 * that call them, BUT NEVER BEFORE! 
 * Idea: Think of big picture first, details later,
 * finest details last. This is a critical aspect of code quality.
 */


/************************Top level functions***********************/

void findRecord(char objectLetter, const Library_t& library) {
  switch (objectLetter) {
    // find and print record with matching title
    case 'r': {
      string badTitle;
      getline(cin, badTitle);
      string title = compactEmbeddedTitle(badTitle);
      // "" or "    " comes here
      if (title.empty()) {
        cout << "Could not read a title!\n";
        return;
      }
      if(!titleExist(title, library)) {
        cout << "No record with that title!\n";
        return;
      }
      printOneRecord(title, library);
      break;
    }
    default:
      cout << "Unrecognized command!\n";
      discardInput();
      break;
  }
  
  
}

void print(char objectLetter, const Library_t& library,
           const Catalog_t& catalog) {
    switch (objectLetter) {
    // print record with matching ID #
    case 'r': {
      int id = 0;
      cin >> id;
      // calls getline if return true
      if (badIntegerExist(id)) {
        // user did not provide integer value, stream is not good
        cout << "Could not read an integer value!\n";
        return;
      }
      // stream is good, input valid and can be used
      printRecordById(id, library);
      break;
    }
    // print collection, print each record in the collection
    case 'c': {
      string collectionName;
      cin >> collectionName;
      // check name exist in catalog, a map<string, Collection>
      if (!collectionNameExist(collectionName, catalog)) {
        cout << "No collection with that name!\n";
        discardInput();
        return;
      }
      printOneCollection(collectionName, library, catalog);
      break;
    }
    // print records in Libray
    case 'L':
      printLibrary(library);
      break;
    // print catalog - all collections in catalog
    case 'C':
      printCatalog(library, catalog);
      break;
    // print memory allocations - # of records and # collections present
    case 'a':
      printMemoryAllocation(library, catalog);
      break;
    default:
      cout << "Unrecognized command!\n";
      discardInput();
      break;
  }
  
}

void modifyRating(char objectLetter, Library_t& library) {
  switch (objectLetter) {
    // modify rating of specific record
    case 'r': {
      int id = 0;
      cin >> id;
      // this function calls getline if cin fails
      if (badIntegerExist(id)) {
        // user did not provide integer value, stream is not good
        cout << "Could not read an integer value!\n";
        return;
      }
      if (!idExist(id, library)) {
        cout << "No record with that ID!\n";
        discardInput();
        return;
      }
      int rating = 0;
      cin >> rating;
      if (badIntegerExist(rating)) {
        cout << "Could not read an integer value!\n";
        return;
      }
      if (modifyRatingOutOfRange(rating)) {
        cout << "Rating is out of range!\n";
        discardInput();
        return;
      }
      modifyRatingForRecord(id, rating, library);
      break;
    }
    default:
      cout << "Unrecognized command!\n";
      discardInput();
      break;
  }
  
}

void add(char objectLetter, Library_t& library, Catalog_t& catalog,
         int& idCounter) {
  switch (objectLetter) {
    // add a record to library
    case 'r': {
      string medium;
      // no need to error check for medium, but for title
      cin >> medium;
      string badTitle;
      getline(cin, badTitle);
      string title = compactEmbeddedTitle(badTitle);
      // "" or "    " enters here
      if (title.empty()) {
        cout << "Could not read a title!\n";
        return;
        
      }
      if (titleExist(title, library)) {
        //cannot add to set
        cout << "Library already has a record with this title!\n";
        return;
      }
      // now can safely add record in library
      addRecord(medium, title, library, idCounter);
      break;
    }
    // add a collection with specified name
    case 'c': {
      string collectionName;
      cin >> collectionName;
      if (collectionNameExist(collectionName, catalog)) {
        cout << "Catalog already has a collection with this name!\n";
        discardInput();
        return;
      }
      addCollection(collectionName, catalog);
      break;
    }
    // add a record to specified collection
    case 'm': {
      string collectionName;
      cin >> collectionName;
      if (!collectionNameExist(collectionName, catalog)) {
        cout << "No collection with that name!\n";
        discardInput();
        return;
      }
      int id;
      cin >> id;
      if (badIntegerExist(id)) {
        cout << "Could not read an integer value!\n";
        return;
      }
      if (!idExist(id, library)) {
        cout << "No record with that ID!\n";
        discardInput();
        return;
      }
      // need library because provide title to search movie
      if (recordExistInCollection(id, collectionName, library, catalog)) {
        cout << "Record is already a member in the collection!\n";
        discardInput();
        return;
      }
      addMemberToCollection(id, collectionName, library, catalog);
    }
      break;
    default:
      cout << "Unrecognized command!\n";
      discardInput();
      break;
  }
  
}

void deletion(char objectLetter, Library_t& library, Catalog_t& catalog) {
  switch (objectLetter) {
    // delete specified record from library
    case 'r': {
      string badTitle;
      getline(cin, badTitle);
      string title = compactEmbeddedTitle(badTitle);
      if (title.empty()) {
        cout << "Could not read a title!\n";
        return;
      }
      if (!titleExist(title, library)) {
        cout << "No record with that title!\n";
        return;
      }
      // need to ensure record exists in entire catalog
      if (recordExistInCatalog(title, library, catalog)) {
        cout << "Cannot delete a record that is a member of a collection!\n";
        return;
      }
      deleteRecord(title, library);
    }
      break;
    // delete specified collection from catalog
    case 'c':{
      string collectionName;
      cin >> collectionName;
      if(!collectionNameExist(collectionName, catalog)) {
        cout << "No collection with that name!\n";
        discardInput();
        return;
      }
      deleteCollection(collectionName, catalog);
    }
      break;
    // delete specified record as member of specified collection
    case 'm': {
      string collectionName;
      int id = 0;
      cin >> collectionName;
      if (!collectionNameExist(collectionName, catalog)) {
        cout << "No collection with that name!\n";
        discardInput();
        return;
      }
      cin >> id;
      if (badIntegerExist(id)) {
        cout << "Could not read an integer value!\n";
        return;
      }
      if (!idExist(id, library)) {
        cout << "No record with that ID!\n";
        discardInput();
        return;
      }
      if (!recordExistInCollection(id, collectionName, library, catalog)) {
        cout << "Record is not a member in the collection!\n";
        discardInput();
        return;
      }
      deleteMemberFromCollection(id, collectionName, library, catalog);
    }
      break;
    default:
      cout << "Unrecognized command!\n";
      discardInput();
      break;
  }
}

void clear(char objectLetter, int& idCounter, Library_t& library,
           Catalog_t& catalog) {
  switch (objectLetter) {
    case 'L':
      // need to reset idCounter = 1 here, error checking
      clearLibrary(idCounter, library, catalog);
      break;
    case 'C':
      clearCatalog(catalog);
      break;
    case 'A':
      // also need to reset idCounter = 1 here, but no error checking
      clearAll(idCounter, library, catalog);
      cout << "All data deleted\n";
      break;
    default:
      cout << "Unrecognized command!\n";
      discardInput();
      break;
  }
  
}

void save(char objectLetter, Library_t& library, Catalog_t& catalog) {
  switch (objectLetter) {
    // save all data
    case 'A':
      saveData(library, catalog);
      break;
    default:
      cout << "Unrecognized command!\n";
      discardInput();
      break;
  }
  
}

void restore(char objectLetter, int& idCounter, Library_t& library,
             Catalog_t& catalog) {
  switch (objectLetter) {
    // restore all data
    case 'A':
      restoreData(idCounter, library, catalog);
      break;
    default:
      cout << "Unrecognized command!\n";
      discardInput();
      break;
  }
  
}


bool quit(char objectLetter, Library_t& library, Catalog_t& catalog) {
  switch (objectLetter) {
    case 'q':
      catalog.clear();
      library.clear();
      cout << "All data deleted\n";
      cout << "Done\n";
      return true;
    default:
      cout << "Unrecognized command!\n";
      discardInput();
      return false;
  }
  
}

void discardInput() {
  string useless;
  cin.clear();
  /* getline reads all user text on input line, up to newline
   * character resulting from pressing <ENTER>.
   */
  getline(cin, useless);
}



/**********************2nd level functions*************************/

bool badIntegerExist(int id) {
  if (!cin) {
    // discardInput calls cin.clear()
    discardInput();
    return true;
  }
  return false;
}

// cannot use this function for mr command!
// cannot modify rating back to 0
bool ratingOutOfRange(int rating) {
  // rating = [0,5], 0 means new record a.k.a movie
  if (rating < 0 || rating > 5) {
    return true;
  }
  return false;
}

// note that rating cannot be modified to be 0!
bool modifyRatingOutOfRange(int rating) {
  if (rating < 1 || rating > 5) {
    return true;
  }
  return false;
}


void printRecordById(int id, const Library_t& library) {
  
  if (!idExist(id, library)) {
    cout << "No record with that ID!\n";
    discardInput();
    return;
  }
  for (auto& movie: library) {
    if (movie.second.id == id) {
      printOneRecord(movie.first, library);
      return;
    }
  }
}


// Note: idCounter starts from 1 in main
void addRecord(const string& medium, const string& title,
               Library_t& library, int& idCounter) {
  
  // alread ensured that title is not inside
  library.insert({title, Movie(medium, idCounter)});
  cout << "Record " << idCounter << " added\n";
  // increment counter for next record created
  idCounter++;
}

void printLibrary(const Library_t& library) {
  if (library.empty()) {
    cout << "Library is empty\n";
    return;
  }
  cout << "Library contains " << library.size() << " records:\n";
  for (auto& movie: library) {
    printOneRecord(movie.first, library);
  }
}

void printCatalog(const Library_t& library, const Catalog_t& catalog) {
  if (catalog.empty()) {
    cout << "Catalog is empty\n";
    return;
  }
  cout << "Catalog contains " << catalog.size() << " collections:\n";
  // loop through each collection object
  for (auto& collection: catalog) {
    // 1 collection has [0-n] records, set<string> in each collection
    printOneCollection(collection.first, library, catalog);
  }
}


void printMemoryAllocation(const Library_t& library,
                           const Catalog_t& catalog) {
  cout << "Memory allocations:\n";
  cout << "Records: " << library.size() << "\n";
  cout << "Collections: " << catalog.size() << "\n";
}


void addCollection(const string& collectionName, Catalog_t& catalog) {
  // .first would refer to a collection's name
  catalog.insert({collectionName, Collection()});
  cout << "Collection " << collectionName << " added\n";
}


void addMemberToCollection(int id, const string& collectionName,
                           Library_t& library, Catalog_t& catalog) {
  // first find record with that id, get that title
  string title;
  for (auto& movie: library) {
    if (movie.second.id == id) {
      title = movie.first;
      break;
    }
  }
  // had bug here before, didn't put & after auto!!
  // then find collection object needed
  for (auto& collection: catalog) {
    if (collection.first == collectionName) {
      collection.second.listOfTitles.insert(title);
    }
  }
  cout << "Member " << id << " " << title << " added\n";
}


void modifyRatingForRecord(int id, int rating, Library_t& library) {
  // had a bug here without &, no & mean we get a copy of movie,
  // change of rating did not persist
  for (auto& movie: library) {
    if (movie.second.id == id) {
      movie.second.rating = rating;
      cout <<  "Rating for record " << movie.second.id << " changed to "
      << rating << "\n";
      return;
    }
  }
}


bool recordExistInCatalog(const string& title, const Library_t& library,
                          const Catalog_t& catalog) {
  // have to check every set for every collection in catalog
  for (auto& collection: catalog) {
    for (auto& title: collection.second.listOfTitles) {
      // use each title in set<string> to feed to library.find()
      auto it = library.find(title);
      if (it != library.end()) {
        return true;
      }
    }
  }
  return false;
}


void deleteRecord(const string& title, Library_t& library) {
  int id = library[title].id;
  library.erase(title);
  cout << "Record " << id << " " << title << " deleted\n";
}


void deleteCollection(const string& collectionName, Catalog_t& catalog) {
  auto it = catalog.find(collectionName);
  catalog.erase(it);
  cout << "Collection " << collectionName << " deleted\n";
}


void deleteMemberFromCollection(int id, const string& collectionName,
                                Library_t& library, Catalog_t& catalog) {
  // first find title, using id
  string title;
  for (auto& movie: library) {
    if (movie.second.id == id) {
      title = movie.first;
      break;
    }
  }
  // then find collection object needed using collectioName as key
  auto it = catalog.find(collectionName);
  // erase title from set inside the collection
  it->second.listOfTitles.erase(title);
  cout << "Member " << id << " " << title << " deleted\n";
}


void saveData(Library_t& library, Catalog_t& catalog) {
  // write libray and catalog data to named file
  string fileName;
  cin >> fileName;
  ofstream savingFile(fileName);
  if (savingFile.is_open()) {
    savingFile << library.size() << "\n";
    for (auto& movie: library) {
      savingFile << movie.second.id << " " << movie.second.medium << " " <<
      movie.second.rating << " " << movie.first << "\n";
    }
    savingFile << catalog.size() << "\n";
    for (auto& collection: catalog) {
      savingFile << collection.first << " " <<
      collection.second.listOfTitles.size() << "\n";
      // print titles in each collection
      for (auto& title: collection.second.listOfTitles) {
        savingFile << title << "\n";
      }
    }
    cout << "Data saved\n";
    savingFile.close();
    return;
  } // if savingFile is open
  cout << "Could not open file!\n";
  discardInput();
  savingFile.close();
}

// return value of 1 indicates the need to reset idCounter to 1
// otherwise, return value of 0 indicates normal execution, ++id
void restoreData(int& idCounter, Library_t& library, Catalog_t& catalog) {
  // restore library and catalog data from file
  string fileName;
  cin >> fileName;
  // use fileName as parameter to construct
  ifstream restoringFile(fileName);
  if (restoringFile.is_open()) {
    catalog.clear();
    library.clear();
    // ++idCounter inside loadLibrary() to ensure id is consistent
    if (!loadLibrary(idCounter, restoringFile, library)) {
      library.clear();
      idCounter = 1;
      cout << "Invalid data found in file!\n";
      discardInput();
      return;
      
    }
    if (!loadCatalog(restoringFile, library, catalog)) {
      catalog.clear();
      library.clear();
      idCounter = 1;
      cout << "Invalid data found in file!\n";
      discardInput();
      return;
    }
    cout << "Data loaded\n";
    restoringFile.close();
    return;
  }
  cout << "Could not open file!\n";
  discardInput();
  restoringFile.close();
}


void clearLibrary(int& idCounter, Library_t& library,
                  const Catalog_t& catalog) {
  // need to check if each collection in catalog is empty
  for (auto& collection: catalog) {
    if (!(collection.second.listOfTitles.empty())) {
      cout << "Cannot clear all records unless all collections are empty!\n";
      return;
    }
  }
  cout << "All records deleted\n";
  library.clear();
  // need to reset idCounter = 1 after clearing library
  idCounter = 1;
}

void clearCatalog(Catalog_t& catalog) {
  cout << "All collections deleted\n";
  catalog.clear();
}

// clearAll doesn't have error message
void clearAll(int& idCounter, Library_t& library, Catalog_t& catalog) {
  catalog.clear();
  library.clear();
  // also need to reset idCounter = 1 after clearing all
  idCounter = 1;
}


/***********************3rd level functions************************/


bool titleExist(const string& title, const Library_t& library) {
  auto it = library.find(title);
  if (it != library.end()) {
    return true;
  }
  return false;
}


bool idExist(int id, const Library_t& library) {
  for (auto& movie: library) {
    if (movie.second.id == id) {
      return true;
    }
  }
  return false;
}

bool collectionNameExist(const string& collectionName,
                         const Catalog_t& catalog) {
  auto it = catalog.find(collectionName);
  if (it != catalog.end()) {
    return true;
  }
  return false;
}


bool recordExistInCollection(int id, const string& collectionName,
                             const Library_t& library,
                             const Catalog_t& catalog) {
  // first find the collection we care about in catalog
  auto it = catalog.find(collectionName);
  // iterate through set<string> in single collection object
  // (it->second) refers to a Collection object
  for (auto& title: (it->second).listOfTitles) {
    // by using title as key, access each record object in library
    auto iter = library.find(title);
    //(iter->second) refers to a Movie object
    if ((iter->second).id == id) {
      // id matches, record already in that specific collection
      return true;
    }
  }
  return false;
}


// prints each movie's, a.k.a record's, information
void printOneRecord(const string& title, const Library_t& library) {
  auto it = library.find(title);
  cout << it->second.id << ": " << it->second.medium << " ";
  // need additional checking for rating, if 0, ouput "u"
  if (it->second.rating == 0) {
    cout << "u ";
  }
  else {
    cout << it->second.rating << " ";
  }
  cout << title << "\n";
}


void printOneCollection(const string& collectionName,
                        const Library_t& library,
                        const Catalog_t& catalog) {
  // find collection, we error-checked that it existed before
  auto it = catalog.find(collectionName);
  cout << "Collection " << collectionName << " contains:";
  // (it->second) refers a Collection object, which contains
  // a set<string>
  if ((it->second).listOfTitles.empty()) {
    cout << " None\n";
    return;
  }
  cout << "\n";
  for (auto& title: (it->second).listOfTitles) {
    printOneRecord(title, library);
  }
}


bool loadLibrary(int& idCounter, ifstream& restoringFile, Library_t& library) {
  int totalMovies = 0;
  restoringFile >> totalMovies;
  if (!restoringFile) {
    return false;
  }
  // total movies cannot be negative
  if (totalMovies < 0) {
    return false;
  }
  // corner case
  if (totalMovies == 0) {
    idCounter = 1;
    return true;
  }

  int id = 0;
  string medium;
  int rating = 0;
  string title;
  // can safely assume highest id = 1, invariant of project
  int highestId = 1;
  
  for (int i = 0; i < totalMovies; i++) {
    // id cannot be 0 or -ve
    restoringFile >> id;
    if (!restoringFile) {
      return false;
    }
    if (id < 0) {
      return false;
    }
    if (id > highestId) {
      // update highestId through iterator if id read > highestId
      highestId = id;
    }
    // no need to check for medium
    restoringFile >> medium >> rating;
    if (!restoringFile) {
      return false;
    }
    if (rating < 0) {
      return false;
    }
    getline(restoringFile, title);
    // feed title to compactEmbeddedTitle cause of leading space!
    title = compactEmbeddedTitle(title);
    // use the other constructor here! need rating param
    library.insert({title, Movie(medium, rating, id)});
  } // for
  
  // safety
  if (!restoringFile) {
    return false;
  }
  //modify idCounter here
  idCounter = highestId + 1;
  return true;
}

bool loadCatalog(ifstream& restoringFile, const Library_t& library,
                 Catalog_t& catalog) {
  int totalCollections = 0;
  restoringFile >> totalCollections;
  if (!restoringFile) {
    return false;
  }
  // total collections cannot be negative
  if (totalCollections < 0) {
    return false;
  }
  // corner case
  if (totalCollections == 0) {
    return true;
  }
  string collectionName;
  int titlesInCollection = 0;
  string title;
  string garbage;
  for (int i = 0; i < totalCollections; i++) {
    
    restoringFile >> collectionName;
    if (!restoringFile) {
      return false;
    }
    restoringFile >> titlesInCollection;
    if (titlesInCollection < 0) {
      return false;
    }
    if (!restoringFile) {
      return false;
    }
    // insert collection into catalog, one by one
    catalog.insert({collectionName, Collection()});
    // need to getline here, cause mixed cin with geline next
    // note that restoringFile.clear() did not work!
    getline(restoringFile, garbage);
    for (int j = 0; j < titlesInCollection; j++) {
      getline(restoringFile, title);
      if (!restoringFile) {
        return false;
      }
      // it points to a specific movie in library
      auto it = library.find(title);
       // check that title must be in library
      if (it == library.end()) {
        return false;
      }
      // know that titles must be in library now
      // iter points to a Collection object
      auto iter = catalog.find(collectionName);
      // build listOfTitles in that specific collection
      iter->second.listOfTitles.insert(title);
    } // inner for, loop through each title in each collection
  } // outer for, loop through each collection
  
  // safety
  if (!restoringFile) {
    return false;
  }
  return true;
}



/* SO PROUD OF MYSELF to come out with on 10th Jan at 1.30am!
   Note: need to getline before calling this function
   If badTitle is empty, string returned is empty!
   If badTitle is whitespace(s), return an empty string too!
 */

string compactEmbeddedTitle(const string& badTitle) {
  // first check if empty string
  if (badTitle.empty()) {
    // user entered dr<enter>/ar<enter>/fr<enter>
    // return itself, which is empty string
    return badTitle;
  }
  
  // check if we have "<whitespace>...<whitespace>", did not consider
  // this before!
  if (onlyWhiteSpaceTitleExist(badTitle)) {
    string emptyString;
    return emptyString;
  }
  

  bool redundantWhiteSpace = false;
  deque<char> title;
  for (auto letter: badTitle) {
    //first check for first whitespace
    if (isspace(letter)) {
      // check if this whitespace is FIRST-ENCOUNTERED
      // this applies to leading, between, trailing whitespaces
      if (!redundantWhiteSpace) {
        // first time encountering whitespace, need this space first
        title.push_back(letter);
        redundantWhiteSpace = true;
        
      }
      else {
        // don't want redundundant whitespace
        continue;
      }
    } // if whitespace
    else if (letter == '\n') {
      break;
    } // if newline, means quit
    else {
      // dealing with useful char here
      // HAD A BUG here before, forgot to reset bool to be false
      // when encounter new useful char in a word!!!
      redundantWhiteSpace = false;
      title.push_back(letter);
    }
  } // for
  
  // NEED TO CHECK if first and last element has whitespace!
  // If there is, we dont want them, as they represent leading and
  // trailing whitespaces!
  if (isspace(title.front())) {
    title.pop_front();
  }
  if (isspace(title.back())) {
    title.pop_back();
  }
  string compactTitle;
  // copy over deque<char> into string, concatenate the chars
  for (auto letter: title) {
    compactTitle += letter;
  }
  return compactTitle;
}


bool onlyWhiteSpaceTitleExist(const string& badTitle) {
  for (auto letter: badTitle) {
    // encounted a useful char, impossible to have "      " now
    if (!isspace(letter)) {
      return false;
    }
  }
  return true;
}






