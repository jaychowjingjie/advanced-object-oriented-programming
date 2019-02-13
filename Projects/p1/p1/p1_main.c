//
//  main.c
//  Project1
//
//  Created by Jay Chow on 1/18/17.
//  Copyright © 2017 Jay Chow. All rights reserved.
//
#include "Ordered_container.h"
#include "p1_globals.h"
#include "Utility.h"
#include <string.h>
#include <ctype.h>
#include <stdio.h>

/* Note: Good function tree, fundamental programming technique for 
 organized code. Main contains top level loop that reads and executes
 each input command. Does this by reading two command characters and 
 then calling a separate function, one for each command, that peforms 
 the actual work of that command. Each command function must be 
 implemented by calling a set of helper functions. Reuse helper 
 functions across commands if possible.
 */

typedef struct  Ordered_container*  OC_ptr_t;

/************************Top level functions***********************/
static void find_record(const OC_ptr_t);
static void print_record(const OC_ptr_t);
static void print_collection(const OC_ptr_t);
static void print_library(const OC_ptr_t);
static void print_catalog(const OC_ptr_t);
static void print_allocation(const OC_ptr_t, const OC_ptr_t );
static void add_record(OC_ptr_t , OC_ptr_t , int* );
static void add_collection(OC_ptr_t );
static void add_member(OC_ptr_t , OC_ptr_t);
static void modify_rating(OC_ptr_t );
static void delete_record(OC_ptr_t , OC_ptr_t , OC_ptr_t);
static void delete_collection(OC_ptr_t );
static void delete_member(OC_ptr_t , OC_ptr_t );
static void clear_library(OC_ptr_t , OC_ptr_t , OC_ptr_t , int* );
static void clear_catalog(OC_ptr_t );
static void clear_all_data(OC_ptr_t , OC_ptr_t, OC_ptr_t , int* );
static void save_all_data(const OC_ptr_t , const OC_ptr_t );
static void restore_all_data(OC_ptr_t , OC_ptr_t , OC_ptr_t , int* );

/**********************2nd level functions*************************/
static void clear_containers(OC_ptr_t , OC_ptr_t , OC_ptr_t );
static char* compact_title(char* str);
static void discard_input(void);

