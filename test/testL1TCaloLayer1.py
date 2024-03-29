import FWCore.ParameterSet.Config as cms

process = cms.Process("L1TCaloLayer1Test")

import EventFilter.L1TCaloLayer1RawToDigi.util as util

from FWCore.ParameterSet.VarParsing import VarParsing

options = VarParsing()
options.register('runNumber', 0, VarParsing.multiplicity.singleton, VarParsing.varType.int, 'Run to analyze')
options.register('lumis', '1-max', VarParsing.multiplicity.singleton, VarParsing.varType.string, 'Lumis')
options.register('dataStream', '/ExpressPhysics/Run2015D-Express-v3/FEVT', VarParsing.multiplicity.singleton, VarParsing.varType.string, 'Dataset to look for run in')
options.register('inputFiles', [], VarParsing.multiplicity.list, VarParsing.varType.string, 'Manual file list input, will query DAS if empty')
options.register('inputFileList', '', VarParsing.multiplicity.singleton, VarParsing.varType.string, 'Manual file list input, will query DAS if empty')
options.register('useORCON', False, VarParsing.multiplicity.singleton, VarParsing.varType.bool, 'Use ORCON for conditions.  This is necessary for very recent runs where conditions have not propogated to Frontier')
options.parseArguments()

def formatLumis(lumistring, run) :
    lumis = (lrange.split('-') for lrange in lumistring.split(','))
    runlumis = (['%d:%s' % (run,lumi) for lumi in lrange] for lrange in lumis)
    return ['-'.join(l) for l in runlumis]

print 'Getting files for run %d...' % options.runNumber
if len(options.inputFiles) is 0 and options.inputFileList is '' :
    inputFiles = util.getFilesForRun(options.runNumber, options.dataStream)
elif len(options.inputFileList) > 0 :
    with open(options.inputFileList) as f :
        inputFiles = list((line.strip() for line in f))
else :
    inputFiles = cms.untracked.vstring(options.inputFiles)
if len(inputFiles) is 0 :
    raise Exception('No files found for dataset %s run %d' % (options.dataStream, options.runNumber))
print 'Ok, time to analyze'

process.load("FWCore.MessageService.MessageLogger_cfi")
process.load('EventFilter.L1TCaloLayer1RawToDigi.l1tCaloLayer1Digis_cfi')
process.load('L1Trigger.L1TCaloLayer1.layer1EmulatorDigis_cfi')
process.layer1EmulatorDigis.verbose = cms.bool(True)

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(1000) )

process.source = cms.Source("PoolSource",
    fileNames = cms.untracked.vstring(inputFiles)
#   fileNames = cms.untracked.vstring('file:/data/dasu/Sept25_905539F2-5363-E511-831A-02163E013687.root')
)

process.out = cms.OutputModule("PoolOutputModule",
    fileName = cms.untracked.string('/data/dasu/l1tCaloLayer1.root'),
    outputCommands = cms.untracked.vstring('keep *')
)

process.load('Configuration/StandardSequences/FrontierConditions_GlobalTag_condDBv2_cff')
process.GlobalTag.globaltag = '74X_dataRun2_Express_v1'

process.p = cms.Path(process.l1tCaloLayer1Digis*process.layer1EmulatorDigis)

process.e = cms.EndPath(process.out)
