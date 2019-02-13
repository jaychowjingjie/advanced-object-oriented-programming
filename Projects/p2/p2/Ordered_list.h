//
//  Ordered_list.h
//  Project2
//
//  Created by Jay Chow on 1/29/17.
//  Copyright © 2017 Jay Chow. All rights reserved.
//
/* Ordered_list is a linked-list class template with iterators similar to
 the Standard Library std::list class. Each list node contains an object of the
 type specified in the first template parameter, T.
 
 This is an ordered list in that the nodes are automatically kept in order.
 The type of the ordering function is a second template parameter, OF
 (for Ordering Function), whose default value is a type given by a small
 template for a function object class that orders two objects of type T using
 that type's less-than operator. Thus the default is to order objects from
 smallest to largest using their defined less-than relationship. Another
 template provides a function object class that orders two pointers to objects
 of type T by dereferencing the pointers and applying T's less-than operator.
 For example:
 
 // Thing objects in order by Thing::operator<.
 Ordered_list<Thing, Less_than_ref> ol_things;
 
	// the same as above, by default
	Ordered_list<Thing> ol_things;
 
 // Thing pointers in order by Thing::operator< applied to dereferenced pointers.
 Ordered_list<Thing*, Less_than_ptrs> ol_things;
 
 // Thing pointers in order by a custom ordering.
 Ordered_list<Thing*, My_ordering_class> ol_things;
	
 The only way to add to the list is with the insert function, which automatically
 puts the new item in the proper place in the list using the ordering function
 to determine the point of insertion. If a matching object is already in the list
 (as determined by the ordering function) the insertion fails - the list is not
 modified, and false is returned by the insert function.
 
 The iterators encapsulate a pointer to the list nodes, and are a public class
 nested within the Ordered_list class, and would be declared e.g. as
 Ordered_list<Thing*, Less_than_ptr>::Iterator;
 Operators ++, *, and -> are overloaded for iterators similar to std::list<>::iterator.
 
 This container supports const operations; if the container is declared as a const
 object, then the compiler will automatically apply the const versions of the begin(),
 end(), and find() functions, which will return const_Iterators. A const_Iterator
 cannot be used to modify the data to which it points. const_Iterators are also
 a public class nested within the Ordered_list class.
 
 Copy constructor and assignment operators are defined, so that Ordered_lists
 can be used like built-in types. Move construction and assignment operators are
 also defined, consistent with C++11 container library practice.
 
 The operations on Ordered_list provide exception safety in the form of the basic
 exception guarantee - if an attempt to create or modify an Ordered_list fails and
 an exception is thrown, no memory is leaked and the exception is propagated out to
 the client code to catch. In addition, the relevant operations also provide the
 strong exception guarantee - if an attempt to modify an Ordered_list fails, it
 is left in its original state, and the exception is propagated out to the client
 code to catch. Finally, many operations also provide the no-throw guarantee and
 are specified with noexcept. See comments on individual member functions for specifics.
 
 To find an object in the list that matches a supplied "probe" object, the ordering
 function is used to determine equality. That is, the find functions assume that
 if both (x < y) and (y < x) are false, then x == y. This allows both insert and find
 operations to be done with only the less-than relation.
 
 When an object is inserted in the list using the copy version of insert,
 a list node is constructed that contains a copy of the supplied object,
 so objects stored in the list must have accessible and properly defined copy
 constructors. The move version of the insert function will attempt to move
 construct the supplied object in the new list node instead of copying it.
 A move constructor for the object must be available for this to happen.
 
 When a node is removed from the list with erase(), it is destroyed, and so a
 class-type object contained in the node must have an accessible and properly
 defined destructor function. When the list is destroyed, or cleared with the
 clear() function, all of the list nodes are destroyed.
 
 Unless the container is declared to be const, this class does not protect the
 list items from being modified. If a list item is modified in a way that changes
 where it should appear in the list, the list will become disordered and list
 items may become un-findable or new items will be inserted incorrectly - the
 effects are undefined, although a specific implementation might behave in a
 predictable manner. It is user's responsibility to ensure that items are not
 changed in a way that affects the validity of the ordering in the list.
 
 If the user declares a list of pointers to objects, the user is responsible for
 allocating and deallocating the pointed-to objects. Note especially that if
 the Ordered_list is deallocated or cleared, or a single node is erased from the
 list, the pointed-to data is NOT deallocated. In short, the Ordered_list does
 not attempt to manage the user's objects.
 
 If any operations are attempted that are erroneous (e.g. erasing a non-existent
 node), the results are undefined.
 
 This module includes some function templates for applying functions to items in
 the container, using iterators to specify the range of items to apply the function
 to.
 
 All Ordered_list constructors and the destructor increment/decrement
 g_Ordered_list_count. The list Node constructors and destructor
 increment/decrement g_Ordered_list_Node_count.
 */