int main(int argc, const char * argv[]) {
  // {f,p,m,a,d,c,s,r}
  char actionLetter;
  // {r,c,m,L,C,A,a}
  char objectLetter;
  OC_ptr_t lib_id_ptr =
  OC_create_container((OC_comp_fp_t)order_Record_id);
  OC_ptr_t lib_title_ptr =
  OC_create_container((OC_comp_fp_t)order_Record_title);
  OC_ptr_t cat_name_ptr =
  OC_create_container((OC_comp_fp_t)order_Collection_name);
  // fresh record starts at id 1
  int id_counter = 1;
  printf("\nEnter command: ");
  while (scanf(" %c %c", &actionLetter, &objectLetter )) {
    switch (actionLetter) {
      case 'f': {
        switch (objectLetter) {
          case 'r':
            find_record(lib_title_ptr);
            break;
          default:
            printf("Unrecognized command!\n");
            discard_input();
            break;
        }
        break;
      }
      case 'p': {
        switch (objectLetter) {
          case 'r':
            print_record(lib_id_ptr);
            break;
          case 'c':
            print_collection(cat_name_ptr);
            break;
          case 'L':
            print_library(lib_title_ptr);
            break;
          case 'C':
            print_catalog(cat_name_ptr);
            break;
          case 'a':
            print_allocation(lib_title_ptr, cat_name_ptr);
            break;
          default:
            printf("Unrecognized command!\n");
            discard_input();
            break;
        }
        break;
      }
      case 'a': {
        switch (objectLetter) {
          case 'r':
            add_record(lib_title_ptr, lib_id_ptr, &id_counter);
            break;
          case 'c':
            add_collection(cat_name_ptr);
            break;
          case 'm':
            add_member(lib_id_ptr, cat_name_ptr);
            break;
          default:
            printf("Unrecognized command!\n");
            discard_input();
            break;
        }
        break;
      }
      case 'm': {
        switch (objectLetter) {
          case 'r':
            modify_rating(lib_id_ptr);
            break;
          default:
            printf("Unrecognized command!\n");
            discard_input();
            break;
        }
        break;
      }
      case 'd': {
        switch (objectLetter) {
          case 'r':
            delete_record(lib_title_ptr, lib_id_ptr, cat_name_ptr);
            break;
          case 'c':
            delete_collection(cat_name_ptr);
            break;
          case 'm':
            delete_member(lib_id_ptr, cat_name_ptr);
            break;
          default:
            printf("Unrecognized command!\n");
            discard_input();
            break;
        }
        break;
      }
      case 'c': {
        switch (objectLetter) {
          case 'L':
            // need to reset id_counter = 1 here
            clear_library(lib_title_ptr, lib_id_ptr, cat_name_ptr,
                          &id_counter);
            break;
          case 'C':
            clear_catalog(cat_name_ptr);
            break;
          case 'A':
            // also need to reset id_counter = 1
            clear_all_data(lib_title_ptr, lib_id_ptr, cat_name_ptr,
                           &id_counter);
            break;
          default:
            printf("Unrecognized command!\n");
            discard_input();
            break;
        }
        break;
      }
      case 's': {
        switch (objectLetter) {
          case 'A':
            // 2 containers
            save_all_data(lib_title_ptr, cat_name_ptr);
            break;
          default:
            printf("Unrecognized command!\n");
            discard_input();
            break;
        }
        break;
      }
      case 'r': {
        switch (objectLetter) {
          case 'A':
            // restore all data structures
            restore_all_data(lib_title_ptr, lib_id_ptr, cat_name_ptr,
                             &id_counter);
            break;
          default:
            printf("Unrecognized command!\n");
            discard_input();
            break;
        }
        break;
      }
      case 'q': {
        switch (objectLetter) {
          case 'q':
            clear_containers(lib_title_ptr, lib_id_ptr, cat_name_ptr);
            OC_destroy_container(cat_name_ptr);
            OC_destroy_container(lib_title_ptr);
            OC_destroy_container(lib_id_ptr);
            printf("All data deleted\n");
            printf("Done\n");
            return 0;
          default:
            printf("Unrecognized command!\n");
            discard_input();
            break;
        }
        break;
      }
      default:
        printf("Unrecognized command!\n");
        discard_input();
        break;
    } // outer switch
    printf("\nEnter command: ");
  } // while
  return 0;
}

/************************Top level functions***********************/

// fr <title>, note find_Record is from Record module
static void find_record(const OC_ptr_t lib_title_ptr) {
  char bad_title[MAX_TITLE_SIZE];
  // note that newline is part of string
  fgets(bad_title, MAX_TITLE_SIZE, stdin);
  const char* title = compact_title(bad_title);
  if (!(int)strlen(title)) {
    printf("Could not read a title!\n");
    return;
  }
  void* rec_ptr = OC_find_item_arg(lib_title_ptr, title, (OC_find_item_arg_fp_t)find_Record_title);
  if (!rec_ptr) {
    printf("No record with that title!\n");
    return;
  }
  print_Record(OC_get_data_ptr(rec_ptr));
}

// pr <ID>
static void print_record(const OC_ptr_t lib_id_ptr) {
  int id = 0;
  if (scanf("%d", &id) != 1) {
    printf("Could not read an integer value!\n");
    discard_input();
    return;
  }
  void* rec_ptr = OC_find_item_arg(lib_id_ptr, &id,
                   (OC_find_item_arg_fp_t)find_Record_id);
  if (!rec_ptr) {
    printf("No record with that ID!\n");
    discard_input();
    return;
  }
  print_Record(OC_get_data_ptr(rec_ptr));
}

// pc <collection_name>
static void print_collection(const OC_ptr_t cat_name_ptr) {
  char collection_name[MAX_COLLECTION_NAME_SIZE];
  scanf("%s", collection_name);
  void* collection_ptr =
  OC_find_item_arg(cat_name_ptr, collection_name,
                   (OC_find_item_arg_fp_t)find_Collection_name);

  // check name exist in catalog from collection_ptr returned
  if (!collection_ptr) {
    printf("No collection with that name!\n");
    discard_input();
    return;
  }
  print_Collection(OC_get_data_ptr(collection_ptr));
}

