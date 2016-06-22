"""
      TestExport.py
      Unit test of ExchangePlugin_TestExport class
"""
#=========================================================================
# Initialization of the test
#=========================================================================
import os
import math

from ModelAPI import *
from GeomDataAPI import *
from GeomAlgoAPI import *
from GeomAPI import *

from TestImport import testImport

__updated__ = "2015-05-22"

aSession = ModelAPI_Session.get()
#=========================================================================
# Help functions
#=========================================================================
def removeFile(theFileName):
    try: os.remove(theFileName)
    except OSError: pass
    assert not os.path.exists(theFileName), \
            "Can not remove file {0}".format(theFileName)

#=========================================================================
# Common test function
#=========================================================================
def testExport(theType, theFormat, theFile, theVolume, theDelta):
    # Import a reference part
    aSession.startOperation("Add part")
    aPartFeature = aSession.moduleDocument().addFeature("Part")
    aSession.finishOperation()
    aPart = aSession.activeDocument()

    aSession.startOperation("Import screw")
    anImportFeature = aPart.addFeature("Import")
    anImportFeature.string("file_path").setValue("Data/screw.step")
    anImportFeature.execute()
    aSession.finishOperation()

    removeFile(theFile)
    # Export a part
    aSession.startOperation("Export part")
    aFeatureKind = "Export"
    anExportFeature = aPart.addFeature(aFeatureKind)
    assert anExportFeature, "{0}: Can not create a feature {1}".format(theType, aFeatureKind)

    aFormatAttrName = "file_format"
    aFormatAttr = anExportFeature.string(aFormatAttrName)
    assert aFormatAttr, "{0}: Can not receive string field {1}".format(theType, aFormatAttrName)
    aFormatAttr.setValue(theFormat)

    aFileAttrName = "file_path"
    aFileAttr = anExportFeature.string(aFileAttrName)
    assert aFileAttr, "{0}: Can not receive string field {1}".format(theType, aFileAttrName)
    aFileAttr.setValue(theFile)

    aSelectionListAttrName = "selection_list"
    aSelectionListAttr = anExportFeature.selectionList(aSelectionListAttrName)
    assert aSelectionListAttr, "{0}: Can not receive selection list field {1}".format(theType, aSelectionListAttrName)
    aSelectionListAttr.setSelectionType("solids")
    aSelectionListAttr.append(anImportFeature.firstResult(), anImportFeature.firstResult().shape())

    anExportFeature.execute()
    aSession.finishOperation()

    assert os.path.exists(theFile), "{0}: Can not find exported file {1}".format(theType, theFile)

    # Test exported file by importing
    testImport(theType, theFile, theVolume, theDelta)

def testExportXAO():
    # Import a reference part
    aSession.startOperation("Add part")
    aPartFeature = aSession.moduleDocument().addFeature("Part")
    aSession.finishOperation()
    aPart = aSession.activeDocument()

    aSession.startOperation("Import Box_1")
    anImportFeature = aPart.addFeature("Import")
    anImportFeature.string("file_path").setValue("Data/Box_1.brep")
    anImportFeature.execute()
    aSession.finishOperation()

    # Create groups
    aSession.startOperation()
    aGroupFeature = aSession.activeDocument().addFeature("Group")
    aGroupFeature.data().setName("boite_1")
    aSelectionListAttr = aGroupFeature.selectionList("group_list")
    aSelectionListAttr.setSelectionType("solid")
    aSelectionListAttr.append(anImportFeature.lastResult(), None)
    aGroupFeature.execute()
    aSession.finishOperation()

    aSession.startOperation()
    aGroupFeature = aSession.activeDocument().addFeature("Group")
    aGroupFeature.data().setName("")
    aSelectionListAttr = aGroupFeature.selectionList("group_list")
    aSelectionListAttr.setSelectionType("face")
    aSelectionListAttr.append("Box_1_1/Shape1_1")
    aSelectionListAttr.append("Box_1_1/Shape2_1")
    aGroupFeature.execute()
    aSession.finishOperation()

    # Export
    aSession.startOperation("Export")
    anExportFeature = aPart.addFeature("Export")
#     anExportFeature.string("ExportType").setValue("XAO")
    anExportFeature.string("file_path").setValue("Data/export.xao")
#     anExportFeature.string("file_format").setValue("XAO")
    anExportFeature.string("xao_author").setValue("me")
    anExportFeature.string("xao_geometry_name").setValue("mygeom")
    anExportFeature.execute()
    aSession.finishOperation()

    # Check exported file
    import filecmp
    assert filecmp.cmp("Data/export.xao", "Data/export_ref.xao")

if __name__ == '__main__':
#=========================================================================
# Export a shape into BREP
#=========================================================================
    aRealVolume = 3.78827401738e-06
    testExport("BREP", "BREP", os.path.join(os.getcwd(), "Data", "screw_export.brep"), aRealVolume, 10 ** -17)
    testExport("BRP", "BREP", os.path.join(os.getcwd(), "Data", "screw_export.brp"), aRealVolume, 10 ** -17)
#=========================================================================
# Export a shape into STEP
#=========================================================================
    testExport("STEP", "STEP", os.path.join(os.getcwd(), "Data", "screw_export.step"), 3.78825807533e-06, 10 ** -17)
    testExport("STP", "STEP", os.path.join(os.getcwd(), "Data", "screw_export.stp"), 3.78825807533e-06, 10 ** -17)
#=========================================================================
# Export a shape into IGES
#=========================================================================
    testExport("IGES-5.1", "IGES-5.1", os.path.join(os.getcwd(), "Data", "screw_export-5.1.iges"), 3.78829613776e-06, 10 ** -17)
    testExport("IGS-5.1", "IGES-5.1", os.path.join(os.getcwd(), "Data", "screw_export-5.1.igs"), 3.78829613776e-06, 10 ** -17)
    testExport("IGES-5.3", "IGES-5.3", os.path.join(os.getcwd(), "Data", "screw_export-5.3.iges"), 3.78827401651e-06, 10 ** -17)
    testExport("IGS-5.3", "IGES-5.3", os.path.join(os.getcwd(), "Data", "screw_export-5.3.igs"), 3.78827401651e-06, 10 ** -17)
#=========================================================================
# Export a shape into XAO
#=========================================================================
    testExportXAO()
#=========================================================================
# End of test
#=========================================================================
