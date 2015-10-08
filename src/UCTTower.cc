#include <iostream>
#include <iomanip>
#include <string>
#include <math.h>
#include <stdlib.h>
#include <stdint.h>

#include "UCTTower.hh"

bool UCTTower::process() {
  towerData = ecalET + hcalET;
  if(towerData > etMask) towerData = etMask;
  uint32_t er = 0;
  if(ecalET == 0 || hcalET == 0) {
    er = 0;
    towerData |= zeroFlagMask;
    if(hcalET == 0)
      towerData |= eohrFlagMask;
  }
  else if(ecalET == hcalET) {
    er = 0;
    towerData |= eohrFlagMask;
  }
  else if(ecalET > hcalET) {
    er = (uint32_t) log2(((double) ecalET) / ((double) hcalET));
    if(er > erMaxV) er = erMaxV;
    towerData |= eohrFlagMask;
  }
  else {
    er = (uint32_t) log2(((double) hcalET) / ((double) ecalET));
    if(er > erMaxV) er = erMaxV;
  }
  towerData |= (er >> erShift);
  // Unfortunately, hcalFlag is presently bogus :(
  // It has never been studied nor used in Run-1
  // The same status persists in Run-2, but it is available usage
  // Currently, summarize all hcalFeatureBits in one flag bit
  if(hcalFB != 0) towerData |= hcalFlagMask;
  if(ecalFG) towerData |= ecalFlagMask;
  // Store ecal and hcal calibrated ET in unused upper bits
  towerData |= (ecalET << ecalShift);
  towerData |= (hcalET << hcalShift);
  // All done!
  return true;
}

bool UCTTower::setECALData(bool eFG, uint32_t eET) {
  ecalFG = eFG;
  ecalET = eET;
  if(eET > 0xFF) {
    std::cerr << "UCTTower::setData - ecalET too high " << eET << "; Pegged to 0xFF" << std::endl;
    ecalET = 0xFF;
  }
  return true;
}

bool UCTTower::setHCALData(uint32_t hET, uint32_t hFB) {
  hcalET = hET;
  hcalFB = hFB;
  if(hET > 0xFF) {
    std::cerr << "UCTTower::setData - ecalET too high " << hET << "; Pegged to 0xFF" << std::endl;
    hcalET = 0xFF;
  }
  if(hFB > 0x1F) {
    std::cerr << "UCTTower::setData - too many hcalFeatureBits " << std::hex << hFB 
	      << "; Used only bottom 5 bits" << std::endl;
    hcalFB &= 0x1F;
  }
  return true;
}

const uint16_t UCTTower::location() const {
  uint16_t l = 0;
  if(negativeEta) l = 0x8000; // Used top bit for +/- eta-side
  l |= iPhi;                  // Max iPhi is 4, so bottom 2 bits for iPhi
  l |= (iEta   << 2);         // Max iEta is 4, so 2 bits needed
  l |= (region << 4);         // Max region number 14, so 4 bits needed
  l |= (card   << 8);         // Max card number is 6, so 3 bits needed
  l |= (crate  << 11);        // Max crate number is 2, so 2 bits needed
  return l;
}

UCTTower::UCTTower(uint16_t location) {
  if((location & 0x8000) != 0) negativeEta = true;
  crate =  (location & 0x1800) >> 11;
  card =   (location & 0x0700) >>  8;
  region = (location & 0x00F0) >>  4;
  iEta =   (location & 0x000C) >>  2;
  iPhi =   (location & 0x0003);
  towerData = 0;
}

const uint64_t UCTTower::extendedData() const {
  uint64_t d = rawData();
  uint64_t l = location();
  uint64_t r = (l << 48) + d;
  return r;
}

void UCTTower::print(bool header) {
  if((ecalET + hcalET) == 0) return;
  if(header) {
    std::cout << "Side Crt  Crd  Rgn  iEta iPhi cEta cPhi eET  eFG  hET  hFB  Summary" << std::endl;
  }
  UCTGeometry g;
  std::string side = "+eta ";
  if(negativeEta) side = "-eta ";
  std::cout << side
	    << std::showbase << std::internal << std::setfill('0') << std::setw(4) << std::hex
	    << crate << " "
	    << std::showbase << std::internal << std::setfill('0') << std::setw(4) << std::hex
	    << card << " "
	    << std::showbase << std::internal << std::setfill('0') << std::setw(4) << std::hex
	    << region << " "
	    << std::showbase << std::internal << std::setfill('0') << std::setw(4) << std::hex
	    << iEta << " "
	    << std::showbase << std::internal << std::setfill('0') << std::setw(4) << std::hex
	    << iPhi << " "
	    << std::showbase << std::internal << std::setfill('0') << std::setw(4) << std::hex
	    << g.getCaloEtaIndex(negativeEta, region, iEta) << " "
	    << std::showbase << std::internal << std::setfill('0') << std::setw(4) << std::hex
	    << g.getCaloPhiIndex(crate, card, region, iPhi) << " "
	    << std::showbase << std::internal << std::setfill('0') << std::setw(4) << std::hex
	    << ecalET << " "
	    << std::showbase << std::internal << std::setfill('0') << std::setw(4) << std::hex
	    << ecalFG << " "
	    << std::showbase << std::internal << std::setfill('0') << std::setw(4) << std::hex
	    << hcalET << " "
	    << std::showbase << std::internal << std::setfill('0') << std::setw(4) << std::hex
	    << hcalFB << " "
	    << std::showbase << std::internal << std::setfill('0') << std::setw(10) << std::hex
	    << towerData
	    << std::endl;
}