#ifndef ORDERED_LIST_H
#define ORDERED_LIST_H
#include "p2_globals.h"
#include "Utility.h"
#include <utility>
#include <cassert>
// These Function Object Class templates make it simple to use a class's less-than
// operator for the ordering function in declaring an Ordered_list container.
// These declare operator() as a const member function because the function does
// not modify the state of the function object, meaning that it can be used in
// a const member function of the Ordered_list class.
// Compare two objects (passed by const&) using T's operator<
template<typename T>
struct Less_than_ref {
  bool operator() (const T& t1, const T& t2) const {return t1 < t2;}
};
// Compare two pointers (T is a pointer type) using *T's operator<
template<typename T>
struct Less_than_ptr {
  bool operator()(const T p1, const T p2) const {return *p1 < *p2;}
};
// A custom ordering function object class is similar, but no template is needed. For example:
//  struct My_ordering_class {
//		bool operator() (const Thing* p1, const Thing* p2) const {return /* whatever you want */;}
//	};
// Note: noexcept is promise that DECLARED function doesn't throw exception!
// T is the type of the objects in the list - the data item in the list node
// OF is the ordering function object type, defaulting to Less_than_ref for T
template<typename T, typename OF = Less_than_ref<T> >
class Ordered_list {
public:
  // Default constructor creates an empty container that has an ordering function
  // object of the type specified in the second template type parameter (OF).
  Ordered_list();
  
  // Copy construct this list from another list by copying its data.
  // The basic exception guarantee:
  // If an exception is thrown when the type T contents of a node are copied,
  // any nodes already constructed are destroyed, so that no memory is leaked,
  // and the exception is then propagated from this constructor.
  Ordered_list(const Ordered_list& original);
  
  // Move construct this list from another list by taking its data,
  // leaving the original in an empty state (like when default constructed).
  // Since no type T data is copied, no exceptions are possible,
  // so the no-throw guarantee is made.
  Ordered_list(Ordered_list&& original) noexcept;
  
  // Copy assign this list with a copy of another list, using the copy-swap idiom.
  // Basic and strong exception guarantee:
  // If an exception is thrown during the copy, no memory is leaked, and lhs is unchanged.
  Ordered_list& operator= (const Ordered_list& rhs);
  
  // Move assignment operator simply swaps the current content with the rhs.
  // Since no type T data is copied, no exceptions are possible,
  // so the no-throw guarantee is made.
  Ordered_list& operator= (Ordered_list&& rhs) noexcept;
  
  // deallocate all the nodes in this list
  ~Ordered_list();
  
  // Delete the nodes in the list, if any, and initialize it.
  // No exceptions are supposed to happen so the no-throw guarantee is made.
  void clear() noexcept;
  
