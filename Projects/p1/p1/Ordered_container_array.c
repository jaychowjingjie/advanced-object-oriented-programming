//
//  Ordered_container_array.c
//  Project1
//
//  Created by Jay Chow on 1/19/17.
//  Copyright © 2017 Jay Chow. All rights reserved.
//

#include "Ordered_container.h"
#include <stdlib.h>

#define ARRAY_INITIAL_SIZE 3
#define ARRAY_GROWTH_RATE 2

/* number of Ordered_containers currently allocated */
int g_Container_count = 0;
/* number of Ordered_container items currently in use */
int g_Container_items_in_use = 0;
/* number of Ordered_container items currently allocated */
int g_Container_items_allocated = 0;


/* A complete type declaration for Ordered_container implemented as an array */
struct Ordered_container {
  OC_comp_fp_t comp_fun;	/* pointer to comparison function  */
  void** array;			/* pointer to array of pointers to void */
  int allocation;			/* current size of array */
  int size;				/* number of items  currently in the array */
};

// helper functions
static void init(struct Ordered_container* );
static int binary_search(const struct Ordered_container* ,
                         const void* ,
                         int (*comp)(const void*, const void*));
static void swap_array_elt(void** item_ptr_lhs, void** item_ptr_rhs);
static void realloc_array(struct Ordered_container* );

/* Create an empty container using the supplied comparison function,
 and return the pointer to it. */
struct Ordered_container* OC_create_container(OC_comp_fp_t f_ptr) {
  struct Ordered_container* oc_ptr =
  malloc(sizeof(struct Ordered_container));
  oc_ptr->comp_fun = f_ptr;
  init(oc_ptr);
  g_Container_items_allocated += ARRAY_INITIAL_SIZE;
  g_Container_count += 1;
  return oc_ptr;
}

/* Destroy the container and its items; caller is responsible for
 deleting all pointed-to data before calling this function.
 After this call, the container pointer value must not be used
 again. */
void OC_destroy_container(struct Ordered_container* c_ptr) {
  free(c_ptr->array);
  c_ptr->array = NULL;
  g_Container_items_in_use -= c_ptr->size;
  g_Container_items_allocated -= c_ptr->allocation;
  free(c_ptr);
  c_ptr = NULL;
  g_Container_count -= 1;
}

/* Delete all the items in the container and initialize it.
 Caller is responsible for deleting any pointed-to data first. */
void OC_clear(struct Ordered_container* c_ptr) {
  g_Container_items_in_use -= c_ptr->size;
  g_Container_items_allocated =
  g_Container_items_allocated - c_ptr->allocation + ARRAY_INITIAL_SIZE;
  c_ptr->size = 0;
  c_ptr->allocation = ARRAY_INITIAL_SIZE;
}

/* Return the number of items currently stored in the container */
int OC_get_size(const struct Ordered_container* c_ptr) {
  return c_ptr->size;
}

/* Return non-zero (true) if the container is empty, zero (false)
 if the container is non-empty */
int OC_empty(const struct Ordered_container* c_ptr) {
  if (!(c_ptr->size)) {
    return 1;
  }
  return 0;
}

/*
 Functions for working with individual items in the container.
 */

/* Get the data object pointer from an item. */
void* OC_get_data_ptr(const void* item_ptr) {
  void** cell_ptr = (void*)item_ptr;
  // note that array cell is also a void*
  return *cell_ptr;
}

/* Delete the specified item.
 Caller is responsible for any deletion of the data pointed to by
 the item. */
void OC_delete_item(struct Ordered_container* c_ptr, void* item_ptr) {
  void** cell_ptr = item_ptr;
  int item_location = (int)(cell_ptr - c_ptr->array);
  int swap_item = c_ptr->size - item_location - 1;
  while(swap_item--) {
    swap_array_elt(cell_ptr, cell_ptr + 1);
    cell_ptr++;
  }
  c_ptr->size--;
  g_Container_items_in_use -= 1;
}

/*
 Functions that search and insert into the container using the
 supplied comparison function.
 */

/* Create a new item for the specified data pointer and put it in
 the container in order. If there is already an item in the
 container that compares equal to new item according to the
 comparison function, the insertion will not take place and 0 is
 returned to indicate failure. Otherwise, the insertion is done and
 non-zero is returned to show success. This function will not modify
 the pointed-to data. */
int OC_insert(struct Ordered_container* c_ptr, const void* data_ptr) {
  if(c_ptr->size == c_ptr->allocation) {
    realloc_array(c_ptr);
  }
  
  if(!(c_ptr->size)) {
    *(c_ptr->array) = (void *)data_ptr;
    c_ptr->size++;
    g_Container_items_in_use++;
    return 1;
  }
  int insert_spot = binary_search(c_ptr, data_ptr, c_ptr->comp_fun);
  if(insert_spot != c_ptr->size) {
    if(c_ptr->comp_fun(*(c_ptr->array + insert_spot), data_ptr) == 0)
      return 0;
    int swap_item = c_ptr->size;
    if(insert_spot < 0 ) insert_spot = 0;
    while(swap_item != insert_spot) {
      swap_array_elt((c_ptr->array + swap_item),
                     (c_ptr->array + swap_item - 1));
      swap_item--;
    }
    *(c_ptr->array + insert_spot) = (void*)data_ptr;
    
  }
  else {
    *(c_ptr->array + c_ptr->size) = (void*)data_ptr;
  }
  c_ptr->size++;
  g_Container_items_in_use++;
  return 1;
}


