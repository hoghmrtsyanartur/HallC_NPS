/*
 * G4Helper.hh
 *
 *  Created on: Dec 18, 2020
 *      Author: petrstepanov
 */

#ifndef SRC_G4HELPER_HH_
#define SRC_G4HELPER_HH_
#include <G4Types.hh>

class G4Helper {
public:
  static G4Helper* getInstance();
  virtual ~G4Helper();

  void add2DText(const char* text);

private:
  G4Helper();
  static G4Helper* instance;

  G4int lineNumber;
};

#endif /* SRC_G4HELPER_HH_ */
