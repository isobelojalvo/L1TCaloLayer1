#ifndef UCTGeometry_hh
#define UCTGeometry_hh

// UCT Geometric mapping between EB+EE/HB+HE & HF TPGs to CPT7s
// Access methods check if the mapping is correct
// Access methods also allow conversion of TPG to CTP7 mapping
// This class only has functions and defintions, no local memory

/*
    
  Crate Eta mapping:

  eta:      -5 ... -4 ... -3 ... -2 ... -1 ... 0 ... 1 ... 2 ...  3 ...  4 ...  5
  absIEta:  41   ...  30(29)28     ...        1 1     ...       28(29)30   ... 41
  region:   12 11 10  9  8  7  6  5  4 3 2 1  0 0 1 2  3  4   5  6 7 8 9 10 11 12

  For EB/EE towers we have 8-bit ET + 1-bit finegrain input
  For HB/HE towers we have 8-bit ET + 5-bit feature (TBD)
  For HF only 8-bits ET per input tower

  Each EB/HB+EE/HE regions are 4x4 == ~0.35 x 0.35 iEta x iPhi wide

  Calo TPG eta index is 1 through 28 for EB/HB+EE/HE
  Note that tower 29 is unused in trigger as it shadows portion of HE
  Corresponding negative values are for negative eta

  The uHTR HF eta index is 30-41 for 12 divisions in eta between 3-5
  In the HF region size is proposed as 2x2 HF TPGs in etaxphi == 0.33 x 0.35
  It will be in 2x1 for 40 and 41 as phi is already doubled there at TPG level

*/

#include <utility>

#define NCrates 3
#define NCardsInCrate 6
#define NRegionsInCard 7
#define NEtaInRegion 4
#define NPhiInRegion 4
#define NPhiInCard NPhiInRegion

#define HFEtaOffset NRegionsInCard * NEtaInRegion + 1
#define NHFRegionsInCard 6
#define NHFEtaInRegion 2
#define NHFPhiInRegion 2
#define NHFPhiInCard NHFPhiInRegion
#define NVHFPhiInCard 1

#define NSides 2  // Positive and Negative Eta sides
#define NEta NRegionsInCard * NEtaInRegion
#define NPhi NCrates * NCardsInCrate * NPhiInRegion
#define NHFEta NHFRegionsInCard * NHFEtaInRegion
#define NHFPhi NCrates * NCardsInCrate * NHFPhiInRegion

#define MaxCrateNumber (NCrates - 1)
#define MaxCardNumber (NCardsInCrate - 1)
#define MaxRegionNumber (NRegionsInCard - 1)
#define MaxEtaInRegion (NEtaInRegion - 1)
#define MaxPhiInRegion (NPhiInRegion - 1)

#define MaxCaloEta 41
#define MaxCaloPhi 72
#define CaloHFRegionStart 7
#define CaloVHFRegionStart 12
#define MaxCaloPhiInHF MaxCaloPhi/2
#define MaxCaloPhiInVHF MaxCaloPhi/4

#define MaxUCTRegionsPhi MaxCaloPhi / NPhiInRegion
#define MaxUCTRegionsEta 2 * (NRegionsInCard + NHFRegionsInCard)

typedef std::pair<int, uint32_t> UCTRegionIndex;
typedef std::pair<int, int> UCTTowerIndex;

class UCTGeometry {

public:

  UCTGeometry() {;}
  ~UCTGeometry() {;}

  // Calorimeter indices are defined to be ints and do not count from zero
  // Eta index sign indicates negative or positive eta
  // The zero eta value is illegal
  // Phi indices go 1 through 72 and other values are illegal

  int getCaloEtaIndex(bool negativeSide, uint32_t region, uint32_t iEta);
  int getCaloPhiIndex(uint32_t crate, uint32_t card, uint32_t region, uint32_t iPhi);

  uint32_t getLinkNumber(bool negativeSide, uint32_t region, uint32_t iEta, uint32_t iPhi);
  uint32_t getChannelNumber(bool negativeSide, uint32_t iEta, uint32_t iPhi);

  uint32_t getNCrates() {return NCrates;}
  uint32_t getNCards() {return NCardsInCrate;}
  uint32_t getNRegions() {return (NRegionsInCard+NHFRegionsInCard);}
  uint32_t getNEta(uint32_t region);
  uint32_t getNPhi(uint32_t region);

  uint32_t getCrate(int caloEta, int caloPhi);
  uint32_t getCard(int caloEta,int caloPhi);
  uint32_t getRegion(int caloEta,int caloPhi);
  uint32_t getiEta(int caloEta,int caloPhi);
  uint32_t getiPhi(int caloEta,int caloPhi);

  bool checkCrate(uint32_t crate) {return !(crate < NCrates);}
  bool checkCard(uint32_t card) {return !(card < NCardsInCrate);}
  bool checkRegion(uint32_t region) {return !(region < (NRegionsInCard + NHFRegionsInCard));}
  bool checkEtaIndex(uint32_t region, uint32_t iEta) {
    if(region < NRegionsInCard)
      return !(iEta < NEtaInRegion);
    else
      return !(iEta < NHFEtaInRegion);
  }
  bool checkPhiIndex(uint32_t region, uint32_t iPhi) {
    if(region < NRegionsInCard)
      return !(iPhi < NPhiInRegion);
    else
      return !(iPhi < NHFPhiInRegion);
  }

  // For summary card, we label regions by phi and eta indices
  //  UCTRegionPhiIndices are 0-17
  //  UCTRegionEtaIndices are 1-7 for EB/HB+EE/HE and 8-13 for HF, 
  //   with negative values for negative eta, and zero being illegal
  // We label by the pair (UCTRegionPhiIndex, UCTRegionEtaIndex)
  // We can then walk the eta-phi plane by looking at nearest neighbors
  // using the access functions below.  We should never need beyond 
  // nearest-neighbor for all but global objects like TotalET, HT, MET, MHT
  // In those cases, we loop over all regions.

  uint32_t getUCTRegionPhiIndex(int caloPhi) {
    if(caloPhi < 71) return ((caloPhi + 1) / 4);
    else return 17;
  }
  uint32_t getUCTRegionEtaIndex(int caloEta) {
    // Region index is same for all phi; so get for phi = 1
    uint32_t rgn = getRegion(caloEta, 1);
    if(caloEta < 0) return -(rgn+1);
    return (rgn+1);
  }

  uint32_t getUCTRegionPhiIndex(uint32_t crate, uint32_t card);

  int getUCTRegionEtaIndex(bool negativeSide, uint32_t region) {
    if(!checkRegion(region)) return 0xDEADBEEF;
    if(negativeSide) return -(region + 1);
    else return (region + 1);
  }

  UCTRegionIndex getUCTRegionIndex(UCTTowerIndex caloTower) {
    return getUCTRegionIndex(caloTower.first, caloTower.second);
  }

  UCTRegionIndex getUCTRegionIndex(int caloEta, int caloPhi);

  UCTRegionIndex getUCTRegionIndex(bool negativeSide, uint32_t crate, uint32_t card, uint32_t region);

  UCTTowerIndex getUCTTowerIndex(UCTRegionIndex r, uint32_t iEta = 0, uint32_t iPhi = 0);

  double getUCTTowerEta(int caloEta);
  double getUCTTowerPhi(int caloPhi, int caloEta);

};

#endif