// pL
static void print_library(const OC_ptr_t lib_title_ptr) {
  if (OC_empty(lib_title_ptr)) {
    printf("Library is empty\n");
    return;
  }
  printf("Library contains %d records:\n", OC_get_size(lib_title_ptr));
  OC_apply(lib_title_ptr, (OC_apply_fp_t)print_Record);
}

// pC
static void print_catalog(const OC_ptr_t cat_name_ptr) {
  if (OC_empty(cat_name_ptr)) {
    printf("Catalog is empty\n");
    return;
  }
  printf("Catalog contains %d collections:\n", OC_get_size(cat_name_ptr));
  OC_apply(cat_name_ptr, (OC_apply_fp_t)print_Collection);
}

// pa
static void print_allocation(const OC_ptr_t lib_title_ptr,
                             const OC_ptr_t cat_name_ptr) {
  printf("Memory allocations:\n");
  printf("Records: %d\n", OC_get_size(lib_title_ptr));
  printf("Collections: %d\n", OC_get_size(cat_name_ptr));
  printf("Containers: %d\n", g_Container_count);
  printf("Container items in use: %d\n", g_Container_items_in_use);
  printf("Container items allocated: %d\n", g_Container_items_allocated);
  printf("C-strings: %d bytes total\n", g_string_memory);
}

// ar <medium> <title>
static void add_record(OC_ptr_t lib_title_ptr, OC_ptr_t lib_id_ptr,
                       int* id_count_ptr) {
  char medium[MAX_MEDIUM_SIZE];
  scanf(MAX_MEDIUM_FORMAT, medium);
  char bad_title[MAX_TITLE_SIZE];
  fgets(bad_title, MAX_TITLE_SIZE, stdin);
  const char* title = compact_title(bad_title);
  if (!(int)strlen(title)) {
    printf("Could not read a title!\n");
    return;
  }
  // need to insert into 2 containers
  void* rec_ptr = create_Record(*id_count_ptr, medium, title);
  if (!OC_insert(lib_title_ptr, rec_ptr)) {
    destroy_Record(rec_ptr);
    printf("Library already has a record with this title!\n");
    return;
  }
  
  OC_insert(lib_id_ptr, rec_ptr);
  printf("Record %d added\n", (*id_count_ptr));
  // increments id_count by 1 after each new record
  (*id_count_ptr) += 1;
}

// ac <collection_name>
static void add_collection(OC_ptr_t cat_name_ptr) {
  char collection_name[MAX_COLLECTION_NAME_SIZE];
  scanf(MAX_COLLECTION_NAME_FORMAT, collection_name);
  struct Collection* collection_ptr = create_Collection(collection_name);
  void* item_ptr = OC_find_item_arg(cat_name_ptr, collection_name, (OC_find_item_arg_fp_t)find_Collection_name);
  
  
  if(item_ptr) {
    printf("Catalog already has a collection with this name!\n");
    discard_input();
    return;
  }
  OC_insert(cat_name_ptr, collection_ptr);
  printf("Collection %s added\n", collection_name);
}

// am <collection_name> <id>
static void add_member(OC_ptr_t lib_id_ptr,
                       OC_ptr_t cat_name_ptr) {
  char collection_name[MAX_COLLECTION_NAME_SIZE];
  scanf(MAX_COLLECTION_NAME_FORMAT, collection_name);
  
  void* item_ptr_col =
  OC_find_item_arg(cat_name_ptr, collection_name,
                   (OC_find_item_arg_fp_t)find_Collection_name);

  // first check collection has that name
  if (!item_ptr_col) {
    printf("No collection with that name!\n");
    discard_input();
    return;
  }
  int id = 0;
  if (scanf("%d", &id) != 1) {
    printf("Could not read an integer value!\n");
    discard_input();
    return;
  }
  void* item_ptr_rec = OC_find_item_arg(lib_id_ptr, &id,
                   (OC_find_item_arg_fp_t)find_Record_id);
  if (!item_ptr_rec) {
    printf("No record with that ID!\n");
    discard_input();
    return;
  }
  struct Collection* col_ptr = OC_get_data_ptr(item_ptr_col);
  struct Record* rec_ptr = OC_get_data_ptr(item_ptr_rec);
  // returns 1 if member already present
  if (add_Collection_member(col_ptr, rec_ptr)) {
    printf("Record is already a member in the collection!\n");
    discard_input();
    return;
  }
  printf("Member %d %s added\n", id, get_Record_title(rec_ptr));
}

