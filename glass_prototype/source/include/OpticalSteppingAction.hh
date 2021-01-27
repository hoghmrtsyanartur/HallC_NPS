/*
 * OpticalSteppingAction.h
 *
 *  Created on: Nov 24, 2020
 *      Author: petrstepanov
 */

#ifndef SRC_OpticalSteppingAction_H_
#define SRC_OpticalSteppingAction_H_

#include <G4UserSteppingAction.hh>
#include "HistoManager.hh"
#include "OpticalEventAction.hh"
#include <G4Step.hh>

class OpticalSteppingAction: public G4UserSteppingAction {
public:
  OpticalSteppingAction(HistoManager* histoManager, OpticalEventAction* oea);
  virtual ~OpticalSteppingAction();

  void UserSteppingAction(const G4Step* aStep);

private:
  HistoManager* fHistoManager;
  OpticalEventAction* fOpticalEventAction;
};

#endif /* SRC_OpticalSteppingAction_H_ */
