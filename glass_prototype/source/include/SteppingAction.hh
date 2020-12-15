/*
 * SteppingAction.h
 *
 *  Created on: Nov 24, 2020
 *      Author: petrstepanov
 */

#ifndef SRC_SteppingAction_H_
#define SRC_SteppingAction_H_

#include <G4UserSteppingAction.hh>
#include "HistoManager.hh"
#include <G4Step.hh>

class SteppingAction: public G4UserSteppingAction {
public:
  SteppingAction(HistoManager* histoManager);
  virtual ~SteppingAction();

  void UserSteppingAction(const G4Step* aStep);

private:
  HistoManager* fHistoManager;
};

#endif /* SRC_SteppingAction_H_ */
