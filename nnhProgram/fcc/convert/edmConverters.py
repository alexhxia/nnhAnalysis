"""
Event Data Model converters.
Read or write LCIO or EDM4hep events.

https://github.com/key4hep/k4MarlinWrapper/blob/master/doc/edmConverters.md
"""

from Configurables import k4DataSvc, PodioInput     # readEDM4hepEvent
from Configurables import EventDataSvc, LcioEvent   # readLCIOEvent
from Configurables import PodioOutput               # writeEDM4hepEvent
from Configurables import MarlinProcessorWrapper    # writeLCIOEvent
from Configurables import ToolSvc, EDM4hep2LcioTool # converterEDM4hepToLCIO
from Configurables import ToolSvc, Lcio2EDM4hepTool # converterLCIOToEDM4hep


def readEDM4hepEvent(rootFile):
    """Read EDM4hep events"""

    # Instantiate the Event Service from k4FWCore: k4DataSvc
    evtsvc = k4DataSvc('EventDataSvc')
    
    # Indicate the event file to read.
    evtsvc.input = rootFile #'path/to/file.root' 

    # Then use the algorithm PodioInput to select the collections to read by their name.
    inp = PodioInput('InputReader')
    inp.collections = [
        'ReconstructedParticles',
        'EFlowTrack'
    ]
    inp.OutputLevel = DEBUG

    # Add the algorithm PodioInput to the algorithm list.
    algList.append(inp)


def readLCIOEvent(slcioFile): 
    """Read LCIO events"""
    
    # Instantiate the Event Data Service.
    evtsvc = EventDataSvc()

    # Use LcioEvent and indicate the event file to read.
    read = LcioEvent()
    read.OutputLevel = DEBUG
    read.Files = [slcioFile] # "path/to/file.slcio"

    # Add the algorithm LcioEvent to the algorithm list.
    algList.append(read)
    print(algList)
    return algList
    
    
def writeEDM4hepEvent(outputRootFile):
    """Write EDM4hep events"""
    
    # Instantiate PodioOutput and indicate event file to write.
    out = PodioOutput("PodioOutput", filename = outputRootFile) # "my_output.root")
    
    # Select command for PodioOutput
    out.outputCommands = ["keep *"]

    # Add the PodioOutput to the algorithm list.
    algList.append(out)


def writeLCIOEvent(outputLCIOFile):
    """Write LCIO events"""
    
    # Instantiate a MarlinProcessorWrapper with a relevant name.
    Output_DST = MarlinProcessorWrapper("Output_DST")
    Output_DST.OutputLevel = WARNING
    
    # Indicate the ProcessorType to be LCIOOutputProcessor.
    Output_DST.ProcessorType = "LCIOOutputProcessor"
    
    # Indicate the relevant parameters for the Marlin Processor.
    Output_DST.Parameters = {
        "DropCollectionNames": [],
        "DropCollectionTypes": ["MCParticle", "LCRelation", "SimCalorimeterHit"],
    }

    # Add the MarlinProcessorWrapper to the algorithm list.        
    algList.append(Output_DST)  


def converterEDM4hepToLCIO():
    """
    EDM4hep to LCIO converter
    Collections from events that are already read, or are produced 
    by a Gaudi Algorithm can be converted from EDM4hep to LCIO format:
    """
    
    # Instantiate the EDM4hep2LcioTool Gaudi Tool.
    edmConvTool = EDM4hep2LcioTool("EDM4hep2lcio")
    
    # Indicate the collections to convert in Parameters.
    # * To convert all available collections write an asterisk, like follows: 
    #   edmConvTool.Parameters = ["*"]
    # * Arguments are read in groups of 2: name of the EDM4hep collection, 
    #   name of the LCIO converted collection.
    edmConvTool.Parameters = [
        "EFlowTrack", 
        "EFlowTrack_LCIO",
        "ReconstructedParticles", 
        "ReconstructedParticle_LCIO"
    ]
    edmConvTool.OutputLevel = DEBUG

    # Select the Gaudi Algorithm that will convert the indicated collections.
    InitDD4hep = MarlinProcessorWrapper("InitDD4hep")

    # Add the Tool to the Gaudi Algorithm.
    InitDD4hep.EDM4hep2LcioTool=edmConvTool
    
    
def converterLCIOToEDM4hep():
    """
    LCIO to EDM4hep converter
    Collections from events that are already read, or are produced 
    by a gaudi Algorithm can be converted from LCIO to EDM4hep format:
    """
    
    # Instantiate the Lcio2EDM4hepTool Gaudi Tool.
    lcioConvTool = Lcio2EDM4hepTool("LCIO2EDM4hep")
    
    # Indicate the collections to convert in Parameters.
    # * To convert all available collections write an asterisk, like follows: 
    #   lcioConvTool.Parameters = ["*"]
    # * Arguments are read in groups of 2: name of the LCIO collection, 
    #   name of the EDM4hep converted collection.
    lcioConvTool.Parameters = [
        "EFlowTrackConv", 
        "EFlowTrackEDM4hep",
        "ReconstructedParticle", 
        "ReconstructedParticlesEDM4hep",
        "BuildUpVertices", 
        "BuildUpVerticesEDM4hep",
        "PrimaryVertices", 
        "PrimaryVerticesEDM4hep"
    ]
    lcioConvTool.OutputLevel = DEBUG

    # Select the Gaudi Algorithm that will convert the indicated collections.
    JetClusteringAndRefiner = MarlinProcessorWrapper("JetClusteringAndRefiner")

    # Add the Tool to the Gaudi Algorithm.
    JetClusteringAndRefiner.Lcio2EDM4hepTool = lcioConvTool
    

if __name__ == "__main__":
    
    readLCIOEvent("muons.slcio")
    #converterLCIOToEDM4hep()
    print("marche")
    