// mr <id> <rating>
static void modify_rating(OC_ptr_t lib_id_ptr) {
  int id = 0;
  if (scanf("%d", &id) != 1) {
    printf("Could not read an integer value!\n");
    discard_input();
    return;
  }
  void* item_ptr =
  OC_find_item_arg(lib_id_ptr, &id,
                   (OC_find_item_arg_fp_t)find_Record_id);
  if (!item_ptr) {
    printf("No record with that ID!\n");
    discard_input();
    return;
  }
  int rating = 0;
  if (scanf("%d", &rating) != 1) {
    printf("Could not read an integer value!\n");
    discard_input();
    return;
  }
  if (rating < 1 || rating > 5) {
    printf("Rating is out of range!\n");
    discard_input();
    return;
  }
  struct Record* rec_ptr = OC_get_data_ptr(item_ptr);
  set_Record_rating(rec_ptr, rating);
  printf("Rating for record %d changed to %d\n",
         get_Record_ID(rec_ptr), rating);
}

// dr <title>
static void delete_record(OC_ptr_t lib_title_ptr, OC_ptr_t lib_id_ptr,
                          OC_ptr_t cat_name_ptr) {
  char bad_title[MAX_TITLE_SIZE];
  fgets(bad_title, MAX_TITLE_SIZE, stdin);
  char* title = compact_title(bad_title);
  if (!strlen(title)) {
    printf("Could not read a title!\n");
    return;
  }
  void* item_ptr =
  OC_find_item_arg(lib_title_ptr, title,
                   (OC_find_item_arg_fp_t)find_Record_title);
  
  if (!item_ptr) {
    printf("No record with that title!\n");
    return;
  }
  struct Record* rec_ptr = OC_get_data_ptr(item_ptr);
  
  if (OC_apply_if_arg(cat_name_ptr,
      (OC_apply_if_arg_fp_t)is_Collection_member_present, rec_ptr)) {
    printf("Cannot delete a record that is a member of a collection!\n");
    return;
  }
  printf("Record %d %s deleted\n", get_Record_ID(rec_ptr), title);
  OC_delete_item(lib_title_ptr, item_ptr);
  int id = get_Record_ID(rec_ptr);
  item_ptr = OC_find_item_arg(lib_id_ptr, &id,
                              (OC_find_item_arg_fp_t)find_Record_id);
  destroy_Record(rec_ptr);
  OC_delete_item(lib_id_ptr, item_ptr);
}

// dc <collection_name>
static void delete_collection(OC_ptr_t cat_name_ptr) {
  char collection_name[MAX_COLLECTION_NAME_SIZE];
  scanf("%s", collection_name);
  void* item_ptr =
  OC_find_item_arg(cat_name_ptr, collection_name,
                   (OC_find_item_arg_fp_t)find_Collection_name);
  if (!item_ptr) {
    printf("No collection with that name!\n");
    return;
  }
  struct Collection* col_ptr = OC_get_data_ptr(item_ptr);
  destroy_Collection(col_ptr);
  OC_delete_item(cat_name_ptr, item_ptr);
  printf("Collection %s deleted\n", collection_name);
}
  