/* Return a pointer to an item that points to data equal to the data
 object pointed to by data_ptr, using the ordering function to do
 the comparison with data_ptr as the first argument. The data_ptr
 object is assumed to be of the same type as the data objects pointed
 to by container items. NULL is returned if no matching item is found.
 If more than one matching item is present, it is unspecified which
 one is returned. The pointed-to data will not be modified. */
void* OC_find_item(const struct Ordered_container* c_ptr,
                   const void* data_ptr) {
  if(c_ptr->size == 0) return NULL;
  int found_item = binary_search(c_ptr, data_ptr, c_ptr->comp_fun);
  if(found_item >= c_ptr->size) return NULL;
  if(c_ptr->comp_fun(*(c_ptr->array + found_item), data_ptr) == 0) {
    return (c_ptr->array + found_item);
  }
  return NULL;
}

/* Return a pointer to the item that points to data that matches the
 supplied argument given by arg_ptr according to the supplied
 function, which compares arg_ptr as the first argument with the data
 pointer in each item. This function does not require that arg_ptr be
 of the same type as the data objects, and so allows the container
 to be searched without creating a complete data object first.
 NULL is returned if no matching item is found. If more than one
 matching item is present, it is unspecified which one is returned.
 The comparison function must implement an ordering consistent with
 the ordering produced by the comparison function specified when the
 container was created; if not, the result is undefined. */
void* OC_find_item_arg(const struct Ordered_container* c_ptr,
                       const void* arg_ptr, OC_find_item_arg_fp_t fafp) {
  if(c_ptr->size == 0) return NULL;
  int found_item = binary_search(c_ptr, arg_ptr, fafp);
  if(found_item >= c_ptr->size) return NULL;
  if(fafp(arg_ptr, *(c_ptr->array + found_item)) == 0) {
    return (c_ptr->array + found_item);
  }
  return NULL;
}

/* Apply the supplied function to the data pointer in each item of
 the container. The contents of the container cannot be modified. */
void OC_apply(const struct Ordered_container* c_ptr, OC_apply_fp_t afp) {
  for(int i = 0 ; i < (c_ptr->size); i++) {
    afp(c_ptr->array[i]);
  }
}


/* Apply the supplied function to the data pointer in each item in
 the container. If the function returns non-zero, the iteration is
 terminated, and that value returned. Otherwise, zero is returned.
 The contents of the container cannot be modified. */
int OC_apply_if(const struct Ordered_container* c_ptr,
                OC_apply_if_fp_t afp) {
  for(int i = 0 ; i < (c_ptr->size); i++) {
    if(afp(*(c_ptr->array + i))) {
      return afp(c_ptr->array[i]);
    }
  }
  return 0;
}


/* Apply the supplied function to the data pointer in each item in
 the container; the function takes a second argument, which is the
 supplied void pointer. The contents of the container cannot be
 modified. */
void OC_apply_arg(const struct Ordered_container* c_ptr,
                  OC_apply_arg_fp_t afp, void* arg_ptr) {
  for(int i = 0 ; i < (c_ptr->size); ++i) {
    afp((c_ptr->array[i]), arg_ptr);
  }
}

/* Apply the supplied function to the data pointer in each item in
 the container; the function takes a second argument, which is the
 supplied void pointer. If the function returns non-zero, the
 iteration is terminated, and that value returned. Otherwise, zero
 is returned. The contents of the container cannot be modified */
int OC_apply_if_arg(const struct Ordered_container* c_ptr,
                    OC_apply_if_arg_fp_t afp, void* arg_ptr) {
  for(int i = 0 ; i < (c_ptr->size); ++i) {
    if(afp(c_ptr->array[i], arg_ptr)) {
     return afp(c_ptr->array[i], arg_ptr);
    }
  }
  return 0;
}



static void init(struct Ordered_container* oc_ptr) {
  oc_ptr->allocation = ARRAY_INITIAL_SIZE;
  oc_ptr->size = 0;
  oc_ptr->array = malloc(ARRAY_INITIAL_SIZE * sizeof(void*));
}


static int binary_search(const struct Ordered_container* c_ptr,
                      const void* data_ptr,
                      int (*comp)(const void*, const void*)) {
  int low = 0;
  int mid = 0;
  int high = 0;
  high = c_ptr->size - 1;
  while(low <= high) {
    mid = (low + high) / 2;
    
    if(comp(data_ptr, c_ptr->array[mid]) < 0) {
      high = mid - 1;
      if(high < low) return mid;
    }
    else if(comp(data_ptr, c_ptr->array[mid]) > 0) {
      low = mid + 1;
      if(low > high) return low;
    }
    else {
      return mid;
    }
  }
  return mid + 1;
}

// swap the data pointers
static void swap_array_elt(void** item_ptr_lhs, void** item_ptr_rhs) {
  void* temp_data_ptr = *item_ptr_lhs;
  *item_ptr_lhs = *item_ptr_rhs;
  *item_ptr_rhs = temp_data_ptr;
}



static void realloc_array(struct Ordered_container* oc_ptr) {
  int alloc = ARRAY_GROWTH_RATE * (oc_ptr->size + 1);
  int size = oc_ptr->size;
  void** new_arr = malloc(alloc * sizeof(void*));
  for(int i = 0; i < size; i++) {
    new_arr[i] = oc_ptr->array[i];
  }
  free(oc_ptr->array);
  g_Container_items_allocated += (alloc - oc_ptr->allocation);
  oc_ptr->allocation = alloc;
  oc_ptr->size = size;
  oc_ptr->array = new_arr;
}
