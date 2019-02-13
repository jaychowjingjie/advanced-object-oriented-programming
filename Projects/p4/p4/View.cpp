#include "View.h"
#include "Geometry.h"
#include "Utility.h"
#include <string>
#include <iostream>
#include <iomanip>
#include <vector>
#include <map>
#include <functional>
#include <algorithm>
#include <cmath>

using namespace std;
using namespace placeholders;

const int VIEW_DEFAULT_SIZE = 25;
const double VIEW_DEFAULT_SCALE = 2;
const Point VIEW_DEFAULT_ORIGIN(-10, -10);
const string VIEW_MULTIPLE_OBJECT = "* ";
const string VIEW_NO_OBJECT = ". ";
const int VIEW_LINES_PER_AXIS_LABEL = 3;
const int VIEW_AXIS_LABEL_MAX = 4;

// default constructor sets the default size, scale, and origin, outputs constructor message
View::View()
{
  set_defaults();
  cout << "View constructed" << endl;
}
// outputs destructor message
View::~View()
{
  cout << "View destructed" << endl;
}

// Save the supplied name and location for future use in a draw() call
// If the name is already present,the new location replaces the previous one.
void View::update_location(const std::string& name, Point location)
{
  object_data[name] = location;
}

// Remove the name and its location; no error if the name is not present.
void View::update_remove(const std::string& name)
{
  auto object_it = object_data.find(name);
  if (object_it != object_data.end()) object_data.erase(object_it);
}

// prints out the current map
void View::draw()
{
  // save precision
  int old_precision = cout.precision();
  
  vector<vector<string>> map_view;
  vector<string> objects_out_of_map;
  // a row in the map must start out filled only with VIEW_NO_OBJECTs
  vector<string> initial_row;
  for (int i = 0; i < size; i++) initial_row.push_back(VIEW_NO_OBJECT);
  // fill the map with copies of the initial row
  for (int i = 0; i < size; i++) map_view.push_back(vector<string>(initial_row));
  
  // generate the map
  // each object in object_data is a pair where first is the name and second is the Point
  for (auto&& object : object_data)
  {
    int x, y;
    if (get_subscripts(x, y, object.second))
    {
      if (map_view[y][x] == VIEW_NO_OBJECT) map_view[y][x] = object.first.substr(0, SHORTEN_NAME_LENGTH);
      else map_view[y][x] = VIEW_MULTIPLE_OBJECT;
    } else objects_out_of_map.push_back(object.first);
  }
  
  cout << "Display size: " << size << ", scale: " << scale << ", origin: " << origin << endl;
  cout.precision(0);
  if (objects_out_of_map.size() > 0)
  {
    for (size_t i = 0; i < objects_out_of_map.size() - 1; i++)
    {
      cout << objects_out_of_map[i] << ", ";
    }
    cout << objects_out_of_map[objects_out_of_map.size() - 1] << " outside the map" << endl;
  }
  // start from max y and iterate through map
  for (int y = size - 1; y >= 0; y--)
  {
    if (y % VIEW_LINES_PER_AXIS_LABEL == 0)
    {
      cout << setw(VIEW_AXIS_LABEL_MAX) << (origin.y + scale * y) << " ";
    } else cout << setw(VIEW_AXIS_LABEL_MAX + 1) << " ";
    // iterate through the map of this row
    for (int x = 0; x < size; x++)
    {
      cout << map_view[y][x];
    }
    cout << endl;
  }
  for (int x = 0; x < size; x += VIEW_LINES_PER_AXIS_LABEL)
  {
    cout << setw(SHORTEN_NAME_LENGTH * VIEW_LINES_PER_AXIS_LABEL) << (origin.x + scale * x);
  }
  cout << endl;
  
  // restore precision
  cout.precision(old_precision);
}

// modify the display parameters
// if the size is out of bounds will throw Error("New map size is too big!")
// or Error("New map size is too small!")
void View::set_size(int size_)
{
  if (size_ <= 6) throw Error("New map size is too small!");
  if (size_ > 30) throw Error("New map size is too big!");
  size = size_;
}

// If scale is not positive, will throw Error("New map scale must be positive!");
void View::set_scale(double scale_)
{
  if (scale_ <= 0) throw Error("New map scale must be positive!");
  scale = scale_;
}

// set the parameters to the default values
void View::set_defaults()
{
  size = VIEW_DEFAULT_SIZE;
  scale = VIEW_DEFAULT_SCALE;
  origin = VIEW_DEFAULT_ORIGIN;
}

// Calculate the cell subscripts corresponding to the supplied location parameter,
// using the current size, scale, and origin of the display.
// This function assumes that origin is a  member variable of type Point,
// scale is a double value, and size is an integer for the number of rows/columns
// currently being used for the grid.
// Return true if the location is within the grid, false if not
bool View::get_subscripts(int &ix, int &iy, Point location)
{
  // adjust with origin and scale
  Cartesian_vector subscripts = (location - origin) / scale;
  // truncate coordinates to integer after taking the floor
  // floor function will produce integer smaller than even for negative values,
  // so - 0.05 => -1., which will be outside the array.
  ix = int(floor(subscripts.delta_x));
  iy = int(floor(subscripts.delta_y));
  // if out of range, return false
  if ((ix < 0) || (ix >= size) || (iy < 0) || (iy >= size))
    return false;
  else
    return true;
}