// dm <collection_name> <id>
static void delete_member(OC_ptr_t lib_id_ptr, OC_ptr_t cat_name_ptr) {
  char collection_name[MAX_COLLECTION_NAME_SIZE];
  scanf(MAX_COLLECTION_NAME_FORMAT, collection_name);
  void* item_ptr_col = OC_find_item_arg(cat_name_ptr, collection_name,
        (OC_find_item_arg_fp_t)find_Collection_name);

  // check collection has that name
  if (!item_ptr_col) {
    printf("No collection with that name!\n");
    discard_input();
    return;
  }
  int id = 0;
  if (scanf("%d", &id) != 1) {
    printf("Could not read an integer value!\n");
    discard_input();
    return;
  }
  void* item_ptr_rec = OC_find_item_arg(lib_id_ptr, &id,
                        (OC_find_item_arg_fp_t)find_Record_id);
  if (!item_ptr_rec) {
    printf("No record with that ID!\n");
    discard_input();
    return;
  }
  struct Collection* col_ptr = OC_get_data_ptr(item_ptr_col);
  struct Record* rec_ptr = OC_get_data_ptr(item_ptr_rec);
  
  // returns 1 if member is not present
  if (remove_Collection_member(col_ptr, rec_ptr)) {
    printf("Record is not a member in the collection!\n");
    discard_input();
    return;
  }
  printf("Member %d %s deleted\n", id, get_Record_title(rec_ptr));
}

// cL
static void clear_library(OC_ptr_t lib_title_ptr, OC_ptr_t lib_id_ptr,
                          OC_ptr_t cat_name_ptr, int* id_counter_ptr) {
  if (OC_empty(lib_title_ptr) || OC_empty(lib_id_ptr)) {
    printf("All records deleted\n");
    return;
  }
  // first check each collection is empty
  if (!(OC_apply_if(cat_name_ptr, (OC_apply_if_fp_t)Collection_empty))) {
    printf("Cannot clear all records unless all collections are empty!\n");
    discard_input();
    return;
    
  }
  OC_apply(lib_title_ptr, (OC_apply_fp_t)destroy_Record);
  OC_clear(lib_title_ptr);
  OC_clear(lib_id_ptr);
  printf("All records deleted\n");
  // need to reset id_counter to be 1
  (*id_counter_ptr) = 1;
}

// cC
static void clear_catalog(OC_ptr_t cat_name_ptr) {
  OC_apply(cat_name_ptr, (OC_apply_fp_t)destroy_Collection);
  OC_clear(cat_name_ptr);
  printf("All collections deleted\n");
}

// cA
static void clear_all_data(OC_ptr_t lib_title_ptr, OC_ptr_t lib_id_ptr,
                           OC_ptr_t cat_name_ptr, int* id_counter_ptr) {
  // oc_apply(cat_name_ptr, destr)
  clear_containers(lib_title_ptr, lib_id_ptr, cat_name_ptr);
  // need to reset id_counter to be 1
  (*id_counter_ptr) = 1;
  printf("All data deleted\n");
}

// sA <filename>
static void save_all_data(const OC_ptr_t lib_title_ptr,
                          const OC_ptr_t cat_name_ptr) {
  char file_name[MAX_FILE_NAME_SIZE];
  scanf(MAX_FILE_NAME_FORMAT, file_name);
  FILE *file_write_ptr = fopen(file_name, "w");
  if(!file_write_ptr) {
    printf("Could not open file!\n");
    discard_input();
    return;
  }
  fprintf(file_write_ptr, "%d\n", OC_get_size(lib_title_ptr));
  OC_apply_arg(lib_title_ptr, (OC_apply_arg_fp_t)save_Record,
               file_write_ptr);
  fprintf(file_write_ptr, "%d\n", OC_get_size(cat_name_ptr));
  OC_apply_arg(cat_name_ptr, (OC_apply_arg_fp_t)save_Collection,
               file_write_ptr);
  fclose(file_write_ptr);
  printf("Data saved\n");
}

