/*
 This contains a demo of the Ordered_container module; the behavior 
 is the same regardless of whether the container or array 
 implementation is used.
 
 This demo uses a container of simple structure types - they are 
 local variables; more commonly they would be dynamically allocated.
 
 This version of the demo uses a comparison functions that takes 
 void* pointers and does internal casts, and uses inlined wrapper 
 functions to produce compatible pointer types for apply-functions 
 that take actual type parameters.
 */

#include <stdio.h>
#include "Ordered_container.h"

/* a simple structure type */
struct Point {
  int x;
  int y;
};

/* function prototypes */

/* comparison function with void* parameters */
int compare_Points(const void* ptr1, const void* ptr2);

/* these apply-functions all use actual data type parameters */
void print_Point(struct Point* p);
void demo_apply_func(struct Point* data_ptr, int* arg);

/* wrappers for above, declared as inline (and static) to avoid 
 function call overhead */
static inline void call_print_Point(void* data_ptr);
static inline void call_demo_apply_func(void*data_ptr, void* arg_ptr);

/* functions do not need either wrappers or function pointer casts */
void insert_and_check(struct Ordered_container* container, struct Point *probe);
void find_and_remove(struct Ordered_container * container, struct Point probe);


int main(void)
{
  /* test with some Point objects - the container items hold their 
   addresses, not the actual Point structs! */
  /* "brace initialization" is convenient for structure types */
  struct Point p1 = {1, 1};
  struct Point p2 = {1, 2};
  struct Point p3 = {2, 1};
  struct Point p4 = {2, 2};
  struct Point probe1 = {1, 2};
  struct Point probe2 = {99, 99};
		
  struct Ordered_container * container;
  
  container = OC_create_container(compare_Points);
  
  printf("size is %d\n", OC_get_size(container));
  
  /* call the wrapper to avoid incompatible function pointer types */
  OC_apply(container, call_print_Point);
  
  /* fill the container with p1, p2, p3, p4 but insert in this order: 
   p3, p1, p4, p2, and see if the points get put in the right order. */
  insert_and_check(container, &p3);
  insert_and_check(container, &p1);
  insert_and_check(container, &p4);
  // try to insert p3 again
  insert_and_check(container, &p3);
  insert_and_check(container, &p2);
  
  /* use find_item and delete_item */
  find_and_remove(container, probe1);
  printf("size is %d\n", OC_get_size(container));
  OC_apply(container, call_print_Point);
  
  find_and_remove(container, probe2);
  printf("size is %d\n", OC_get_size(container));
  OC_apply(container, call_print_Point);
  
  /* demo another apply function */
  {
    int odd_or_even_value = 42;
    OC_apply_arg(container, call_demo_apply_func, &odd_or_even_value);
    odd_or_even_value = 3;
    OC_apply_arg(container, call_demo_apply_func, &odd_or_even_value);
  }
  
  OC_clear(container);
  
  printf("size is %d\n", OC_get_size(container));
  OC_apply(container, (OC_apply_fp_t)print_Point);
  
  OC_destroy_container(container);
  /* using the pointer "container" is undefined at this point */
  
  printf("Done\n");
  return 0;
}

/* Order two points first by x value, then by y value */
int compare_Points(const void* data_ptr1, const void* data_ptr2)
{
  /* cast to actual data types */
  struct Point* ptr1 = (struct Point*)data_ptr1;
  struct Point* ptr2 = (struct Point*)data_ptr2;
  
  /* compare x values if different; if the same, compare y values */
  if(ptr1->x != ptr2->x)
    return (ptr1->x - ptr2->x);
  return (ptr1->y - ptr2->y);
}


/* this apply-function simply outputs the point */
void print_Point(struct Point* p)
{
  printf("(%d, %d)\n", p->x, p->y);
}

/* wrapper to make function pointer types compatible */
static inline void call_print_Point(void* data_ptr)
{
  //	print_Point((struct Point*)data_ptr); // explicit cast
  print_Point(data_ptr); // implicit cast
}


/* this apply-function uses the arg parameter to make a decision about what to output for each item */
void demo_apply_func(struct Point* data_ptr, int* arg_ptr)
{
  if(*arg_ptr % 2)  /* is the arg even or odd? */
    printf("I like this item: ");
  else
    printf("I hate this item: ");
  print_Point(data_ptr);
}

/* wrapper to make function pointer types compatible */
static inline void call_demo_apply_func(void* data_ptr, void* arg_ptr)
{
  demo_apply_func(data_ptr, arg_ptr); // implicit cast
}

/* attempt to insert the data_ptr into the container, check for success;
 if successful, print the size and contents of the container */
void insert_and_check(struct Ordered_container * container, struct Point * data_ptr)
{
  printf("\ninserting (%d, %d)\n", data_ptr->x, data_ptr->y);
  int result = OC_insert(container, data_ptr);
  if(!result) {
    printf("insertion failed!\n");
    return;
		}
  printf("size is %d\n", OC_get_size(container));
  OC_apply(container,(OC_apply_fp_t)print_Point);
}

/* search the container for the probe item; if found output it and then delete it */
void find_and_remove(struct Ordered_container * container, struct Point probe)
{
  printf("\nsearch for (%d, %d):\n", probe.x, probe.y);
  void * found_item = OC_find_item(container, &probe);
  if(found_item) {
    struct Point * p = (struct Point *)OC_get_data_ptr(found_item);
    printf("found item points to (%d, %d)\n", p->x, p->y);
    OC_delete_item(container, found_item);
    printf("item removed\n");
    /* found_item now points to an undefined value - it is invalid */
		}
  else {
    printf("probed item not found\n");
		}
}



/* output
 size is 0
 
 inserting (2, 1)
 size is 1
 (2, 1)
 
 inserting (1, 1)
 size is 2
 (1, 1)
 (2, 1)
 
 inserting (2, 2)
 size is 3
 (1, 1)
 (2, 1)
 (2, 2)
 
 inserting (2, 1)
 insertion failed!
 
 inserting (1, 2)
 size is 4
 (1, 1)
 (1, 2)
 (2, 1)
 (2, 2)
 
 search for (1, 2):
 found item points to (1, 2)
 item removed
 size is 3
 (1, 1)
 (2, 1)
 (2, 2)
 
 search for (99, 99):
 probed item not found
 size is 3
 (1, 1)
 (2, 1)
 (2, 2)
 I hate this item: (1, 1)
 I hate this item: (2, 1)
 I hate this item: (2, 2)
 I like this item: (1, 1)
 I like this item: (2, 1)
 I like this item: (2, 2)
 size is 0
 Done
 */
