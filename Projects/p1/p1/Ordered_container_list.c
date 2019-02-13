//
//  Ordered_container_list.c
//  Project1
//
//  Created by Jay Chow on 1/19/17.
//  Copyright © 2017 Jay Chow. All rights reserved.
//

#include "Ordered_container.h"
#include <stdlib.h>


/* struct LL_Node structure declaration. This declaration is local 
 to this file.
 This is a two-way or doubly-linked list. Each node has a pointer to 
 the previous node and another pointer to the next node in the list. 
 This means insertions or removals of a node can be made in constant 
 time, once the location has been determined. */
struct LL_Node {
  struct LL_Node* prev;      /* pointer to the previous node */
  struct LL_Node* next;		/* pointer to the next node */
  void* data_ptr; 			/* uncommitted pointer to the data item */
};

/* Declaration for Ordered_container. This declaration is local to 
 this file. A pointer is maintained to the last node in the list as 
 well as the first, meaning that additions to the end of the list 
 can be made in constant time. The number of nodes in the list is 
 kept up-to-date in the size member variable, so that the size of 
 the list can be accessed in constant time. */
struct Ordered_container {
  OC_comp_fp_t comp_func;
  struct LL_Node* first;
  struct LL_Node* last;
  int size;
};


/* number of Ordered_containers currently allocated */
int g_Container_count = 0;
/* number of Ordered_container items currently in use */
int g_Container_items_in_use = 0;
/* number of Ordered_container items currently allocated */
int g_Container_items_allocated = 0;

// helper functions
static void reset_oc_data_members(struct Ordered_container* oc_ptr);
static void free_all_nodes(struct Ordered_container* oc_ptr);

/*
 Functions for the entire container.
 */

/* Create an empty container using the supplied comparison function,
 and return the pointer to it. */
struct Ordered_container* OC_create_container(OC_comp_fp_t f_ptr) {
  struct Ordered_container* oc_ptr =
  malloc(sizeof(struct Ordered_container));
  // sets the function pointer member to the fp passed in
  oc_ptr->comp_func = f_ptr;
  reset_oc_data_members(oc_ptr);
  g_Container_count += 1;
  return oc_ptr;
}

/* Destroy the container and its items; caller is responsible for
 deleting all pointed-to data before calling this function.
 After this call, the container pointer value must not be used
 again. */
void OC_destroy_container(struct Ordered_container* c_ptr) {
  free_all_nodes(c_ptr);
  free(c_ptr);
  c_ptr = NULL;
  g_Container_count -= 1;
}

/* Delete all the items in the container and initialize it.
 Caller is responsible for deleting any pointed-to data first. */
void OC_clear(struct Ordered_container* c_ptr) {
  free_all_nodes(c_ptr);
  reset_oc_data_members(c_ptr);
}

/* Return the number of items currently stored in the container */
int OC_get_size(const struct Ordered_container* c_ptr) {
  return c_ptr->size;
}

/* Return non-zero (true) if the container is empty, zero (false)
 if the container is non-empty */
int OC_empty(const struct Ordered_container* c_ptr) {
  if (c_ptr->size  == 0) {
    return 1;
  }
  return 0;
}

/*
 Functions for working with individual items in the container.
 */

/* Get the data object pointer from an item. */
void* OC_get_data_ptr(const void* item_ptr) {
  // first cast void* into LL_Node*
  struct LL_Node* node_ptr = (struct LL_Node*) item_ptr;
  return node_ptr->data_ptr;
}

/* Delete the specified item.
 Caller is responsible for any deletion of the data pointed to by
 the item. */
