//
//  Record.c
//  Project1
//
//  Created by Jay Chow on 1/19/17.
//  Copyright © 2017 Jay Chow. All rights reserved.
//

#include "Record.h"
#include "Utility.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


/* a Record contains an int ID, rating, and pointers to C-strings for the title and medium */
struct Record {
  char* title;
  int ID;
  char* medium;
  int rating;
};

/* Create a Record object, giving it the supplied ID number.
 The function that allocates dynamic memory for a Record and the
 contained data. The rating is set to 0. */
struct Record* create_Record(int ID_number, const char* medium,
                             const char* title) {
  struct Record* rec_ptr = malloc(sizeof(struct Record));
  check_bad_allocation(rec_ptr);
  rec_ptr->title = c_string_allocation(title);
  rec_ptr->ID = ID_number;
  rec_ptr->medium = c_string_allocation(medium);
  rec_ptr->rating = 0;
  return rec_ptr;
}

/* Destroy a Record object
 This is the only function that frees the memory for a Record
 and the contained data. */
void destroy_Record(struct Record* record_ptr) {
  c_string_deallocation(record_ptr->title);
  c_string_deallocation(record_ptr->medium);
  free(record_ptr);
  // safety
  record_ptr = NULL;
}

/* Accesssors */

/* Return the ID number. */
int get_Record_ID(const struct Record* record_ptr) {
  return record_ptr->ID;
}

/* Get the title pointer. */
const char* get_Record_title(const struct Record* record_ptr) {
  return record_ptr->title;
}

/* Set the rating. */
void set_Record_rating(struct Record* record_ptr, int new_rating) {
  record_ptr->rating = new_rating;
}

/* Print a Record data items to standard output with a final
 \n character. Output order is ID number followed by a ':' then
 medium, rating, title, separated by one space. If the rating is
 zero, a 'u' is printed instead of the rating. */
void print_Record(const struct Record* record_ptr) {
  printf("%d: %s ", record_ptr->ID, record_ptr->medium);
  // need additional checking for rating, if 0, ouput "u"
  if (record_ptr->rating == 0) {
    printf("u ");
  }
  else {
    printf("%d ", record_ptr->rating);
  }
  printf("%s\n", record_ptr->title);
}

/* Write a Record to a file stream with a final \n character.
 Output order is ID number, medium, rating, title */
void save_Record(const struct Record* record_ptr, FILE* outfile) {
  fprintf(outfile, "%d %s %d %s\n", record_ptr->ID, record_ptr->medium,
          record_ptr->rating, record_ptr->title);
}

/* Read a Record's data from a file stream, create the data object
 and return a pointer to it, NULL if invalid data discovered in file.
 No check made in this function for whether the Record already
 exists or not. */
struct Record* load_Record(FILE* infile) {
  int id = 0;
  int rating = 0;
  // note that this includes space for the null byte
  char medium[MAX_MEDIUM_SIZE];
  char title[MAX_TITLE_SIZE];
  // format string concatenates
  if (fscanf(infile, "%d" MAX_MEDIUM_FORMAT "%d",
             &id, medium, &rating) != 3) {
    return NULL;
  }
  if (id < 0 || rating < 0) {
    return NULL;
  }
  // use fgetchar to suck out leading whitespace
  fgetc(infile);
  fgets(title, MAX_TITLE_SIZE, infile);
  // get rid of newline char at end, index is always strlen - 1
  title[(int)strlen(title) - 1] = '\0';
  // need to change ratings from rating input from file
  struct Record* rec_ptr = create_Record(id, medium, title);
  set_Record_rating(rec_ptr, rating);
  return rec_ptr;
}
