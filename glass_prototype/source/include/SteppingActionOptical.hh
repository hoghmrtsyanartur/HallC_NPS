/*
 * SteppingActionOptical.h
 *
 *  Created on: Nov 24, 2020
 *      Author: petrstepanov
 */

#ifndef SRC_SteppingActionOptical_H_
#define SRC_SteppingActionOptical_H_

#include <G4UserSteppingAction.hh>
#include "HistoManager.hh"
#include <G4Step.hh>

class SteppingActionOptical: public G4UserSteppingAction {
public:
  SteppingActionOptical(HistoManager* histoManager);
  virtual ~SteppingActionOptical();

  void UserSteppingAction(const G4Step* aStep);

private:
  HistoManager* fHistoManager;
};

#endif /* SRC_SteppingActionOptical_H_ */
