//
//  main.cpp
//  Project0
//
//  Created by Jay Chow on 1/5/17.
//  Copyright © 2017 Jay Chow. All rights reserved.
//

#include "MediaManager.h"
#include <iostream>
using namespace std;

/* Note: Good function tree, fundamental programming technique for organized
 * code. Main contains top level loop that reads and executes each input
 * command. Does this by reading two command characters and then calling a 
 * separate function, one for each command, that peforms the actual work of that
 * command. Each command function must be implemented by calling a set of helper
 * functions. Reuse these helper functions across commands if possible.
 */

int main(int argc, const char * argv[]) {
  // {f,p,m,a,d,c,s,r}
  char actionLetter;
  // {r,c,m,L,C,A,a}
  char objectLetter;
  Catalog_t catalog;
  Library_t library;
  // fresh record starts at id = 1
  int idCounter = 1;
  cout << "\nEnter command: ";
  /* program prompts for two-letter command, followed by input params depending
   * on specific command. Program reads command, applies error checking,
   * executes command, then re-prompts user for next command.
   */
  while (cin >> actionLetter >> objectLetter) {
    switch (actionLetter) {
      // find (records only)
      case 'f':
        findRecord(objectLetter, library);
        break;
      // print
      case 'p':
        print(objectLetter, library, catalog);
        break;
      // modify
      case 'm':
        modifyRating(objectLetter, library);
        break;
      // add
      case 'a':
        add(objectLetter, library, catalog, idCounter);
        break;
      // delete
      case 'd':
        deletion(objectLetter, library, catalog);
        break;
      // clear
      case 'c':
        // need to reset idCounter = 1 after cA or cL!
        clear(objectLetter, idCounter, library, catalog);
        break;
      // save
      case 's':
        save(objectLetter, library, catalog);
        break;
      // restore
      case 'r':
        // need to modify idCounter
        restore(objectLetter, idCounter, library, catalog);
        break;
      // quit (requires qq to quit program)
      case 'q':
        if (quit(objectLetter, library, catalog)) {
          return 0;
        }
        break;
      default:
        cout << "Unrecognized command!\n";
        discardInput();
        break;
    } // switch
    cout << "\nEnter command: ";
  } // while
  
  return 0;
}
