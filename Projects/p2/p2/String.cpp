//
//  String.cpp
//  Project2
//
//  Created by Jay Chow on 1/29/17.
//  Copyright © 2017 Jay Chow. All rights reserved.
//

#include "String.h"
#include <cstring>
#include <cctype>
#include <utility>
using namespace std;

// intialize static member variables
char String::a_null_byte = '\0';
// counts number of String objects in existence
int String::number = 0;
// counts total amount of memory allocated
int String::total_allocation = 0;
// whether to output constructor/destructor/operator= messages,
// initially false
bool String::messages_wanted = false;

// Default initialization is to contain an empty string with no allocation.
// If a non-empty C-string is supplied, this String gets minimum allocation.
String::String(const char* cstr_) {
  if (messages_wanted) {
    cout << "Ctor: \"" << cstr_ << "\"" << endl;
  }
  ++number;
  internal_c_str = &a_null_byte;
  allocation = 0;
  length = 0;
  int c_str_len = (int)strlen(cstr_);
  if (c_str_len > 0) {
    allocation = c_str_len + 1;
    internal_c_str = allocate(allocation);
    strcpy(internal_c_str, cstr_);
    length = c_str_len;
  }
}

// The copy constructor initializes this String with the original's data,
// and gets minimum allocation.
String::String(const String& original) {
  if (messages_wanted) {
    cout << "Copy ctor: \"" << original << "\"" << endl;
  }
  ++number;
  allocation = original.length + 1;
  internal_c_str = allocate(allocation);
  strcpy(internal_c_str, original.internal_c_str);
  length = original.length;
}

// Move constructor - take original's data, and set the original String
// member variables to the empty state (don't initialize "this" String and swap).
// Note: Should use move for non-primitive types in initializer's list
String::String(String&& original) noexcept {
  if (messages_wanted) {
    cout << "Move ctor: \"" << original << "\"" << endl;
  }
  ++number;
  swap(original);
  original.internal_c_str = &a_null_byte;
  original.allocation = 0;
  original.length = 0;
}

// deallocate C-string memory, noexcept means won't thrown exception
// Recall: Write destructor if object holds any resources(memory, handles etc)
String::~String() noexcept {
  if (messages_wanted) {
    cout << "Dtor: \"";
    if (allocation > 0) {
      cout << internal_c_str;
    }
    cout << "\"" << endl;
  }
  --number;
  deallocate();
}
    
// Assignment operators
// Left-hand side gets a copy of rhs data and gets minimum allocation.
// This operator use the copy-swap idiom for assignment.
    
// Copy swap is the better way for copy assignment!
String& String::operator= (const String& rhs) {
  if (messages_wanted) {
    cout << "Copy assign from String:  \"" << rhs << "\"" << endl;
  }
  // use copy constructor for copy assignment
  String temp(rhs);
  // swap this object's data with temp's data which was built from rhs
  swap(temp);
  return *this;
}

// Copy assignment - operator creates a temporary String object from the rhs
// C-string, and swaps the contents
String& String::operator= (const char* rhs) {
  if (messages_wanted) {
   cout << "Assign from C-string:  \"" << rhs << "\"" << endl;
  }
  String temp(rhs);
  swap(temp);
  return *this;
}

// Move assignment - simply swaps contents with rhs without any copying
String& String::operator= (String&& rhs) noexcept {
  if (messages_wanted) {
   cout << "Move assign from String:  \"" << rhs << "\"" << endl;
  }
  swap(rhs);
  return *this;
}
    
// Return a reference to character i in the string.
// Throw exception if 0 <= i < size is false.
char& String::operator[] (int i) {
  if(i < 0 || i >= length) {
   throw String_exception("Subscript out of range");
  }
  return internal_c_str[i];
}

// const version for const Strings
const char& String::operator[] (int i) const {
  if(i < 0 || i >= length) {
    throw String_exception("Subscript out of range");
  }
  return internal_c_str[i];
}
    
/* Return a String starting with i and extending for len characters
   The substring must be contained within the string.
   Values of i and len for valid input are as follows:
   i >= 0 && len >= 0 && i <= size && (i + len) <= size.
   If both i = size and len = 0, the input is valid and the result is an
   empty string. Throw exception if the input is invalid. 
 */
