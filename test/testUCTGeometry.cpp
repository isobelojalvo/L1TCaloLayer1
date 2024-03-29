#include <iostream>
#include <stdlib.h>
#include <stdint.h>

using namespace std;

#include "L1Trigger/L1TCaloLayer1/src/UCTGeometry.hh"

int main(int argc, char** argv) {
  UCTGeometry g;
  for(int caloPhi = 1; caloPhi <= MaxCaloPhi; caloPhi++) {
    for(int caloEta = -MaxCaloEta; caloEta <= MaxCaloEta; caloEta++) {
      if(caloEta == 0 || abs(caloEta) == 29) continue;
      else if(abs(caloEta) > 39 && caloPhi > MaxCaloPhiInVHF) continue;
      else if(abs(caloEta) > 29 && caloPhi > MaxCaloPhiInHF) continue;
      uint32_t crt = g.getCrate(caloEta, caloPhi);
      uint32_t crd = g.getCard(caloEta, caloPhi);
      uint32_t rgn = g.getRegion(caloEta, caloPhi);
      uint32_t eta = g.getiEta(caloEta, caloPhi);
      uint32_t phi = g.getiPhi(caloEta, caloPhi);
      int cEta = g.getCaloEtaIndex((caloEta < 0), rgn, eta);
      int cPhi = g.getCaloPhiIndex(crt, crd, rgn, phi);
      if(cEta != caloEta || cPhi != caloPhi) {
	cerr << "(caloEta, caloPhi) = (" << caloEta << ", " << caloPhi << ") "
	     << "(crt,crd,rgn,eta,phi) = ("
	     << crt << ", " << crd << ", " << rgn << ", " << eta << ", " << phi << ")" << endl; 
	cerr << "(caloEta, caloPhi) = (" << cEta << ", " << cPhi << ") " 
	     << "Obtained instead   " << endl;
      }
      double realEta = g.getUCTTowerEta(caloEta);
      double realPhi = g.getUCTTowerPhi(caloPhi, caloEta);
      cout << "(caloEta, caloPhi) = (" << caloEta << ", " << caloPhi << ") ;" 
	   << "(realEta, realPhi) = (" << realEta << ", " << realPhi << ") "
	   << endl;
    }
  }
  return 0;
}