  // Return the number of nodes in the list
  int size() const {
    return num_nodes;
  }
  // Return true if the list is empty
  bool empty() const {
    return !num_nodes;
  }
private:
  // Node is a nested class that is private to the Ordered_list<T, OF> class.
  // It is declared here so that Iterator class can refer to it in function
  // definitions in the class declaration. The supplied members are there
  // to make it clear when the node count should be incremented or decremented.
  // Because it is a private member of Ordered_list, you can add members of your
  // choice, such as special constructors.
  struct Node {
    // Construct a node containing a copy of the T data; the copy operation
    // might throw an exception, so the basic and strong guarantee should
    // be satisfied as long as the copy is attempted before the containing list is
    // modified.
    Node(const T& new_datum, Node* new_prev, Node* new_next) :
    datum(new_datum), prev(new_prev), next(new_next)
    {g_Ordered_list_Node_count++;}
    // Move version of constructs a node using an rvalue reference to the new datum
    // and move constructs the datum member variable from it, using its move constructor.
    Node(T&& new_datum, Node* new_prev, Node* new_next) :
    datum(std::move(new_datum)), prev(new_prev), next(new_next)
    {g_Ordered_list_Node_count++;}
    // Copy constructor, move constructor, and dtor defined only to support allocation
    // counting. Node copy constructor should provide the same basic and strong
    // guarantee as Node construction.
    Node(const Node& original) :
    datum(original.datum), prev(original.prev), next(original.next)
    {g_Ordered_list_Node_count++;}
    // The following functions should not be needed and so are deleted
    Node(Node&& original) = delete;
    Node& operator= (const Node& rhs) = delete; // shouldn't be needed
    Node& operator= (Node&& rhs) = delete; // shouldn't be needed
    // only defined to support allocation counting
    ~Node()
    {g_Ordered_list_Node_count--;}
    T datum;
    Node* prev; // pointer to previous node
    Node* next; // pointer to next node
		};
		
public:
  // An Iterator object designates a Node by encapsulating a pointer to the Node,
  // and provides Standard Library-style operators for using, manipulating,
  // and comparing Iterators. This class is nested inside Ordered_list<> as
  // a public member; refer to as e.g. Ordered_list<int, My_of>::Iterator
  class Iterator {
		public:
    // default initialize to nullptr
    Iterator() : node_ptr(nullptr) {}
    // Overloaded dereferencing operators
    // The * operator returns a reference to the datum in the pointed-to node.
    T& operator* () const {
      assert(node_ptr);
      return (node_ptr->datum);
    }
    // The -> operator simply returns the address of the data in the pointed-to node.
    // The compiler reapplies the -> operator with the returned pointer.
    /* *** The definition is supplied here because it is a special-case of operator overloading. */
    T* operator-> () const {
      assert(node_ptr);
      return &(node_ptr->datum);
    }
    // Prefix ++ operator moves the iterator forward to point to the next node
    // and returns this iterator.
    Iterator& operator++ ()	 {
      assert(node_ptr);
      node_ptr = node_ptr->next;
      return *this;
    }
    // Postfix ++ operator saves the current address for the pointed-to node,
    // moves this iterator to point to the next node, and returns
    // an iterator pointing to the node at the saved address.
    Iterator operator++ (int) {
      assert(node_ptr);
      Node* saved_node_ptr = node_ptr;
      node_ptr = node_ptr->next;
      return Iterator(saved_node_ptr);
    }
    // Iterators are equal if they point to the same node
    bool operator== (Iterator rhs) const {
      return (node_ptr == rhs.node_ptr);
    }
    bool operator!= (Iterator rhs) const {
      return (node_ptr != rhs.node_ptr);
    }
    
    // *** here, declare the outer Ordered_list class as a friend
    friend Ordered_list;
    
		private:
    /* *** define here a private constructor for Iterator that takes a Node* parameter.
     Ordered_list can use this to create Iterators conveniently initialized to point to
     a Node. It is private because the client code can't and shouldn't be using it -
     it isn't even supposed to know about the Node objects.  */
    Iterator(Node* other_node_ptr) : node_ptr(other_node_ptr) {}
    /* *** you may have other private member functions, but not member variables */
    Node* node_ptr;
		};
  // end of nested Iterator class declaration
  
  // const_Iterator is a public nested class within Ordered_list.
  // It behaves identically to an Iterator except that it cannot be used to modify
  // the pointed-to data, as shown by its dereferencing operators returning const.
  class const_Iterator {
		public:
    // default initialize to nullptr
    const_Iterator() : node_ptr(nullptr) {}
    