void OC_delete_item(struct Ordered_container* c_ptr, void* item_ptr) {
  struct LL_Node* next_item = ((struct LL_Node*)item_ptr)->next;
  struct LL_Node* prev_item = ((struct LL_Node*)item_ptr)->prev;
  if(prev_item) {
    prev_item->next = next_item;
  }
  else {
    c_ptr->first = next_item;
  }
  if(next_item) {
    next_item->prev = prev_item;
  }
  else {
    c_ptr->last = prev_item;
  }
  free(item_ptr);
  g_Container_items_in_use--;
  g_Container_items_allocated--;
  c_ptr->size--;
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
  struct LL_Node* insert_pos = c_ptr->first;
  // if list is not empty
  if(!OC_empty(c_ptr)) {
    while(insert_pos) {
      if(c_ptr->comp_func(insert_pos->data_ptr, data_ptr) == 0) {
        return 0;
      }
      else if(c_ptr->comp_func(insert_pos->data_ptr, data_ptr) < 0) {
        insert_pos = insert_pos->next;
      }
      else {
        // found the place to insert
        break;
      }
    }//while
  }//if
  
  struct LL_Node* new_node_ptr = malloc(sizeof(struct LL_Node));
  new_node_ptr->prev = new_node_ptr->next = NULL;
  new_node_ptr->data_ptr = (void*)data_ptr;
  
  if(!OC_empty(c_ptr)) {
    if(insert_pos) {
      new_node_ptr->next = insert_pos;
      new_node_ptr->prev = insert_pos->prev;
      insert_pos->prev = new_node_ptr;
      if(new_node_ptr->prev) {
        new_node_ptr->prev->next = new_node_ptr;
      }
      else {
        c_ptr->first = new_node_ptr;
      }
    }
    else {
      new_node_ptr->prev = c_ptr->last;
      c_ptr->last->next = new_node_ptr;
      c_ptr->last = new_node_ptr;
    }
  }
  else {
    c_ptr->first = c_ptr->last = new_node_ptr;
  }
  
  g_Container_items_in_use++;
  g_Container_items_allocated++;
  c_ptr->size++;
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
  struct LL_Node* find_node_ptr = c_ptr->first;
  while(find_node_ptr) {
    if(c_ptr->comp_func(data_ptr, find_node_ptr->data_ptr) > 0) {
      find_node_ptr = find_node_ptr->next;
    }
    else if(c_ptr->comp_func(data_ptr, find_node_ptr->data_ptr) == 0) {
      return find_node_ptr;
    }
    // no point finding
    else {
      return NULL;
    }
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
  struct LL_Node* find_ptr = c_ptr->first;
  while(find_ptr) {
    if(fafp(arg_ptr, find_ptr->data_ptr) == 0) {
      return find_ptr;
    }
    else if(fafp(arg_ptr, find_ptr->data_ptr) > 0) {
     find_ptr = find_ptr->next;
    }
    else {
     return NULL;
    }
  }
  return NULL;
}

/* Apply the supplied function to the data pointer in each item of
 the container. The contents of the container cannot be modified. */
void OC_apply(const struct Ordered_container* c_ptr, OC_apply_fp_t afp) {
  struct LL_Node* node_apply_ptr = c_ptr->first;
  while(node_apply_ptr) {
    afp(node_apply_ptr->data_ptr);
    node_apply_ptr = node_apply_ptr->next;
  }
}


/* Apply the supplied function to the data pointer in each item in
 the container. If the function returns non-zero, the iteration is
 terminated, and that value returned. Otherwise, zero is returned.
 The contents of the container cannot be modified. */
int OC_apply_if(const struct Ordered_container* c_ptr,
                OC_apply_if_fp_t afp) {
  // note that fp is of different type passed in from OC_apply
  struct LL_Node* node_apply_if_ptr = c_ptr->first;
  while(node_apply_if_ptr) {
    if(afp(node_apply_if_ptr->data_ptr)) {
      return afp(node_apply_if_ptr->data_ptr);
    }
    node_apply_if_ptr = node_apply_if_ptr->next;
  }
  return 0;
}


/* Apply the supplied function to the data pointer in each item in
 the container; the function takes a second argument, which is the
 supplied void pointer. The contents of the container cannot be
 modified. */
void OC_apply_arg(const struct Ordered_container* c_ptr,
                  OC_apply_arg_fp_t afp, void* arg_ptr) {
  struct LL_Node* node_apply_arg_ptr = c_ptr->first;
  while (node_apply_arg_ptr) {
    afp(node_apply_arg_ptr->data_ptr, arg_ptr);
    node_apply_arg_ptr = node_apply_arg_ptr->next;
  }
}

/* Apply the supplied function to the data pointer in each item in
 the container; the function takes a second argument, which is the
 supplied void pointer. If the function returns non-zero, the
 iteration is terminated, and that value returned. Otherwise, zero
 is returned. The contents of the container cannot be modified */
int OC_apply_if_arg(const struct Ordered_container* c_ptr,
                    OC_apply_if_arg_fp_t afp, void* arg_ptr) {
  // note the only diff is the type of fp passed in from above
  struct LL_Node* node_apply_if_arg_ptr = c_ptr->first;
  while(node_apply_if_arg_ptr) {
    if(afp(node_apply_if_arg_ptr->data_ptr, arg_ptr)) {
      return afp(node_apply_if_arg_ptr->data_ptr, arg_ptr);
    }
    node_apply_if_arg_ptr = node_apply_if_arg_ptr->next;
  }
  return 0;
}

// sets node pointers to be null pointers and size to be 0
static void reset_oc_data_members(struct Ordered_container* oc_ptr) {
  oc_ptr->first = NULL;
  oc_ptr->last = NULL;
  oc_ptr->size = 0;
}

/* deletes all nodes in the ordered container, decrements
 g_Container_items_in_use and g_Container_items_allocated by size
 of container */
static void free_all_nodes(struct Ordered_container* oc_ptr) {
  struct LL_Node* q_ptr;
  for (struct LL_Node* n_ptr = oc_ptr->first; n_ptr != NULL; n_ptr = q_ptr) {
    q_ptr = n_ptr->next;
    free(n_ptr);
  }
  g_Container_items_in_use -= oc_ptr->size;
  g_Container_items_allocated -= oc_ptr->size;
}
