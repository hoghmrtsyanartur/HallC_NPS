/*
 * VisHelper.hh
 *
 *  Created on: Dec 18, 2020
 *      Author: petrstepanov
 */

#ifndef SRC_VisHelper_HH_
#define SRC_VisHelper_HH_

#include <G4Types.hh>
#include <G4VVisManager.hh>

class VisHelper {
public:
  static VisHelper* getInstance();
  virtual ~VisHelper();

  void draw2DText(const char* text);
  void drawSatistics();
  void exportImage(G4String extension);

private:
  VisHelper();
  static VisHelper* instance;

  G4VVisManager* getVisManager();

  G4int lineNumber;
};

#endif /* SRC_VisHelper_HH_ */
