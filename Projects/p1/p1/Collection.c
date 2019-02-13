//
//  Collection.c
//  Project1
//
//  Created by Jay Chow on 1/19/17.
//  Copyright © 2017 Jay Chow. All rights reserved.
//

#include "Collection.h"
#include "Record.h"
#include "Utility.h"
#include "Ordered_container.h"
#include <stdlib.h>
#include <string.h>

// wrapper function, spec requires 2 wrappers
static inline void call_print_Record(void* v_ptr);
static inline void call_save_Record(void* v_ptr, void* f_ptr);

/* a Collection contains a pointer to a C-string name and a 
 container that holds pointers to Records - the members. */
struct Collection {
  char* name;
  struct Ordered_container* members;
};

/* Create a Collection object. This is the only function that
 allocates memory for a Collection and the contained data. */
struct Collection* create_Collection(const char* name) {
  struct Collection* collection_ptr = malloc(sizeof(struct Collection));
  check_bad_allocation(collection_ptr);
  // name is pointer to c-string
  collection_ptr->name = c_string_allocation(name);
  // order_Record_title is casted function address here
  collection_ptr->members =
  OC_create_container((OC_comp_fp_t)order_Record_title);
  return collection_ptr;
}

/* Destroy a Collection object.
 This is the only function that frees the memory for a Collection
 and the contained data. It discards the member list,
 but of course does not delete the Records themselves. */
void destroy_Collection(struct Collection* collection_ptr) {
  c_string_deallocation(collection_ptr->name);
  // members is a pointer to OC
  OC_destroy_container(collection_ptr->members);
  // need to de-allocate name, pointer to c-string on heap too
  free(collection_ptr);
  collection_ptr = NULL;
}

/* Return tthe collection name. */
const char* get_Collection_name(const struct Collection* collection_ptr) {
  return collection_ptr->name;
}

/* return non-zero if there are no members, 0 if there are members*/
int Collection_empty(const struct Collection* collection_ptr) {
  if(OC_empty(collection_ptr->members)) {
    return 1;
  }
  return 0;
}

/* Add a member; return non-zero and do nothing if already present*/
int add_Collection_member(struct Collection* collection_ptr,
                          const struct Record* record_ptr) {
  if (is_Collection_member_present(collection_ptr, record_ptr)) {
    return 1;
  }
  OC_insert(collection_ptr->members, record_ptr);
  return 0;
}

/* Return non-zero if the record is a member, zero if not. */
int is_Collection_member_present(const struct Collection* collection_ptr,
                                 const struct Record* record_ptr) {
  if (OC_find_item(collection_ptr->members, record_ptr)) {
    return 1;
  }
  return 0;
}

/* Remove a member; return non-zero if not present, zero if was
 present. */
int remove_Collection_member(struct Collection* collection_ptr,
                             const struct Record* record_ptr) {
  if (is_Collection_member_present(collection_ptr, record_ptr)) {
    void* item_ptr = OC_find_item(collection_ptr->members, record_ptr);
    OC_delete_item(collection_ptr->members, item_ptr);
    return 0;
  }
  // member record not present in Collection object
  return 1;
}

/* Print the data in a Collection. */
void print_Collection(const struct Collection* collection_ptr) {
  printf("Collection %s contains:", collection_ptr->name);
  if (OC_empty(collection_ptr->members)) {
    printf(" None\n");
    return;
  }
  printf("\n");
  OC_apply(collection_ptr->members, call_print_Record);
}

/* Write the data in a Collection to a file. */
void save_Collection(const struct Collection* collection_ptr,
                     FILE* outfile) {
  // first print collection name and # of records in that collection
  fprintf(outfile, "%s %d\n", collection_ptr->name,
          OC_get_size(collection_ptr->members));
  
  OC_apply_arg(collection_ptr->members,
               (OC_apply_arg_fp_t)call_save_Record, outfile);
}

/* Read a Collection's data from a file stream, create the data
 object and return a pointer to it, NULL if invalid data discovered
 in file. No check made for whether the Collection already exists
 or not. */
struct Collection* load_Collection(FILE* input_file,
                                const struct Ordered_container* records) {
  // note that this includes space for the null byte
  char collection_name[MAX_COLLECTION_NAME_SIZE];
  int collection_size = 0;
  // format string concatenates
  if (fscanf(input_file, MAX_COLLECTION_NAME_FORMAT "%d",
             collection_name, &collection_size) != 2) {
    return NULL;
  }
  if (collection_size < 0) {
    return NULL;
  }
  // create Collection, feed in name
  struct Collection* collection_ptr = create_Collection(collection_name);
  char title[MAX_TITLE_SIZE];
  // suck out newline
  fgetc(input_file);
  for (int i = 0; i < collection_size; i++) {
    fgets(title, MAX_TITLE_SIZE, input_file);
    // need to get rid of newline at the end
    title[(int)strlen(title) - 1] = '\0';
    void* item_ptr = OC_find_item_arg(records, title,
                     (OC_find_item_arg_fp_t)find_Record_title);
    // first check if title exist in library
    if (item_ptr) {
      if (!OC_insert(collection_ptr->members, OC_get_data_ptr(item_ptr))) {
        return NULL;
      }
    }
    else {
      return NULL;
    }
  } // for
  return collection_ptr;
}

// want to keep same structure as what OC_apply wants
static inline void call_print_Record(void* v_ptr) {
  // one level of indirection
  struct Record* rec_ptr = (struct Record*) v_ptr;
  print_Record(rec_ptr);
}

// want to keep same structure as what OC_apply_arg wants
static inline void call_save_Record(void* v_ptr, void* f_ptr) {
  // need to cast both parameters
  struct Record* rec_ptr = (struct Record*) v_ptr;
  FILE* file_ptr = (FILE*)f_ptr;
  /* calls save_Record_title instead of save_Record cause we just
   need titles to output to file when saving collection */
  save_Record_title(rec_ptr, file_ptr);
}