String String::substring(int i, int len) const {
  if (i < 0 || len < 0 || i > length || (i + len) > length) {
    throw String_exception("Substring bounds invalid");
  }
  // allocate a dynamic array based on len(input), dont forget null byte
  char* sub_str = new char[len + 1];
  for (int idx = i; idx < i + len; idx++) {
    // need to substract i to get correct index for sub_str
    sub_str[idx - i] = internal_c_str[idx];
  }
  sub_str[len] = '\0';
  // build own String from dynamic array of char
  String temp(sub_str);
  delete[] sub_str;
  return temp;
}
    
// Modifiers
// Set to an empty string with minimum allocation by create/swap with an
// empty string.
void String::clear() {
  String empty_str;
  swap(empty_str);
}
    
/* Remove the len characters starting at i; allocation is unchanged.
The removed characters must be contained within the String.
Valid values for i and len are the same as for substring. */
void String::remove(int i, int len) {
  if (i < 0 || len < 0 || i > length || (i + len) > length) {
    throw String_exception("Remove bounds invalid");
  }
  // dont forget null byte
  char* smaller_c_str = new char[length - len + 1];
  bool jump_index = false;
  for (int idx = 0; idx < length; idx++) {
    // jump the index to skip over "removed" characters
    if (i == idx) {
      idx += len;
      // need to substract len to get back to correct index in smaller_c_str
      smaller_c_str[idx - len] =internal_c_str[idx];
      jump_index = true;
      continue;
    }
    if (jump_index) {
      smaller_c_str[idx - len] = internal_c_str[idx];
    }
    else {
      smaller_c_str[idx] = internal_c_str[idx];
    }
  }
  // remove(size -1, 1) will be helped here by overwriting last bye with null
  smaller_c_str[length - len] = '\0';
  // copy from smaller_c_str into internal_c_str
  strcpy(internal_c_str, smaller_c_str);
  length -= len;
  delete[] smaller_c_str;
}
    
/* Insert the supplied source String before character i of this String,
 pushing the rest of the contents back, reallocating as needed.
 If i == size, the inserted string is added to the end of this String.
 This function does not create any temporary String objects.
 It either directly inserts the new data into this String's space 
 if it is big enough, or allocates new space and copies in the old
 data with the new data inserted. This String retains the final allocation.
 Throw exception if 0 <= i <= size is false. */
void String::insert_before(int i, const String& src) {
  // check for errors
  if (i < 0 || i > length) {
    throw String_exception("Insertion point out of range");
  }
  if (i == length)
  {
    *this += src;
    return;
  }
  int chars_to_add = (int)src.size();
  grow_by_n_char(src.size());
  // allocate dynamic array, with size = current length(size) + size of src + 1
  char* bigger_c_str = new char[length + chars_to_add + 1];
  // bounds cannot start from 0 in each for-loop
  for (int j = 0; j < i; j++) {
    bigger_c_str[j] = internal_c_str[j];
  }
  // start from i to NOT OVERLAP when copying into bigger_c_string
  for (int k = i; k < (i + chars_to_add); k++) {
    bigger_c_str[k] = src[k - i];
  }
  // length + src.size() is the new upperbound for bigger_c_str
  for (int x = (i + chars_to_add); x < length + chars_to_add; x++) {
    bigger_c_str[x] = internal_c_str[x - chars_to_add];
  }
  // dont forget to add null byte
  bigger_c_str[length + chars_to_add] = '\0';
  // copy bigger_c_str into internal_c_str safely
  strcpy(internal_c_str, bigger_c_str);
  length += chars_to_add;
  delete[] bigger_c_str;
}
    
/* These concatenation operators add the rhs string data to the lhs
   object. They do not create any temporary String objects. They
   either directly copy the rhs data into the lhs space if it is big
   enough to hold the rhs, or allocate new space and copy the old lhs
   data into it followed by the rhs data. The lhs object retains the
   final memory allocation. If the rhs is a null byte or an empty
   C-string or String, no change is made to lhs String. */
String& String::operator += (char rhs) {
  grow_by_n_char(1);
  internal_c_str[length] = rhs;
  ++length;
  return *this;
}
    

