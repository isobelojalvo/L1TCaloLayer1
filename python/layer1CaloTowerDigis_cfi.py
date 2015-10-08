import FWCore.ParameterSet.Config as cms

layer1CaloTowerDigis = cms.EDProducer('L1TCaloLayer1',
                                      ecalTPSource = cms.InputTag("l1tCaloLayer1Digis"),
                                      hcalTPSource = cms.InputTag("l1tCaloLayer1Digis"),
                                      verbose = cms.bool(False)
                                      )
