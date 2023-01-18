#include "femobject.h"
#include "imports.h"

#include <random>

namespace FemdomSlave {

// Public Operator
void FEMObject::initialize(float radius, int n, int m)
{
  // Triangulate regularly
  regularTriangulation(radius, n, m);

  // Triangulate randomly, n*m gives number of boundary nodes
  if (!only_regular)
    randomTriangulation(n * m);

  // Generate edges and triangles for all vertices
  triangulateDelaunay();

  // Calculate values for stiffness matrix and load vector, n*m gives number of boundary nodes
  calculate(n * m);
}

// Public Operator
void FEMObject::solveSystem()
{
  // Vector xi contains z values for each internal node
  DVector xi = stiffness_matrix * (force * load_vector);

  // Update z for internal nodes using xi vector
  for (int i = 0; i < nodes.size(); ++i)
    nodes[i].vertex->setZ(xi[i]);
}

// Private Operator
void FEMObject::regularTriangulation(float radius, int n, int m)
{
  // x increment value
  auto x = radius / m;

  // insert first vertex
  insertAlways(Vertex(0, 0, 0));

  // Calculate and insert remaining vertices starting in middle and working outwards
  for (int j = 1; j <= m; ++j) {
    for (int i = 0; i < n * j; ++i) {
      Angle alpha = i * (M_2PI / (n * j));
      auto R = SqMatrix(alpha);
      auto pos = R * Point(x * j, 0);
      insertAlways(Vertex(pos[0], pos[1]));
    }
  }

  // Reverse so that boundary vertices are on the beginning of array
  reverse();
}

// Private Operator
void FEMObject::randomTriangulation(int e_nodes)
{
  // Copy from uniform_int_distribution manual page.
  // Didn't use GMRandom, don't think it's random enough.
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> dis(e_nodes, size() - 1);

  // Decides how many times objects shall be swapped.
  auto swap_lim = size() * 3;

  // Swap objects, size * 2 seems to give a good mix without taking too much time.
  for (int i = 0; i < swap_lim; ++i)
    swap(dis(gen), dis(gen));

  // Decides how many objects will be removed, removes 75% of internal nodes.
  auto delete_lim = std::floor((size() - e_nodes) * 0.75);

  // Remove from back since it's cheapest.
  for (int i = 0; i < delete_lim; ++i)
    removeBack();
}

// Private Operator
void FEMObject::calculate(int bound)
{
  // Insert all INTERNAL vertices into nodes array
  for (int i = bound; i < size(); ++i)
    nodes.insertAlways(Node(getVertex(i)));

  // Initialize A and b
  auto size = nodes.size();
  stiffness_matrix.setDim(size, size);
  load_vector.setDim(size);

  // Calculate Stiffness Matrix and Load Vector
  for (int i = 0; i < size; ++i) {
    for (int j = 0; j < i; ++j)
      calculateNonDiagA(i, j);

    calculateDiagAndB(i);
  }

  // Invert the goddamn matrix you walnut.
  stiffness_matrix.invert();
}

// Private helper method for calculate
void FEMObject::calculateNonDiagA(int i, int j)
{
  Edge* edge = nodes[i].getNeighbour(nodes[j]);

  // If edge exists then calculate value, else set 0
  if (edge != NULL) {
    // Get the D
    Vector d = edge->getVector();
    // Get both triangles
    auto triangles = edge->getTriangle();
    // Vertices at P1 and P2, just so I don't call get vertex so much
    auto p0Vertex = edge->getFirstVertex();
    auto p1Vertex = edge->getLastVertex();
    // Triangle vertices
    auto t1vertices = triangles[0]->getVertices();
    auto t2vertices = triangles[1]->getVertices();

    // Points used to find a1 and a2
    Point p0 = p0Vertex->getParameter(), p2, p3;

    for (int i = 0; i < 3; i++) {
      // If vertice is not the P0 and P1 vertice then it's P2
      if (t1vertices[i] != p0Vertex
          && t1vertices[i] != p1Vertex)
        p2 = t1vertices[i]->getParameter();
      // If vertice is not the P0 and P1 vertice then it's P3
      if (t2vertices[i] != p0Vertex
          && t2vertices[i] != p1Vertex)
        p3 = t2vertices[i]->getParameter();
    }

    // Calculate helper values
    Vector a1 = p2 - p0;
    Vector a2 = p3 - p0;
    auto dd = 1.0f / std::abs(d * d);
    auto dh1 = dd * (a1 * d);
    auto dh2 = dd * (a2 * d);
    auto area1 = std::abs(d ^ a1);
    auto area2 = std::abs(d ^ a2);
    auto h1 = dd * area1 * area1;
    auto h2 = dd * area2 * area2;

    // Calculate value for stiffness matrix
    stiffness_matrix[j][i] = stiffness_matrix[i][j] = (dh1 * (1 - dh1) / h1 - dd) * (area1 / 2) + (dh2 * (1 - dh2) / h2 - dd) * (area2 / 2);
  }
  else
    stiffness_matrix[j][i] = stiffness_matrix[i][j] = 0;
}

// Private helper method for calculate
void FEMObject::calculateDiagAndB(int i)
{
  //Get triangles array for node and initialize variables.
  auto triangles = nodes[i].getTriangles();
  auto Tk = 0.0f, b = 0.0f;

  // Calculate Tk and b for each triangle
  for (int k = 0; k < triangles.size(); ++k) {
    // Get triangle and its vertices
    auto tVertices = triangles[k]->getVertices();

    // If vertex for node at i is the same as second vertex swap them.
    if (nodes[i].vertex == tVertices[1])
      tVertices.swap(0, 1);
    // If vertex for node at i is the same as third vertex swap them.
    if (nodes[i].vertex == tVertices[2])
      tVertices.swap(0, 2);

    // Now vertices are in correct order, use them to get points p0, p1 and p2
    auto p0 = tVertices[0]->getParameter();
    auto p1 = tVertices[1]->getParameter();
    auto p2 = tVertices[2]->getParameter();

    // Calculate vectors d0, d1 and d2
    Vector d0 = p1 - p0;
    Vector d1 = p2 - p0;
    Vector d2 = p2 - p1;

    // Sum Tk values and areas in b
    Tk += (d2 * d2) / std::abs(d0 ^ d1) * 0.5;
    b += triangles[k]->getArea2D();
  }

  // Set Tk to matrix and b to vector.
  stiffness_matrix[i][i] = Tk;
  load_vector[i] = b / 3;
}

// Helper method for animation
void FEMObject::setMaxForce(float max_force)
{
  this->max_force = max_force;
}

// Helper method for animation
void FEMObject::setSimulationSpeed(float simulation_speed)
{
  this->simulation_speed = simulation_speed;
}

// Helper method for animation
void FEMObject::setOnlyRegular(bool only_regular)
{
  this->only_regular = only_regular;
}

} // namespace FemdomSlave
