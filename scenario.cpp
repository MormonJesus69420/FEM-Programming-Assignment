#include "scenario.h"

//// hidmanager
//#include "hidmanager/defaulthidmanager.h"

// gmlib
#include <gmOpenglModule>
#include <gmParametricsModule>
#include <gmSceneModule>

// qt
#include "femmefatale/femobject.h"
#include <QQuickItem>

void Scenario::initializeScenario()
{
  // Insert a light
  GMlib::Point<GLfloat, 3> init_light_pos(2.0, 4.0, 10);
  GMlib::PointLight* light = new GMlib::PointLight(GMlib::GMcolor::white(), GMlib::GMcolor::white(),
      GMlib::GMcolor::white(), init_light_pos);
  light->setAttenuation(0.8, 0.002, 0.0008);
  scene()->insertLight(light, false);

  // Insert Sun
  scene()->insertSun();

  // Default camera parameters
  int init_viewport_size = 600;
  GMlib::Point<float, 3> init_cam_pos(0.0f, 0.0f, 0.0f);
  GMlib::Vector<float, 3> init_cam_dir(0.0f, 1.0f, 0.0f);
  GMlib::Vector<float, 3> init_cam_up(0.0f, 0.0f, 1.0f);

  // Projection cam
  auto proj_rcpair = createRCPair("Projection");
  proj_rcpair.camera->set(init_cam_pos, init_cam_dir, init_cam_up);
  proj_rcpair.camera->setCuttingPlanes(1.0f, 8000.0f);
  proj_rcpair.camera->rotateGlobal(GMlib::Angle(-45), GMlib::Vector<float, 3>(1.0f, 0.0f, 0.0f));
  proj_rcpair.camera->translateGlobal(GMlib::Vector<float, 3>(0.0f, -20.0f, 20.0f));
  scene()->insertCamera(proj_rcpair.camera.get());
  proj_rcpair.renderer->reshape(GMlib::Vector<int, 2>(init_viewport_size, init_viewport_size));

  // Front cam
  auto front_rcpair = createRCPair("Front");
  front_rcpair.camera->set(init_cam_pos + GMlib::Vector<float, 3>(0.0f, -50.0f, 0.0f), init_cam_dir, init_cam_up);
  front_rcpair.camera->setCuttingPlanes(1.0f, 8000.0f);
  scene()->insertCamera(front_rcpair.camera.get());
  front_rcpair.renderer->reshape(GMlib::Vector<int, 2>(init_viewport_size, init_viewport_size));

  // Side cam
  auto side_rcpair = createRCPair("Side");
  side_rcpair.camera->set(init_cam_pos + GMlib::Vector<float, 3>(-50.0f, 0.0f, 0.0f), GMlib::Vector<float, 3>(1.0f, 0.0f, 0.0f), init_cam_up);
  side_rcpair.camera->setCuttingPlanes(1.0f, 8000.0f);
  scene()->insertCamera(side_rcpair.camera.get());
  side_rcpair.renderer->reshape(GMlib::Vector<int, 2>(init_viewport_size, init_viewport_size));

  // Top cam
  auto top_rcpair = createRCPair("Top");
  top_rcpair.camera->set(init_cam_pos + GMlib::Vector<float, 3>(0.0f, 0.0f, 50.0f), -init_cam_up, init_cam_dir);
  top_rcpair.camera->setCuttingPlanes(1.0f, 8000.0f);
  scene()->insertCamera(top_rcpair.camera.get());
  top_rcpair.renderer->reshape(GMlib::Vector<int, 2>(init_viewport_size, init_viewport_size));

  //////////////////////////////////////////////
  //////////////// Testing Area ////////////////
  //////////////////////////////////////////////
  auto fem1 = new FemdomSlave::FEMObject();
  fem1->initialize(5, 7, 15);
  fem1->setMaxForce(0.5);
  fem1->setSimulationSpeed(3);
  fem1->toggleDefaultVisualizer();
  fem1->setMaterial(GMlib::GMmaterial::sapphire());
  scene()->insert(fem1);

  auto fem2 = new FemdomSlave::FEMObject();
  fem2->translateGlobal(GMlib::Vector<float, 3>(10.f, 0.0f, 0.0f));
  fem2->setOnlyRegular(true);
  fem2->initialize(3, 5, 10);
  fem2->setMaxForce(1);
  fem2->setSimulationSpeed(1);
  fem2->toggleDefaultVisualizer();
  fem2->setMaterial(GMlib::GMmaterial::ruby());
  scene()->insert(fem2);

  auto fem3 = new FemdomSlave::FEMObject();
  fem3->translateGlobal(GMlib::Vector<float, 3>(-10.f, 0.0f, 0.0f));
  fem3->initialize(3, 5, 25);
  fem3->setMaxForce(4);
  fem3->setSimulationSpeed(5);
  fem3->toggleDefaultVisualizer();
  fem3->setMaterial(GMlib::GMmaterial::emerald());
  scene()->insert(fem3);

  victor.push_back(fem1);
  victor.push_back(fem2);
  victor.push_back(fem3);
  //////////////////////////////////////////////
}

void Scenario::cleanupScenario()
{
}

void Scenario::simulate()
{
  for (FemdomSlave::FEMObject* fem : victor) {
    fem->replot();
    fem->computeNormals();
  }
}
