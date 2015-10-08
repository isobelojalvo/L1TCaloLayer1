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
  if(crate == 0) {
    caloPhiIndex = 13 + card * 4 + iPhi;
  }
  else if(crate == 1) {
    if(card < 3) {
      caloPhiIndex = 61 + card * 4 + iPhi;
    }
    else {
      caloPhiIndex = 1 + (card - 3) * 4 + iPhi;
    }
  }
  else if(crate == 2) {
    caloPhiIndex = 37 + card * 4 + iPhi;
  }
  if(region >= CaloVHFRegionStart) {
    caloPhiIndex = 1 + caloPhiIndex / 4;
  }
  else if(region >= CaloHFRegionStart) {
    caloPhiIndex = 1 + caloPhiIndex / 2;
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
      uctRegionPhiIndex = card;
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
  if(cPhi >= 13 && cPhi <= 36) crate = 0;
  else if(cPhi >= 37 && cPhi <= 60) crate = 2;
  else if(cPhi >= 61 && cPhi <= 72) crate = 1;
  else if(cPhi >= 1 && cPhi <= 12) crate = 1;  
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
    card = (cPhi - 13) / 4;
  }
  else if(crate == 2) {
    card = (cPhi - 37) / 4;
  }
  else if(crate == 1 && cPhi > 60) {
    card = (cPhi - 61) / 4;
  }
  else if(crate == 1 && cPhi <= 12) {
    card = (cPhi + 11) / 4;
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
  uint32_t card = getCard(caloEta, caloPhi);
  uint32_t iPhi = 0xDEADBEEF;
  if(region < CaloHFRegionStart && caloPhi <= MaxCaloPhi) {
    iPhi = (caloPhi - 1 - card * NPhiInCard) % NPhiInCard;
  }
  else if(region < CaloVHFRegionStart && caloPhi <= MaxCaloPhiInHF) {
    iPhi = (caloPhi - 1 - card * NHFPhiInCard) % NHFPhiInCard;
  }
  else {
    iPhi = (caloPhi - 1 - card * NVHFPhiInCard) % NVHFPhiInCard;
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
  int towerEta = (regionEta / absRegionEta) * (absRegionEta * NEtaInRegion + iEta + 1);
  uint32_t regionPhi = region.second;
  int towerPhi = regionPhi * NPhiInRegion + iPhi + 1;
  return UCTTowerIndex(towerEta, towerPhi);
}

UCTRegionIndex UCTGeometry::getUCTRegionNorth(UCTRegionIndex center) {
  int eta = center.first;
  uint32_t phi = center.second;
  phi += 1;
  if(phi > MaxUCTRegionsPhi) phi -= MaxUCTRegionsPhi;
  return UCTRegionIndex(eta, phi);
}

UCTRegionIndex UCTGeometry::getUCTRegionSouth(UCTRegionIndex center) {
  int eta = center.first;
  uint32_t phi = center.second;
  phi -= 1;
  if(phi == 0) phi = MaxUCTRegionsPhi;
  return UCTRegionIndex(eta, phi);
}

UCTRegionIndex UCTGeometry::getUCTRegionEast(UCTRegionIndex center) {
  int eta = center.first;
  uint32_t phi = center.second;
  eta += 1;
  if(eta == 0) eta = 1; // eta = 0 is illegal, go one above
  if(eta > MaxUCTRegionsEta) eta = 0; // beyond high Eta edge - should not be used
  return UCTRegionIndex(eta, phi);
}

UCTRegionIndex UCTGeometry::getUCTRegionWest(UCTRegionIndex center) {
  int eta = center.first;
  uint32_t phi = center.second;
  eta -= 1;
  if(eta == 0) eta = -1; // eta = 0 is illegal, go one below
  if(eta < -MaxUCTRegionsEta) eta = 0; // beyond high Eta edge - should not be used
  return UCTRegionIndex(eta, phi);
}

UCTRegionIndex UCTGeometry::getUCTRegionNE(UCTRegionIndex center) {
  int eta = center.first;
  uint32_t phi = center.second;
  phi += 1;
  if(phi > MaxUCTRegionsPhi) phi -= MaxUCTRegionsPhi;
  eta += 1;
  if(eta == 0) eta = 1; // eta = 0 is illegal, go one above
  if(eta > MaxUCTRegionsEta) eta = 0; // beyond high Eta edge - should not be used
  return UCTRegionIndex(eta, phi);
}

UCTRegionIndex UCTGeometry::getUCTRegionNW(UCTRegionIndex center) {
  int eta = center.first;
  uint32_t phi = center.second;
  phi += 1;
  if(phi > MaxUCTRegionsPhi) phi -= MaxUCTRegionsPhi;
  eta -= 1;
  if(eta == 0) eta = -1; // eta = 0 is illegal, go one below
  if(eta < -MaxUCTRegionsEta) eta = 0; // beyond high Eta edge - should not be used
  return UCTRegionIndex(eta, phi);
}

UCTRegionIndex UCTGeometry::getUCTRegionSE(UCTRegionIndex center) {
  int eta = center.first;
  uint32_t phi = center.second;
  phi -= 1;
  if(phi == 0) phi = MaxUCTRegionsPhi;
  eta += 1;
  if(eta == 0) eta = 1; // eta = 0 is illegal, go one above
  if(eta > MaxUCTRegionsEta) eta = 0; // beyond high Eta edge - should not be used
  return UCTRegionIndex(eta, phi);
}

UCTRegionIndex UCTGeometry::getUCTRegionSW(UCTRegionIndex center) {
  int eta = center.first;
  uint32_t phi = center.second;
  phi -= 1;
  if(phi == 0) phi = MaxUCTRegionsPhi;
  eta -= 1;
  if(eta == 0) eta = -1; // eta = 0 is illegal, go one below
  if(eta < -MaxUCTRegionsEta) eta = 0; // beyond high Eta edge - should not be used
  return UCTRegionIndex(eta, phi);
}

