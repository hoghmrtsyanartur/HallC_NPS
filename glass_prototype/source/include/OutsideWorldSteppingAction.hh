/*
 * OutsideWorldSteppingAction.h
 *
 *  Created on: Nov 24, 2020
 *      Author: petrstepanov
 */

#ifndef SRC_OUTSIDEWORLDSTEPPINGACTION_H_
#define SRC_OUTSIDEWORLDSTEPPINGACTION_H_

#include <G4UserSteppingAction.hh>
#include "HistoManager.hh"
#include <G4Step.hh>

class OutsideWorldSteppingAction: public G4UserSteppingAction {
public:
  OutsideWorldSteppingAction(HistoManager* histoManager);
  virtual ~OutsideWorldSteppingAction();

  void UserSteppingAction(const G4Step* aStep);

private:
  HistoManager* fHistoManager;
};

#endif /* SRC_OUTSIDEWORLDSTEPPINGACTION_H_ */
