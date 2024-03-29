#include <iostream>
#include <stdlib.h>
#include <stdint.h>

#include "UCTGeometry.hh"

uint32_t UCTGeometry::getLinkNumber(bool negativeEta, uint32_t region, 
				    uint32_t iEta, uint32_t iPhi) {
  if(checkRegion(region)) {
    std::cerr << "Invalid region number: region = " << region << std::endl;
    exit(1);
  }
  if(checkEtaIndex(region, iEta)) {
    std::cerr << "Invalid eta index: iEta = " << iEta << std::endl;
    exit(1);
  }
  if(checkPhiIndex(region, iPhi)) {
    std::cerr << "Invalid eta index: iPhi = " << iPhi << std::endl;
    exit(1);
  }
  uint32_t linkNumber = 0xDEADBEEF;
  if(region < MaxRegionNumber) {
    if(iEta < NEtaInRegion / 2) {
      linkNumber = region * 2;
    }
    else {
      linkNumber = region * 2 + 1;
    }
  }
  else {
    linkNumber = NRegionsInCard * 2 + iPhi;
  }

  if(!negativeEta) {
    linkNumber += NRegionsInCard * 2 + 2;
  }
  return linkNumber;
}

int UCTGeometry::getCaloEtaIndex(bool negativeSide, uint32_t region, uint32_t iEta) {

  if(checkRegion(region)) {
    std::cerr << "Invalid region number: region = " << region << std::endl;
    exit(1);
  }
  if(checkEtaIndex(region, iEta)) {
    std::cerr << "Invalid eta index: iEta = " << iEta << std::endl;
    exit(1);
  }

  int caloEtaIndex = region * NEtaInRegion + iEta + 1;
  if(region > 6) {
    caloEtaIndex = (region - 7) * NHFEtaInRegion + iEta + 30;
  }

  if(negativeSide) return -caloEtaIndex;
  return caloEtaIndex;

}

int UCTGeometry::getCaloPhiIndex(uint32_t crate, uint32_t card, 
				 uint32_t region, uint32_t iPhi) {
  if(checkCrate(crate)) {
    std::cerr << "Invalid crate number: crate = " << crate << std::endl;
    exit(1);
  }
  if(checkCard(card)) {
    std::cerr << "Invalid card number: card = " << card << std::endl;
    exit(1);
  }
  if(checkPhiIndex(region, iPhi)) {
    std::cerr << "Invalid phi index: iPhi = " << iPhi << std::endl;
    exit(1);
  }
  int caloPhiIndex = 0xDEADBEEF;
  if(region < CaloHFRegionStart) {
    if(crate == 0) {
      caloPhiIndex = 11 + card * 4 + iPhi;
    }
    else if(crate == 1) {
      caloPhiIndex = 59 + card * 4 + iPhi;
    }
    else if(crate == 2) {
      caloPhiIndex = 35 + card * 4 + iPhi;
    }
    if(caloPhiIndex > 72) caloPhiIndex -= 72;
  }
  else if(region < CaloVHFRegionStart) {
    if(crate == 0) {
      caloPhiIndex = 6 + card * 2 + iPhi;
    }
    else if(crate == 1) {
      caloPhiIndex = 30 + card * 2 + iPhi;
    }
    else if(crate == 2) {
      caloPhiIndex = 18 + card * 2 + iPhi;
    }
    if(caloPhiIndex > 36) caloPhiIndex -= 36;
  }
  else {
    if(crate == 0) {
      caloPhiIndex = 3 + card;
    }
    else if(crate == 1) {
      caloPhiIndex = 15 + card;
    }
    else if(crate == 2) {
      caloPhiIndex = 9 + card;
    }
    if(caloPhiIndex > 18) caloPhiIndex -= 18;
  }
  return caloPhiIndex;
}

uint32_t UCTGeometry::getUCTRegionPhiIndex(uint32_t crate, uint32_t card) {
  if(checkCrate(crate)) {
    std::cerr << "Invalid crate number: crate = " << crate << std::endl;
    exit(1);
  }
  if(checkCard(card)) {
    std::cerr << "Invalid card number: card = " << card << std::endl;
    exit(1);
  }
  uint32_t uctRegionPhiIndex = 0xDEADBEEF;
  if(crate == 0) {
    uctRegionPhiIndex = 3 + card;
  }
  else if(crate == 1) {
    if(card < 3) {
      uctRegionPhiIndex = 15 + card;
    }
    else {
      uctRegionPhiIndex = card - 3;
    }
  }
  else if(crate == 2) {
    uctRegionPhiIndex = 9 + card;
  }
  return uctRegionPhiIndex;
}

uint32_t UCTGeometry::getCrate(int caloEta, int caloPhi) {
  uint32_t crate = 0xDEADBEEF;
  uint32_t region = getRegion(caloEta, caloPhi);
  uint32_t cPhi = caloPhi;
  if(region >= CaloVHFRegionStart) {
    cPhi = caloPhi * 4;
  }
  else if(region >= CaloHFRegionStart) {
    cPhi = caloPhi * 2;
  }
  if(cPhi >= 11 && cPhi <= 34) crate = 0;
  else if(cPhi >= 35 && cPhi <= 58) crate = 2;
  else if(cPhi >= 59 && cPhi <= 72) crate = 1;
  else if(cPhi >= 1 && cPhi <= 10) crate = 1;  
  return crate;
}

