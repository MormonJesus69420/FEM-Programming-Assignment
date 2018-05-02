#ifndef FEMOBJECT_H
#define FEMOBJECT_H

#include <QDebug>
#include <gmTrianglesystemModule>

#include "imports.h"
#include "node.h"

namespace FemdomSlave {

using Array = GMlib::ArrayLX<Node>;

class FEMObject : public TriangleFacets {
  public:
  // Constructors
  FEMObject()
      : TriangleFacets()
  {
  }

  // Operators
  void setSimulationSpeed(float simulation_speed);
  void initialize(float radius, int n, int m);
  void setOnlyRegular(bool only_regular);
  void setMaxForce(float max_force);
  void solveSystem();

  private:
  // Operators
  void regularTriangulation(float radius, int n, int m);
  void randomTriangulation(int e_nodes);
  void calculateNonDiagA(int i, int j);
  void calculateDiagAndB(int i);
  void calculate(int bound);

  // Members
  DMatrix stiffness_matrix;
  DVector load_vector;
  Array nodes;

  // Animation members
  bool only_regular = false;
  bool going_up = false;
  float simulation_speed = 2;
  float max_force = 0;
  float force = 0;

  protected:
  // Simulate method, used to animate simulation
  // I was informed about it by Bjørn-Richard Pedersen
  // Needed to update some stuff in guiapplication.h and guiapplication.cpp
  // That was told to me by Bjørn-Richard Pedersen too
  void localSimulate(double dt) override
  {
    // Update force value
    force += (max_force/simulation_speed * dt) * (going_up ? -1: 1);

    // Solve system using the new force value
    solveSystem();

    // Flip going_up if force is bigger than max force
    if (force > max_force || force <  -max_force)
      going_up = !going_up;
  }

}; // class FEMObject

} // namespace FemdomSlave

#endif // FEMOBJECT_H
