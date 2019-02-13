//
//  Utility.c
//  Project1
//
//  Created by Jay Chow on 1/22/17.
//  Copyright © 2017 Jay Chow. All rights reserved.
//

#include "Utility.h"
#include "p1_globals.h"
#include "Ordered_container.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/* check ptr returned from malloc is NULL, indicating memory
 is unsuccessful */
void check_bad_allocation(void* ptr) {
  if (ptr == NULL) {
    printf("No free storage space\n");
    exit(1);
  }
}

/* returns a pointer to the newly allocated c-string on heap, and
 updates global variable g_string_memory accordingly */
char* c_string_allocation(const char* string_in) {
  /* need to consider the null byte, strlen returns length that
   is 1 less than the real length(with null byte) */
  char* str_dest = malloc((strlen(string_in) * sizeof(char)) + 1);
  check_bad_allocation(str_dest);
  // now safe to copy string_in into str_dest
  strcpy(str_dest, string_in);
  g_string_memory += (int)strlen(str_dest) + 1;
  return str_dest;
}

/* deallocates memory for c-string on heap, then updates global
 variable g_string_memory accordingly */
void c_string_deallocation(char* string_in) {
  g_string_memory -= ((int)strlen(string_in) + 1);
  // free the memory through pointer
  free(string_in);
  // safety
  string_in = NULL;
}

/* comparison function to feed during construction of Library
 ordered by record ids */
int order_Record_id(const struct Record* rec_ptr_lhs,
                    const struct Record* rec_ptr_rhs) {
  if (get_Record_ID(rec_ptr_lhs) < get_Record_ID(rec_ptr_rhs)) {
    return -1;
  }
  else if (get_Record_ID(rec_ptr_lhs) > get_Record_ID(rec_ptr_rhs)) {
    return 1;
  }
  else {
    return 0;
  }
}

int find_Record_id(const int* id_ptr, const struct Record* rec_ptr) {
  return ((*id_ptr) - get_Record_ID(rec_ptr));
}


/* comparison function to feed during construction of ordered 
 container in each Collection object, or construction of Library */
int order_Record_title(const struct Record* rec_ptr_lhs,
                       const struct Record* rec_ptr_rhs) {
  /* returns < 0 if str_lhs < str_rhs, 0 if str_lhs == str_rhs, 
   > 0 if str_lhs > str_rhs */
  return strcmp(get_Record_title(rec_ptr_lhs),
                get_Record_title(rec_ptr_rhs));
}

int find_Record_title(const char* title, const struct Record* rec_ptr) {
  return strcmp(title, get_Record_title(rec_ptr));
}


/* comparison function to feed during construction of Catalog */
int order_Collection_name(const struct Collection* collection_ptr_lhs,
                          const struct Collection* collection_ptr_rhs) {
  return strcmp(get_Collection_name(collection_ptr_lhs),
                get_Collection_name(collection_ptr_rhs));
  
}

int find_Collection_name(const char* name, const struct Collection* col_ptr) {
  return strcmp(name, get_Collection_name(col_ptr));
}

/* for use specifically in save_collection function */
void save_Record_title(const struct Record* record_ptr, FILE* outfile) {
  fprintf(outfile, "%s\n", get_Record_title(record_ptr));
}