    // A conversion constructor: construct a const_Iterator from an Iterator
    const_Iterator(Iterator original) : node_ptr(original.node_ptr) {}
    
    // Overloaded dereferencing operators
    // The * operator returns a const reference to the datum in the pointed-to node.
    const T& operator* () const {
      assert(node_ptr);
      return node_ptr->datum;
    }
    // The -> operator returns the address of the data in the pointed-to node as const.
    const T* operator-> () const {
      assert(node_ptr);
      return &(node_ptr->datum);
    }
    
    // prefix ++ operator moves the iterator forward to point to the next node
    // and returns this iterator.
    const_Iterator& operator++ () {
      assert(node_ptr);
      node_ptr = node_ptr->next;
      return *this;
    }
    // postfix ++ operator saves the current address for the pointed-to node,
    // moves this iterator to point to the next node, and returns
    // an iterator pointing to the node at the saved address.
    const_Iterator operator++ (int) {
      assert(node_ptr);
      const_Iterator prev(*this);
      node_ptr = node_ptr->next;
      return prev;
    }
    // const_Iterators are equal if they point to the same node.
    bool operator== (const_Iterator rhs) const
				{return node_ptr == rhs.node_ptr;}
    bool operator!= (const_Iterator rhs) const
				{return node_ptr != rhs.node_ptr;}
    
    // *** here, declare the outer Ordered_list class as a friend
    friend Ordered_list;
    
		private:
    /* *** define here a private constructor for const_Iterator that takes a const
     Node* parameter. Ordered_list can use this to create const_Iterators conveniently
     initialized to point to a Node. It is private because the client code can't and
     shouldn't be using it - it isn't even supposed to know about the Node objects.  */
    const_Iterator(const Node* other_node_ptr) : node_ptr(other_node_ptr) {}
    /* *** you may have other private member functions, but not member variables */
    const Node* node_ptr;
		};
  // end of nested const_Iterator class declaration
  
  /* Supply begin and end iterators for a non-const container */
  // Return an Iterator pointing to the first node.
  // If the list is empty, the begin Iterator points to "past the end"
  Iterator begin()
		{return Iterator(first);}
  // Return an iterator pointing to "past the end".
  Iterator end()
		{return Iterator(nullptr);}	// same as next pointer of last node
  
  // Return a const_Iterator pointing to the first node.
  // If the list is empty, the Iterator points to "past the end".
  const_Iterator cbegin()
		{return const_Iterator(first);}
  // return an iterator pointing to "past the end"
  const_Iterator cend()
		{return const_Iterator(nullptr);}	// same as next pointer of last node
  
  /* Supply begin and end iterators for a const container */
  // Return a const_Iterator pointing to the first node.
  // If the list is empty, the Iterator points to "past the end".
  const_Iterator begin() const
		{return const_Iterator(first);}
  // Return a const_Iterator pointing to "past the end".
  const_Iterator end() const
		{return const_Iterator(nullptr);}	// same as next pointer of last node
  // Return a const_Iterator pointing to the first node,
  // If the list is empty, the Iterator points to "past the end"
  const_Iterator cbegin() const
		{return const_Iterator(first);}
  // return a const_Iterator pointing to "past the end".
  const_Iterator cend() const
		{return const_Iterator(nullptr);}	// same as next pointer of last node
  
  // The insert functions add the new datum to the list using the ordering function.
  // If an "equal" datum is already in the list, then the list is not modified
  // and false is returned. Otherwise, the new datum is inserted, and true is returned.
  // A copy of the datum object is made in the new list node.
  bool insert(const T& new_datum);
  // This version of insert provides for moving the contents of a datum
  // into the new list node instead of copying it.
  bool insert(T&& new_datum);
  
