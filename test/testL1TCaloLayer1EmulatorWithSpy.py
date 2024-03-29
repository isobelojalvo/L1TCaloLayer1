import FWCore.ParameterSet.Config as cms

from FWCore.ParameterSet.VarParsing import VarParsing

options = VarParsing()
options.register('setupString', "captures:/data/dasu/Layer1ZeroBiasCaptureData/r260490_1", VarParsing.multiplicity.singleton, VarParsing.varType.string, 'L1TCaloLayer1Spy setupString')
options.register('maxEvents', 162, VarParsing.multiplicity.singleton, VarParsing.varType.int, 'Maximum number of evnets')
options.parseArguments()

process = cms.Process("Layer1EmulatorWithSpy")

process.load("FWCore.MessageService.MessageLogger_cfi")

process.load('L1Trigger.L1TCaloLayer1Spy.l1tCaloLayer1SpyDigis_cfi')
process.l1tCaloLayer1SpyDigis.setupString = cms.untracked.string(options.setupString)

process.load('L1Trigger.L1TCaloLayer1.layer1EmulatorDigis_cfi')
process.layer1EmulatorDigis.ecalTPSource = cms.InputTag("l1tCaloLayer1SpyDigis")
process.layer1EmulatorDigis.hcalTPSource = cms.InputTag("l1tCaloLayer1SpyDigis")
process.layer1EmulatorDigis.verbose = cms.bool(True)

process.load('L1Trigger.L1TCaloLayer1.layer1Validator_cfi')
process.layer1Validator.testSource = cms.InputTag("l1tCaloLayer1SpyDigis")
process.layer1Validator.emulSource = cms.InputTag("layer1EmulatorDigis")
process.layer1Validator.verbose = cms.bool(True)

# Put multiples of 162 - output data for eighteen BXs are available for each capture
# One event is created for each capture.  Putting non-multiples of 162 just means
# that some of the events captured are "wasted".

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(options.maxEvents) )

process.source = cms.Source("EmptySource")

process.out = cms.OutputModule("PoolOutputModule",
    fileName = cms.untracked.string('/data/dasu/l1tCaloLayer1Spy+Emulator.root'),
    outputCommands = cms.untracked.vstring('keep *')
)

process.p = cms.Path(process.l1tCaloLayer1SpyDigis*process.layer1EmulatorDigis*process.layer1Validator)

process.e = cms.EndPath(process.out)
