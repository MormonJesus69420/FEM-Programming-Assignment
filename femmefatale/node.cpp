#include "node.h"

namespace FemdomSlave {

// Constructor
Node::Node()
{
}

// Constructor
Node::Node(Vertex* vertex)
{
  this->vertex = vertex;
}

// Accessornew
TriangleArray Node::getTriangles()
{
  return vertex->getTriangles();
}

// Accessor
Edge* Node::getNeighbour(const Node& node)
{
  auto edges = vertex->getEdges();

  for (int i = 0; i < edges.size(); i++)
    if (edges(i)->getOtherVertex(*vertex) == node.vertex)
      return edges(i);

  return NULL;
}

// Mutator
void Node::setZ(float z)
{
  vertex->setZ(z);
}

// == operator override
bool Node::operator==(const Node& n) const
{
  return vertex == n.vertex;
}

} // namespace FemdomSlave