  // The find function returns an iterator designating the node containing
  // the datum that according to the ordering function, is equal to the
  // supplied probe_datum; end() is returned if the node is not found.
  // If a matching item is not present, the scan is terminated as soon as
  // possible by detecting when the scan goes past where the matching item would be.
  Iterator find(const T& probe_datum) noexcept;
  // The const version of find returns a const_Iterator
  const_Iterator find(const T& probe_datum) const noexcept;
  
  // Delete the specified node, specified by a const_Iterator because it only designates
  // the location of the to-be-removed data. This function can be called with an Iterator
  // which will be simply converted to a const_Iterator by the conversion function above.
  // Caller is responsible for any required deletion of any pointed-to data beforehand.
  // Do not attempt to dereference the iterator after calling this function - it
  // is invalid after this function executes. The results are undefined if the
  // Iterator does not point to an actual node, or the list is empty.
  void erase(const_Iterator it) noexcept;
  
  /// Interchange the member variable values of this list with the other list;
  // Only the pointers and size are interchanged;
  // no allocation or deallocation of list Nodes is done.
  // Thus the no-throw guarantee can be provided.
  // This function can only be called with an other list of the same type as
  // this list, which means that the ordering function objects must be identical, and
  // so do not need to be swapped.
  void swap(Ordered_list & other) noexcept;
  
private:
  // member variable declaration for the ordering function object.
  OF ordering_fobj;
  Node* first;
  Node* last;
  int num_nodes;
  // private helper function
  void push_back(const T& datum);
};
// These function templates are given two iterators, usually .begin() and .end(),
// and apply a function to each item (dereferenced iterator) in the list.
// The templates work for both Iterators and const_Iterators.
// Note that the function must be an ordinary function; these templates will not
// instantiate if the function is a member function of some class. However, you
// can often use a simple ordinary function with parameters that calls the
// member function using the parameter values.
template<typename IT, typename F>
void apply(IT first, IT last, F function)
{
  for(; first != last; ++first) {
    function(*first);
  }
}
// the fourth parameter is used as the second argument of the function
template<typename IT, typename F, typename A>
void apply_arg(IT first, IT last, F function, A arg)
{
  for(; first != last; ++first) {
    function(*first, arg);
  }
}
// this function templates accept the second argument by reference - useful for streams.
template<typename IT, typename F, typename A>
void apply_arg_ref(IT first, IT last, F function, A& arg)
{
  for(; first!= last; ++ first) {
    function(*first, arg);
  }
}
// the function must return true/false; apply the function until true is returned,
// then return true; otherwise return false.
template<typename IT, typename F>
bool apply_if(IT first, IT last, F function)
{
  for(; first != last; ++first) {
    if(function(*first)) {
      return true;
    }
  }
  return false;
}
// this function works like apply_if, with a fourth parameter used as the second
// argument for the function
template<typename IT, typename F, typename A>
bool apply_if_arg(IT first, IT last, F function, A arg)
{
  for(; first != last; ++first) {
    if(function(*first, arg))
      return true;
  }
  return false;
}
// Put your code for Ordered_list member functions here, defined outside
// the class declaration.
// Default constructor creates an empty container that has an ordering function
// object of the type specified in the second template type parameter (OF).
template<typename T, typename OF>
Ordered_list<T, OF>::Ordered_list()
: first(nullptr), last(nullptr), num_nodes(0) {
  g_Ordered_list_count++;
}
// Copy construct this list from another list by copying its data.
// The basic exception guarantee:
// If an exception is thrown when the type T contents of a node are copied,
// any nodes already constructed are destroyed, so that no memory is leaked,
// and the exception is then propagated from this constructor.
template<typename T, typename OF>
Ordered_list<T, OF>::Ordered_list(const Ordered_list& original)
: first(nullptr), last(nullptr), num_nodes(0)
{
  Node* copy_node_ptr = original.first;
  g_Ordered_list_count++;
  try {
    while(copy_node_ptr) {
      push_back(copy_node_ptr->datum);
      copy_node_ptr = copy_node_ptr->next;
    }
  } catch(...) {
    clear();
    throw;
  }
}
// Move construct this list from another list by taking its data,
// leaving the original in an empty state (like when default constructed).
// Since no type T data is copied, no exceptions are possible,
// so the no-throw guarantee is made.
template<typename T, typename OF>
Ordered_list<T, OF>::Ordered_list(Ordered_list&& original) noexcept
:first(original.first), last(original.last), num_nodes(original.num_nodes)
{
  original.first = nullptr;
  original.last = nullptr;
  original.num_nodes = 0;
  g_Ordered_list_count++;
}
// Copy assign this list with a copy of another list, using the copy-swap idiom.
// Basic and strong exception guarantee:
// If an exception is thrown during the copy, no memory is leaked, and lhs is unchanged.
template<typename T, typename OF>
Ordered_list<T, OF>& Ordered_list<T, OF>::operator= (const Ordered_list& rhs)
{
  // copy-swap, make temp list based on rhs
  Ordered_list<T, OF> temp_list(rhs);
  // swap with current list
  swap(temp_list);
  return *this;
}
// Move assignment operator simply swaps the current content with the rhs.
// Since no type T data is copied, no exceptions are possible,
// so the no-throw guarantee is made.
template<typename T, typename OF>
Ordered_list<T, OF>& Ordered_list<T, OF>::operator= (Ordered_list&& rhs) noexcept
{
  swap(rhs);
  return *this;
}
// deallocate all the nodes in this list
template<typename T, typename OF>
Ordered_list<T, OF>::~Ordered_list() {
  g_Ordered_list_count--;
  clear();
}
// Delete the nodes in the list, if any, and initialize it.
// No exceptions are supposed to happen so the no-throw guarantee is made.
template<typename T, typename OF>
void Ordered_list<T, OF>::clear() noexcept {
  while (begin() != end()) {
    erase(begin());
  }
}
// The insert functions add the new datum to the list using the ordering function.
// If an "equal" datum is already in the list, then the list is not modified
// and false is returned. Otherwise, the new datum is inserted, and true is returned.
// A copy of the datum object is made in the new list node.
template<typename T, typename OF>
bool Ordered_list<T, OF>::insert(const T& new_datum) {
  // first find correct insertion spot
  Node* insert_node_ptr = first;
  while(insert_node_ptr) {
    // still haven't found yet, data in node < new_datum
    if(ordering_fobj(insert_node_ptr->datum, new_datum)) {
      insert_node_ptr = insert_node_ptr->next;
    }
    else {
      // data already exist
      if(!ordering_fobj(new_datum, insert_node_ptr->datum)) {
        return false;
      }
      // found the spot
      break;
    }
  }
  Node* new_node_ptr = new Node(new_datum, nullptr, nullptr);
  //insertion into correct spot
  if(empty()) {
    first = new_node_ptr;
    last = new_node_ptr;
    num_nodes++;
    return true;
  }
  if(!insert_node_ptr) {
    if(ordering_fobj(new_datum, first->datum)) {
      first->prev = new_node_ptr;
      new_node_ptr->next = first;
      first = new_node_ptr;
    }
    else {
      last->next = new_node_ptr;
      new_node_ptr->prev = last;
      last = new_node_ptr;
    }
  }
  else {
    if(insert_node_ptr->prev) {
      insert_node_ptr->prev->next = new_node_ptr;
    }
    else {
      first = new_node_ptr;
    }
    new_node_ptr->prev = insert_node_ptr->prev;
    new_node_ptr->next = insert_node_ptr;
    insert_node_ptr->prev = new_node_ptr;
  }
  num_nodes++;
  return true;
}

