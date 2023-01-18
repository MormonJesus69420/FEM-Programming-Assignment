#ifndef SCENARIO_H
#define SCENARIO_H

#include "application/gmlibwrapper.h"
#include "femmefatale/femobject.h"

// qt
#include <QObject>
#include <vector>

class Scenario : public GMlibWrapper {
  Q_OBJECT
  public:
  using GMlibWrapper::GMlibWrapper;

  void initializeScenario() override;
  void cleanupScenario() override;
  void simulate();

  private:
  std::vector<FemdomSlave::FEMObject*> victor;
};

#endif // SCENARIO_H
