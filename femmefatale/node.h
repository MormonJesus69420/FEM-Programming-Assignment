#ifndef NODE_H
#define NODE_H

#include <gmTrianglesystemModule>
#include <memory>
#include <vector>

#include "imports.h"

namespace FemdomSlave {

using TriangleArray = GMlib::Array<Triangle*>;

class Node {
  public:
  // Constructors
  Node();
  Node(Vertex* vertex);

  // Members
  Vertex* vertex;

  // Accessors
  TriangleArray getTriangles();
  Edge* getNeighbour(const Node& node);

  // Mutators
  void setZ(float z);

  //Operator overload
  bool operator==(const Node& n) const;
}; // struct Node

} // namespace FemdomSlave

#endif // NODE_H