// This version of insert provides for moving the contents of a datum
// into the new list node instead of copying it.
template<typename T, typename OF>
bool Ordered_list<T, OF>::insert(T&& new_datum) {
  Node* insert_node_ptr = first;
  while(insert_node_ptr) {
    // still haven't found yet, data in node < new_datum
    if(ordering_fobj(insert_node_ptr->datum, new_datum)) {
      insert_node_ptr = insert_node_ptr->next;
    }
    else {
      if(!ordering_fobj(new_datum, insert_node_ptr->datum)) {
        return false;
      }
      break;
    }
  }
  Node* new_node_ptr = new Node(std::move(new_datum), nullptr, nullptr);
  if(empty()) {
    first = new_node_ptr;
    last = new_node_ptr;
    num_nodes++;
    return true;
  }
  if(!insert_node_ptr) {
    if(ordering_fobj(new_datum, first->datum)) {
      first->prev = new_node_ptr;
      new_node_ptr->next = first;
      first = new_node_ptr;
    }
    else {
      last->next = new_node_ptr;
      new_node_ptr->prev = last;
      last = new_node_ptr;
    }
  }
  else {
    if(insert_node_ptr->prev) {
      insert_node_ptr->prev->next = new_node_ptr;
    }
    else {
      first = new_node_ptr;
    }
    new_node_ptr->prev = insert_node_ptr->prev;
    new_node_ptr->next = insert_node_ptr;
    insert_node_ptr->prev = new_node_ptr;
  }
  num_nodes++;
  return true;
}
// The find function returns an iterator designating the node containing
// the datum that according to the ordering function, is equal to the
// supplied probe_datum; end() is returned if the node is not found.
// If a matching item is not present, the scan is terminated as soon as
// possible by detecting when the scan goes past where the matching item would be.
template<typename T, typename OF>
typename Ordered_list<T, OF>::Iterator Ordered_list<T, OF>::find(const T& probe_datum) noexcept {
  Node *node = first;
  while (node) {
    if (!ordering_fobj(node->datum, probe_datum)) {
      if (!ordering_fobj(probe_datum, node->datum)) {
        return Iterator(node);
      }
      break;
    }
    node = node->next;
  }
  return nullptr;
}
// The const version of find returns a const_Iterator
template<typename T, typename OF>
typename Ordered_list<T, OF>::const_Iterator Ordered_list<T, OF>::find(const T& probe_datum) const noexcept {
  Node *node = first;
  while (node) {
    if (!ordering_fobj(node->datum, probe_datum)) {
      if (!ordering_fobj(probe_datum, node->datum)) {
        return const_Iterator(node);
      }
      break;
    }
    node = node->next;
  }
  return nullptr;
}
// Delete the specified node, specified by a const_Iterator because it only designates
// the location of the to-be-removed data. This function can be called with an Iterator
// which will be simply converted to a const_Iterator by the conversion function above.
// Caller is responsible for any required deletion of any pointed-to data beforehand.
// Do not attempt to dereference the iterator after calling this function - it
// is invalid after this function executes. The results are undefined if the
// Iterator does not point to an actual node, or the list is empty.
template<typename T, typename OF>
void Ordered_list<T, OF>::erase(const_Iterator it) noexcept {
  assert(it.node_ptr);
  if (it != end()) {
    if(it.node_ptr ->next) {
      it.node_ptr->next->prev = it.node_ptr->prev;
    }
    else {
      last = it.node_ptr->prev;
    }
    if (it.node_ptr->prev) {
      it.node_ptr->prev->next = it.node_ptr->next;
    }
    else {
      first = it.node_ptr->next;
    }
    num_nodes--;
    delete it.node_ptr;
  }
}
/// Interchange the member variable values of this list with the other list;
// Only the pointers and size are interchanged;
// no allocation or deallocation of list Nodes is done.
// Thus the no-throw guarantee can be provided.
// This function can only be called with an other list of the same type as
// this list, which means that the ordering function objects must be identical, and
// so do not need to be swapped.
template<typename T, typename OF>
void Ordered_list<T, OF>::swap(Ordered_list & other) noexcept {
  std::swap(first, other.first);
  std::swap(last, other.last);
  std::swap(num_nodes, other.num_nodes);
}
// private helper
template<typename T, typename OF>
void Ordered_list<T, OF>::push_back(const T& datum) {
  if (!first) {
    first = last = new Node(datum, first, nullptr);
  }
  // modify the next pointer of the ex-last node
  else {
    last->next = new Node(datum, first, nullptr);
    last = last->next;
  }
  num_nodes++;
}
#endif