String& String::operator += (const char* rhs) {
  int chars_to_add = (int)strlen(rhs);
  grow_by_n_char(chars_to_add);
  strcpy(internal_c_str + length, rhs);
  length += chars_to_add;
  return *this;
}
    
    
String& String::operator += (const String& rhs) {
  grow_by_n_char(rhs.length);
  strcpy(internal_c_str + length, rhs.internal_c_str);
  length += rhs.length;
  return *this;
}
    
/* Swap the contents of this String with another one.
   The member variable values are interchanged, along with the pointers to the 
   allocated C-strings, but the two C-strings are neither copied nor modified.
   No memory allocation/deallocation is done. */
void String::swap(String& other) noexcept {
  std::swap(internal_c_str, other.internal_c_str);
  std::swap(allocation, other.allocation);
  std::swap(length, other.length);
}
    
// private helper function added,
void String::grow_by_n_char(int n) {
  if (allocation < length + n + 1) {
    int new_allocation = 2 * (length + n + 1);
    int old_length = length;
    char* new_str = allocate(new_allocation);
    if (allocation > 0)
    {
      strcpy(new_str, internal_c_str);
    }
    deallocate();
    internal_c_str = new_str;
    allocation = new_allocation;
    length = old_length;
  }
}
    
void String::deallocate() {
  if (internal_c_str && internal_c_str != &a_null_byte) {
    total_allocation -= allocation;
    delete[] internal_c_str;
  }
  allocation = 0;
  length = 0;
  internal_c_str = &a_null_byte;
}
    
char* String::allocate(int n) {
  total_allocation += n;
  char* new_str = new char[n];
  memset(new_str, '\0', n);
  return new_str;
}
// non-member overloaded operators
    
// compare lhs and rhs strings; constructor will convert a C-string
// literal to a String.
// comparison is based on std::strcmp result compared to 0
bool operator== (const String& lhs, const String& rhs) {
  return strcmp(lhs.c_str(), rhs.c_str()) == 0;
}
    
bool operator!= (const String& lhs, const String& rhs) {
  return strcmp(lhs.c_str(), rhs.c_str()) != 0;
}

bool operator< (const String& lhs, const String& rhs) {
  return strcmp(lhs.c_str(), rhs.c_str()) < 0;
}
    
bool operator> (const String& lhs, const String& rhs) {
  return strcmp(lhs.c_str(), rhs.c_str()) > 0;
}
    
/* Concatenate a String with another String.
 If one of the arguments is a C-string, the String constructor will
 automatically create a temporary String for it to match this function 
 (inefficient, but instructive). This automatic behavior would be disabled 
 if the String constructor was declared "explicit".
 This function constructs a copy of the lhs in a local String variable,
 then concatenates the rhs to it with operator +=, and returns it. */
String operator+ (const String& lhs, const String& rhs) {
  String temp(lhs);
  temp += rhs;
  return temp;
}
    
// Input and output operators and functions
// The output operator writes the contents of the String to the stream
ostream& operator<< (ostream& os, const String& str) {
  os << str.c_str();
  return os;
}
    
/* The input operator clears the supplied String, then starts reading
   the stream. It skips initial whitespace, then copies characters into
   the supplied str until whitespace is encountered again. The terminating
   whitespace remains in the input stream, analogous to how string
   input normally works. str is expanded as needed, and retains the
   final allocation. If the input stream fails, str contains whatever
   characters were read. */
istream& operator>> (istream& is, String& str) {
  str.clear();
  char reading_char;
  while (is && isspace(is.peek())) {
    // extract the whitespace characters that we don't want
    is.get(reading_char);
  }
  // dealing with non-whitespace characters now
  while (is.get(reading_char)) {
    str += reading_char;
    if (isspace(is.peek())) {
      // done cause next char peeked is whitespace
      break;
    }
  }
  return is;
}
    
/* getline for String clears str to an empty String, then reads characters into
 str until it finds a '\n'. Similar to std::getline, the newline character is
 consumed, but not stored in the String. str's allocation is expanded as needed,
 and it retains the final allocation. If the input stream fails, str contains
 whatever characters were read. */
istream& getline(istream& is, String& str) {
  str.clear();
  while (true) {
    char reading_char;
    if (!(is.get(reading_char))) {
      throw String_exception("getline failure");
    }
    if (reading_char != '\n') {
      str += reading_char;
    }
    else {
      break;
    }
  }
  is.unget();
  return is;
}
