//
//  MediaManager.h
//  Project0
//
//  Created by Jay Chow on 1/5/17.
//  Copyright © 2017 Jay Chow. All rights reserved.
//

#ifndef MEDIA_MANAGER_H
#define MEDIA_MANAGER_H

#include <stdio.h>
#include <string>
#include <map>
#include <set>
#include <fstream>

/* Note: No limit on # of possible movies a.k.a records. Movies can be
 * created, destroyed, or modified. Record can be referenced by title or id #.
 */

// title is stored as key in map<string, Movie>
struct Movie {
  // DVD, VHS, etc
  std::string medium;
  // on 1-5 scale
  int rating;
  // id # for identification, automatically assigned by program
  int id;
  
  Movie(std::string mediumIn, int idIn)
  : medium(mediumIn), rating(0), id(idIn) {}
  // this other constructor is ONLY needed for restore, for rating!
  Movie(std::string mediumIn, int ratingIn, int idIn)
  : medium(mediumIn), rating(ratingIn), id(idIn) {}
  Movie() {}
  
};

/* Note: Collections can be created or destroyed, and records added or removed
 * from a collection. No limit on # of collections, nor on # of records in a
 * collection. A record can be in any number of collections, or not be in any.
 * A collection can be destroyed by specificing its name. User can add/remove 
 * record to/from collection by specifying the collection name and record ID #.
 */


// name is stored as key in map<string, Collection>
struct Collection {
  // set maintains uniqueness, and ordered by type
  std::set<std::string> listOfTitles;
};


// more meaningful, short and information hiding
// string refers to title, Movie is the record itself
typedef std::map<std::string, Movie> Library_t;
// string refers to name, Collection refers to collection object
typedef std::map<std::string, Collection> Catalog_t;


/************************Top level functions***********************/
void findRecord(char objectLetter, const Library_t& library);

void print(char objectLetter, const Library_t& library,
           const Catalog_t& catalog);

void modifyRating(char objectLetter, Library_t& library);

void add(char objectLetter, Library_t& library, Catalog_t& catalog,
         int& idCounter);

void deletion(char objectLetter, Library_t& library, Catalog_t& catalog);

void clear(char objectLetter, int& idCounter, Library_t& library,
           Catalog_t& catalog);

void save(char objectLetter, Library_t& library, Catalog_t& catalog);

// if restoreData returned false, reset idCounter = 1
void restore(char objectLetter, int& idCounter, Library_t& library,
             Catalog_t& catalog);

bool quit(char objectLetter, Library_t& library, Catalog_t& catalog);

void discardInput();



/**********************2nd level functions*************************/
bool badIntegerExist(int id);

bool ratingOutOfRange(int rating);

// mr command
bool modifyRatingOutOfRange(int rating);

void printRecordById(int id, const Library_t& library);

void addRecord(const std::string& medium, const std::string& title,
               Library_t& library, int& idCounter);

bool collectionNameExist(const std::string& collectionName,
                         const Catalog_t& catalog);

void printLibrary(const Library_t& library);

void printCatalog(const Library_t& library, const Catalog_t& catalog);

void printMemoryAllocation(const Library_t& library, const Catalog_t& catalog);

void addCollection(const std::string& collectionName, Catalog_t& catalog);

void addMemberToCollection(int id, const std::string& collectionName,
                           Library_t& library, Catalog_t& catalog);

void modifyRatingForRecord(int id, int rating, Library_t& library);

bool recordExistInCatalog(const std::string& title, const Library_t& library,
                          const Catalog_t& catalog);

void deleteRecord(const std::string& title, Library_t& library);

void deleteCollection(const std::string& collectionName, Catalog_t& catalog);

void deleteMemberFromCollection(int id, const std::string& collectionName,
                                Library_t& library, Catalog_t& catalog);

void saveData(Library_t& library, Catalog_t& catalog);

// return false means invalid data found
void restoreData(int& idCounter, Library_t& library, Catalog_t& catalog);

void clearLibrary(int& idCounter, Library_t& library, const Catalog_t& catalog);

void clearCatalog(Catalog_t& catalog);
                   
void clearAll(int& idCounter, Library_t& library, Catalog_t& catalog);



/***********************3rd level functions************************/
bool titleExist(const std::string& title,
                const Library_t& library);

bool idExist(int id, const Library_t& library);

bool collectionNameExist(const std::string& collectionName,
                         const Catalog_t& catalog);

bool recordExistInCollection(int id, const std::string& collectionName,
                             const Library_t& library,
                             const Catalog_t& catalog);

void printOneRecord(const std::string& title, const Library_t& library);

void printOneCollection(const std::string& collectionName,
                        const Library_t& library,
                        const Catalog_t& catalog);

// if returned true, ++idCounter for consistency
bool loadLibrary(int& idCounter, std::ifstream& restoringFile, Library_t& library);

bool loadCatalog(std::ifstream& restoringFile, const Library_t& library,
                 Catalog_t& catalog);

// returns compacted string after dealing with leading,
// embedded, or trailing whitespaces
std::string compactEmbeddedTitle(const std::string& badTitle);

bool onlyWhiteSpaceTitleExist(const std::string& badTitle);
// Third level functions


#endif /* MEDIA_MANAGER_H */
