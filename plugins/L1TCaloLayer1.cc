// -*- C++ -*-
//
// Package:    L1Trigger/L1TCaloLayer1
// Class:      L1TCaloLayer1
// 
/**\class L1TCaloLayer1 L1TCaloLayer1.cc L1Trigger/L1TCaloLayer1/plugins/L1TCaloLayer1.cc

   Description: [one line class summary]

   Implementation:
   [Notes on implementation]
*/
//
// Original Author:  Sridhara Rao Dasu
//         Created:  Thu, 08 Oct 2015 09:20:16 GMT
//
//


// system include files
#include <memory>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDProducer.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "DataFormats/EcalDigi/interface/EcalDigiCollections.h"
#include "DataFormats/HcalDigi/interface/HcalDigiCollections.h"

#include "DataFormats/L1TCalorimeter/interface/CaloTower.h"

using namespace l1t;

//
// class declaration
//

class L1TCaloLayer1 : public edm::EDProducer {
public:
  explicit L1TCaloLayer1(const edm::ParameterSet&);
  ~L1TCaloLayer1();

  static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);

private:
  virtual void beginJob() override;
  virtual void produce(edm::Event&, const edm::EventSetup&) override;
  virtual void endJob() override;
      
  //virtual void beginRun(edm::Run const&, edm::EventSetup const&) override;
  //virtual void endRun(edm::Run const&, edm::EventSetup const&) override;
  //virtual void beginLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&) override;
  //virtual void endLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&) override;

  // ----------member data ---------------------------

  edm::EDGetTokenT<EcalTrigPrimDigiCollection> ecalTPSource;
  std::string ecalTPSourceLabel;
  edm::EDGetTokenT<HcalTrigPrimDigiCollection> hcalTPSource;
  std::string hcalTPSourceLabel;

  bool verbose;

};

//
// constants, enums and typedefs
//


//
// static data member definitions
//

//
// constructors and destructor
//
L1TCaloLayer1::L1TCaloLayer1(const edm::ParameterSet& iConfig) :
  ecalTPSource(consumes<EcalTrigPrimDigiCollection>(iConfig.getParameter<edm::InputTag>("ecalTPSource"))),
  ecalTPSourceLabel(iConfig.getParameter<edm::InputTag>("ecalTPSource").label()),
  hcalTPSource(consumes<HcalTrigPrimDigiCollection>(iConfig.getParameter<edm::InputTag>("hcalTPSource"))),
  hcalTPSourceLabel(iConfig.getParameter<edm::InputTag>("hcalTPSource").label()),
  verbose(iConfig.getParameter<bool>("verbose"))
{
  produces<CaloTowerBxCollection>();
}

L1TCaloLayer1::~L1TCaloLayer1() {}

//
// member functions
//

// ------------ method called to produce the data  ------------
void
L1TCaloLayer1::produce(edm::Event& iEvent, const edm::EventSetup& iSetup)
{
  using namespace edm;

  edm::Handle<EcalTrigPrimDigiCollection> ecalTPs;
  iEvent.getByToken(ecalTPSource, ecalTPs);
  edm::Handle<HcalTrigPrimDigiCollection> hcalTPs;
  iEvent.getByToken(hcalTPSource, hcalTPs);

  for ( const auto& ecalTp : *ecalTPs ) {

    if ((ecalTp.sample(0).raw()) >= 4096) {
      std::cout<<"raw : "<<ecalTp.sample(0).raw()<<" and its condition check :"<< ((ecalTp.sample(0).raw()>>13) & 0x7)<<std::endl;
    }
    
    if(((ecalTp.sample(0).raw()>>13) & 0x7)==0)
      {
	std::cout << ecalTp.compressedEt() << std::endl;
      }

  }

}

// ------------ method called once each job just before starting event loop  ------------
void 
L1TCaloLayer1::beginJob()
{
}

// ------------ method called once each job just after ending the event loop  ------------
void 
L1TCaloLayer1::endJob() {
}

// ------------ method called when starting to processes a run  ------------
/*
  void
  L1TCaloLayer1::beginRun(edm::Run const&, edm::EventSetup const&)
  {
  }
*/
 
// ------------ method called when ending the processing of a run  ------------
/*
  void
  L1TCaloLayer1::endRun(edm::Run const&, edm::EventSetup const&)
  {
  }
*/
 
// ------------ method called when starting to processes a luminosity block  ------------
/*
  void
  L1TCaloLayer1::beginLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&)
  {
  }
*/
 
// ------------ method called when ending the processing of a luminosity block  ------------
/*
  void
  L1TCaloLayer1::endLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&)
  {
  }
*/
 
// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
void
L1TCaloLayer1::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  //The following says we do not know what parameters are allowed so do no validation
  // Please change this to state exactly what you do use, even if it is no parameters
  edm::ParameterSetDescription desc;
  desc.setUnknown();
  descriptions.addDefault(desc);
}

//define this as a plug-in
DEFINE_FWK_MODULE(L1TCaloLayer1);
