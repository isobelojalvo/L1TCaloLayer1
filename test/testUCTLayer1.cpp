#include <iostream>
#include <iomanip>
#include <vector>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>

using namespace std;

#include "UCTLayer1.hh"
#include "UCTCrate.hh"
#include "UCTCard.hh"
#include "UCTRegion.hh"
#include "UCTTower.hh"

double flatRandom(double min, double max) {
  static double rMax = (double) 0x7FFFFFFF;
  uint32_t r = random();
  double d = (double) r;
  double f = min + ((max - min) * d / rMax);
  if(f < min) f = min;
  if(f > max) f = max;
  return f;
}

double gaussianRandom(double mean, double standardDeviation) {
  return 0;
}

double poissonRandom(double mean) {
  static double oldMean = -1;
  static double g;
  if(mean != oldMean) {
    oldMean = mean;
    if(mean == 0) {
      g = 0;
    }
    else {
      g = exp(-mean);
    }
  }    
  double em = -1;
  double t = 1;
  do {
    em++;
    t *= flatRandom(0., 1.);
  } while(t > g);
  return em;
}

void print(UCTLayer1& uct) {
  vector<UCTCrate*> crates = uct.getCrates();
  for(uint32_t crt = 0; crt < crates.size(); crt++) {
    vector<UCTCard*> cards = crates[crt]->getCards();
    for(uint32_t crd = 0; crd < cards.size(); crd++) {
      vector<UCTRegion*> regions = cards[crd]->getRegions();
      for(uint32_t rgn = 0; rgn < regions.size(); rgn++) {
	if(regions[rgn]->et() > 0) {
	  int hitEta = regions[rgn]->hitCaloEta();
	  uint32_t hitPhi = regions[rgn]->hitCaloPhi();
	  vector<UCTTower*> towers = regions[rgn]->getTowers();
	  bool header = true;
	  for(uint32_t twr = 0; twr < towers.size(); twr++) {
	    if(towers[twr]->caloPhi() == hitPhi && towers[twr]->caloEta() == hitEta) {
	      std::cout << "*";
	    }
	    towers[twr]->print(header);
	    if(header) header = false;
	  }
	  regions[rgn]->print();
	}
      }
      cards[crd]->print();
    }
    crates[crt]->print();
  }
  uct.print();
}

int main(int argc, char** argv) {

  UCTLayer1 uctLayer1;

  // Event loop
  while (true) {

    if(!uctLayer1.clearEvent()) {
      std::cerr << "UCT: Failed to clear event" << std::endl;
      exit(1);
    }
    
    // Put a random number of towers in the UCT 

    uint32_t expectedTotalET = 0;
    
    // ECAL TPGs - set a mean of 100 random ECAL towers!
    uint32_t nHitTowers = poissonRandom(100.);
    for(uint32_t i = 0; i < nHitTowers; i++) {
      uint32_t et = (random() & 0xFF); // Random energy up to the maximum allowed
      bool fg = ((random() % 100) < 95); // 5% of the time eleFG Veto should "kill" electron
      int caloEta = ((random()+1) % 28); // Distribute uniformly in +/- eta within acceptance
      while(caloEta < 1 || caloEta > 28) caloEta = ((random()+1) % 28);
      if((random() & 0x1) != 0) caloEta = -caloEta;
      int caloPhi = ((random()+1) % 72); // Distribute uniformly in all phi
      while(caloPhi < 1 || caloPhi > 72) caloPhi = ((random()+1) % 72);
      UCTTowerIndex t = UCTTowerIndex(caloEta, caloPhi);
      if(!uctLayer1.setECALData(t, fg, et)) {
	std::cerr << "UCT: Failed loading an ECAL tower" << std::endl;
	exit(1);
      }
      expectedTotalET += et;
    }

    // HCAL TPGs - set a mean of 100 random HCAL towers!
    nHitTowers = poissonRandom(100.);
    for(uint32_t i = 0; i < nHitTowers; i++) {
      uint32_t et = (random() & 0xFF); // Random energy up to the maximum allowed
      uint32_t fb = (random() & 0x1F); // Set random five bits - this is true emulation!
      int caloEta = ((random()+1) % 28); // Distribute uniformly in +/- eta within acceptance
      while(caloEta < 1 || caloEta > 28) caloEta = ((random()+1) % 28);
      if((random() & 0x1) != 0) caloEta = -caloEta;
      int caloPhi = ((random()+1) % 72); // Distribute uniformly in all phi
      while(caloPhi < 1 || caloPhi > 72) caloPhi = ((random()+1) % 72);
      UCTTowerIndex t = UCTTowerIndex(caloEta, caloPhi);
      if(!uctLayer1.setHCALData(t, et, fb)) {
	std::cerr << "UCT: Failed loading an HCAL tower" << std::endl;
	exit(1);
      }
      expectedTotalET += et;
    }
      
    // Process
    if(!uctLayer1.process()) {
      std::cerr << "UCT: Failed to process layer 1" << std::endl;
      exit(1);
    }

    // Crude check if total ET is approximately OK!
    // We can't expect exact match as there is region level saturation to 10-bits
    // 1% is good enough
    if((uctLayer1.et() - expectedTotalET) < - (0.01 * expectedTotalET) ) {
      print(uctLayer1);
      std::cout << "Expected " 
		<< std::showbase << std::internal << std::setfill('0') << std::setw(10) << std::hex
		<< expectedTotalET << std::endl;
    }

  }

  return 0;

}