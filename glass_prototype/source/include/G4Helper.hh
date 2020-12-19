/*
 * G4Helper.hh
 *
 *  Created on: Dec 18, 2020
 *      Author: petrstepanov
 */

#ifndef SRC_G4HELPER_HH_
#define SRC_G4HELPER_HH_

#include <G4Types.hh>
#include <G4VVisManager.hh>

class G4Helper {
public:
  static G4Helper* getInstance();
  virtual ~G4Helper();

  void add2DText(const char* text);
  void drawSatistics();

private:
  G4Helper();
  static G4Helper* instance;

  G4VVisManager* getVisManager();

  G4int lineNumber;
};

#endif /* SRC_G4HELPER_HH_ */