uint32_t UCTGeometry::getCard(int caloEta, int caloPhi) {
  uint32_t crate = getCrate(caloEta, caloPhi);
  uint32_t region = getRegion(caloEta, caloPhi);
  uint32_t card = 0xDEADBEEF;
  uint32_t cPhi = caloPhi;
  if(region >= CaloVHFRegionStart) {
    cPhi = caloPhi * 4;
  }
  else if(region >= CaloHFRegionStart) {
    cPhi = caloPhi * 2;
  }
  if(crate == 0) {
    card = (cPhi - 11) / 4;
  }
  else if(crate == 2) {
    card = (cPhi - 35) / 4;
  }
  else if(crate == 1 && cPhi > 58) {
    card = (cPhi - 59) / 4;
  }
  else if(crate == 1 && cPhi <= 10) {
    card = (cPhi + 13) / 4;
  }    
  return card;
}

uint32_t UCTGeometry::getRegion(int caloEta, int caloPhi) {
  uint32_t absCEta = abs(caloEta);
  if((absCEta - 1) < (NRegionsInCard * NEtaInRegion))
    return (absCEta - 1) / NEtaInRegion;
  else
    return NRegionsInCard + ((absCEta - 2 - (NRegionsInCard * NEtaInRegion)) / NHFEtaInRegion);
}

uint32_t UCTGeometry::getiEta(int caloEta, int caloPhi) {
  uint32_t absCEta = abs(caloEta);
  if((absCEta - 1) < (NRegionsInCard * NEtaInRegion))
    return (absCEta - 1) % NEtaInRegion;
  else
    return absCEta % NHFEtaInRegion;  // To account for missing tower 29
}

uint32_t UCTGeometry::getiPhi(int caloEta, int caloPhi) {
  uint32_t region = getRegion(caloEta, caloPhi);
  uint32_t iPhi = 0xDEADBEEF;
  if(region < CaloHFRegionStart && caloPhi <= MaxCaloPhi) {
    iPhi = (caloPhi + 1) % NPhiInCard;
  }
  else if(region < CaloVHFRegionStart && caloPhi <= MaxCaloPhiInHF) {
    iPhi = caloPhi % NHFPhiInCard;
  }
  else {
    iPhi = (caloPhi + 1) % NVHFPhiInCard;
  }
  return iPhi;
}

uint32_t UCTGeometry::getNEta(uint32_t region) {
  uint32_t nEta = 0xDEADBEEF;
  if(region < CaloHFRegionStart) {
    nEta = 4;
  }
  else {
    nEta = 2;
  }
  return nEta;
}

uint32_t UCTGeometry::getNPhi(uint32_t region) {
  uint32_t nPhi = 0xDEADBEEF;
  if(region < CaloHFRegionStart) {
    nPhi = 4;
  }
  else if(region < CaloVHFRegionStart) {
    nPhi = 2;
  }
  else {
    nPhi = 1;
  }
  return nPhi;
}

UCTRegionIndex UCTGeometry::getUCTRegionIndex(int caloEta, int caloPhi) {
  uint32_t regionPhi = getUCTRegionPhiIndex(getCrate(caloEta, caloPhi), getCard(caloEta, caloPhi));
  int regionEta = getUCTRegionEtaIndex((caloEta < 0), getRegion(caloEta, caloPhi));
  return UCTRegionIndex(regionEta, regionPhi);
}

UCTRegionIndex UCTGeometry::getUCTRegionIndex(bool negativeSide, uint32_t crate, uint32_t card, uint32_t region) {
  uint32_t regionPhi = getUCTRegionPhiIndex(crate, card);
  int regionEta = getUCTRegionEtaIndex(negativeSide, region);
  return UCTRegionIndex(regionEta, regionPhi);
}

UCTTowerIndex UCTGeometry::getUCTTowerIndex(UCTRegionIndex region, uint32_t iEta, uint32_t iPhi) {
  if(iPhi >= NPhiInRegion || iEta >= NEtaInRegion) {
    return UCTTowerIndex(0, 0); // Illegal values
  }
  int regionEta = region.first;
  int absRegionEta = abs(regionEta);
  int towerEta = (regionEta / absRegionEta) * (absRegionEta * NEtaInRegion + iEta);
  uint32_t regionPhi = region.second;
  int towerPhi = regionPhi * NPhiInRegion + iPhi;
  return UCTTowerIndex(towerEta, towerPhi);
}

double UCTGeometry::getUCTTowerEta(int caloEta) {
  static bool first = true;
  static double twrEtaValues[29];
  if(first) {
    twrEtaValues[0] = 0;
    for(unsigned int i = 0; i < 20; i++) {
      twrEtaValues[i + 1] = 0.0436 + i * 0.0872;
    }
    twrEtaValues[21] = 1.785;
    twrEtaValues[22] = 1.880;
    twrEtaValues[23] = 1.9865;
    twrEtaValues[24] = 2.1075;
    twrEtaValues[25] = 2.247;
    twrEtaValues[26] = 2.411;
    twrEtaValues[27] = 2.575;
    twrEtaValues[28] = 2.825;
    first = false;
  }
  uint32_t absCaloEta = abs(caloEta);
  if(absCaloEta <= 28) return twrEtaValues[absCaloEta];
  else return -999.;
}

double UCTGeometry::getUCTTowerPhi(int caloPhi, int caloEta) {
  if(caloPhi < 0) return -999.;
  uint32_t absCaloEta = abs(caloEta) - 1;
  uint32_t absCaloPhi = abs(caloPhi) - 1;
  if(absCaloEta < 28 && absCaloPhi < 72) return (((double) absCaloPhi + 0.5) * 0.0872);
  else if(absCaloEta < 41 && absCaloPhi < 36) return (((double) absCaloPhi + 0.5) * 0.0872 * 2);
  else if(absCaloEta < 41 && absCaloPhi < 18) return (((double) absCaloPhi + 0.5) * 0.0872 * 4);
  else return -999.;
}
