import FWCore.ParameterSet.Config as cms

process = cms.Process("Layer1EmulatorOnSpy")

process.load("FWCore.MessageService.MessageLogger_cfi")
process.load('L1Trigger.L1TCaloLayer1.layer1EmulatorDigis_cfi')
process.layer1EmulatorDigis.ecalTPSource = cms.InputTag("l1tCaloLayer1SpyDigis")
process.layer1EmulatorDigis.hcalTPSource = cms.InputTag("l1tCaloLayer1SpyDigis")
process.layer1EmulatorDigis.verbose = cms.bool(True)

process.load('L1Trigger.L1TCaloLayer1.layer1Validator_cfi')
process.layer1Validator.testSource = cms.InputTag("l1tCaloLayer1SpyDigis")
process.layer1Validator.emulSource = cms.InputTag("layer1EmulatorDigis")
process.layer1Validator.verbose = cms.bool(True)

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(1000) )

process.source = cms.Source("PoolSource",
    fileNames = cms.untracked.vstring('file:/afs/cern.ch/user/d/dasu/CMSSW_7_4_12_patch4/src/L1Trigger/L1TCaloLayer1Spy/test/l1tCaloLayer1Spy.root')
)

process.out = cms.OutputModule("PoolOutputModule",
    fileName = cms.untracked.string('l1tCaloLayer1Spy+Emulator.root'),
    outputCommands = cms.untracked.vstring('keep *')
)

process.p = cms.Path(process.layer1EmulatorDigis+process.layer1Validator)

process.e = cms.EndPath(process.out)
