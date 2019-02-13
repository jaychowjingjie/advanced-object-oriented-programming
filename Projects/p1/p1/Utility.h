//
//  Utility.h
//  Project1
//
//  Created by Jay Chow on 1/22/17.
//  Copyright © 2017 Jay Chow. All rights reserved.
//

#ifndef UTILITY_H
#define UTILITY_H
#include "Record.h"
#include "Collection.h"

/* max size always 1 more than format size, cause of null byte,
 fscanf, scanf sticks in null byte! */
#define MAX_TITLE_SIZE 64
#define MAX_TITLE_FORMAT "%63s"

#define MAX_COLLECTION_NAME_SIZE 16
#define MAX_COLLECTION_NAME_FORMAT "%15s"

#define MAX_MEDIUM_SIZE 8
#define MAX_MEDIUM_FORMAT "%7s"

#define MAX_FILE_NAME_SIZE 32
#define MAX_FILE_NAME_FORMAT "%31s"



/* check ptr returned from malloc is NULL, indicating memory
 is unsuccessful */
void check_bad_allocation(void* ptr);

/* returns a pointer to the newly allocated c-string on heap, and
 updates global variable g_string_memory accordingly */
char* c_string_allocation(const char* string_in);

/* deallocates memory for c-string on heap, then updates global
 variable g_string_memory accordingly */
void c_string_deallocation(char* ptr);

/* comparison function to feed during construction of Library
 ordered by record ids */
int order_Record_id(const struct Record* rec_ptr_lhs,
                    const struct Record* rec_ptr_rhs);

int find_Record_id(const int* id_ptr, const struct Record* rec_ptr);

/* comparison function to feed during construction of ordered
 container in each Collection object or Library ordered by record
 titles */
int order_Record_title(const struct Record* rec_ptr_lhs,
                       const struct Record* rec_ptr_rhs);

int find_Record_title(const char* title, const struct Record* rec_ptr);


/* comparison function to feed during construction of Catalog */
int order_Collection_name(const struct Collection* collection_ptr_lhs,
                          const struct Collection* collection_ptr_rhs);

int find_Collection_name(const char* name, const struct Collection* col_ptr);

/* for use specifically in save_collection function */
void save_Record_title(const struct Record* record_ptr, FILE* outfile);

#endif