// rA <filenam>
static void restore_all_data(OC_ptr_t lib_title_ptr, OC_ptr_t lib_id_ptr,
                             OC_ptr_t cat_name_ptr, int* id_counter_ptr) {
  char file_name[MAX_FILE_NAME_SIZE];
  scanf(MAX_FILE_NAME_FORMAT, file_name);
  FILE *file_read_ptr = fopen(file_name, "r");
  if(!file_read_ptr) {
    printf("Could not open file!\n");
    discard_input();
    return;
  }
  clear_containers(lib_title_ptr, lib_id_ptr, cat_name_ptr);
  
  // dealing with records now
  int total_movies = 0;
  if (fscanf(file_read_ptr, "%d", &total_movies) != 1) {
    printf("Invalid data found in file!\n");
    discard_input();
    clear_containers(lib_title_ptr, lib_id_ptr, cat_name_ptr);
    *id_counter_ptr = 1;
    fclose(file_read_ptr);
    return;
  }
  if (total_movies < 0) {
    printf("Invalid data found in file!\n");
    discard_input();
    clear_containers(lib_title_ptr, lib_id_ptr, cat_name_ptr);
    *id_counter_ptr = 1;
    fclose(file_read_ptr);
    return;
  }
  int highest_id = 0;
  for (int i = 0; i < total_movies; i++) {
    struct Record* rec_ptr = load_Record(file_read_ptr);
    if (!rec_ptr) {
      printf("Invalid data found in file!\n");
      discard_input();
      clear_containers(lib_title_ptr, lib_id_ptr, cat_name_ptr);
      *id_counter_ptr = 1;
      fclose(file_read_ptr);
      return;
    }
    if (get_Record_ID(rec_ptr) > highest_id) {
      highest_id = get_Record_ID(rec_ptr);
    }
    // need to insert into 2 library containers
    OC_insert(lib_title_ptr, rec_ptr);
    OC_insert(lib_id_ptr, rec_ptr);
  } // for
  
  *id_counter_ptr = highest_id + 1;
  
  // dealing with collections now
  int total_collections = 0;
  if (fscanf(file_read_ptr, "%d", &total_collections) != 1) {
    printf("Invalid data found in file!\n");
    discard_input();
    clear_containers(lib_title_ptr, lib_id_ptr, cat_name_ptr);
    *id_counter_ptr = 1;
    fclose(file_read_ptr);
    return;
  }
  if (total_collections < 0) {
    printf("Invalid data found in file!\n");
    discard_input();
    clear_containers(lib_title_ptr, lib_id_ptr, cat_name_ptr);
    *id_counter_ptr = 1;
    fclose(file_read_ptr);
    return;
  }
  for (int i = 0; i < total_collections; i++) {
    struct Collection* collection_ptr =
    load_Collection(file_read_ptr, lib_title_ptr);
    if (!collection_ptr) {
      printf("Invalid data found in file!\n");
      discard_input();
      clear_containers(lib_title_ptr, lib_id_ptr, cat_name_ptr);
      *id_counter_ptr = 1;
      fclose(file_read_ptr);
      return;
    }
    OC_insert(cat_name_ptr, collection_ptr);
  }
  fclose(file_read_ptr);
  printf("Data loaded\n");
}

/**********************2nd level functions*************************/


static void clear_containers(OC_ptr_t lib_title_ptr, OC_ptr_t lib_id_ptr,
                        OC_ptr_t cat_name_ptr) {
  OC_apply(cat_name_ptr, (OC_apply_fp_t)destroy_Collection);
  OC_clear(cat_name_ptr);
  // apply once is sufficient
  OC_apply(lib_id_ptr, (OC_apply_fp_t)destroy_Record);
  OC_clear(lib_id_ptr);
  OC_clear(lib_title_ptr);
}


// removes leading, embedded and trailing whitespace
static char* compact_title(char* str) {
  // remove leading space
  while(isspace(*str)) {
    ++str;
  }
  char* beg = str;
  char *dst = str;
  for (; *str; ++str) {
    *dst++ = *str;
    if (isspace(*str)) {
      do ++str;
      while (isspace(*str));
      --str;
    } // if
  } // for
  *dst = '\0';
  // trim trailing space
  char* end = beg + (int)strlen(beg) - 1;
  while(end > beg && isspace(*end)) {
    end--;
  };
  
  // write new null terminator at the end
  *(end + 1) = '\0';
  return beg;
}

static void discard_input(void) {
  char c;
  while ((c = getchar()) != '\n') {
  }
}

                              
                              
