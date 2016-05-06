## @package Plugins
#  ExportFeature class definition
#  Copyright (C) 2014-20xx CEA/DEN, EDF R&D

import EventsAPI
import ModelAPI
import GeomAPI
import GeomAlgoAPI

import salome
from salome.geom import geomBuilder

def getObjectIndex(theName):
    aStudy = salome.myStudy
    aId = 0
    aObj = aStudy.FindObjectByName(theName, "GEOM")
    while len(aObj) != 0:
        aId = aId + 1
        aName = theName + '_' + str(aId)
        aObj = aStudy.FindObjectByName(aName, "GEOM")
    return aId

## @ingroup Plugins
#  Feature to export all shapes and groups into the GEOM module
class ExportFeature(ModelAPI.ModelAPI_Feature):

    ## The constructor.
    def __init__(self):
        ModelAPI.ModelAPI_Feature.__init__(self)
        ## Shape that will be exported (the compound if there are several exported bodies)
        self.shape = None
        ## BRep representation of the exported shape (a stream that will be sent to GEOM and converted to GEOM object)
        self.brep = None

    @staticmethod
    ## Export kind. Static.
    def ID():
        return "ExportToGEOM"

    ## Returns the kind of a feature.
    def getKind(self):
        return ExportFeature.ID()

    ## This feature is action: has no property pannel and executes immideately.
    def isAction(self):
        return True

    ## This feature has no attributes, as it is action.
    def initAttributes(self):
      pass

    ## Exports all bodies
    def exportBodies(self):
        global ShapeIndex
        kResultBodyType = "Bodies"
        aPartSize = self.Part.size(kResultBodyType)
        if aPartSize == 0:
            EventsAPI.Events_Error_send("No results in the active document")
            return

        anObjList = [self.Part.object(kResultBodyType, idx) for idx in xrange(aPartSize)]
        aShapesList = GeomAlgoAPI.ShapeList()
        aName = ""
        for idx, anObject in enumerate(anObjList):
            aResult = ModelAPI.modelAPI_Result(anObject)
            aBodyResult = ModelAPI.modelAPI_ResultBody(aResult)
            if not aBodyResult:
                continue
            aShape = aBodyResult.shape()
            if aShape is not None and not aShape.isNull():
              aShapesList.append(aShape)
              if len(aShapesList) == 1:
                aName = aBodyResult.data().name()

        # issue 1045: create compound if there are more than one shape
        if len(aShapesList) > 1:
          self.shape = GeomAlgoAPI.GeomAlgoAPI_CompoundBuilder.compound(aShapesList)
          aName = "ShaperResults"
        elif len(aShapesList) == 1:
          self.shape = aShapesList[0]

        # so, only one shape is always in the result
        aDump = self.shape.getShapeStream()
        # Load shape to SALOME Geom
        aBrep = self.geompy.RestoreShape(aDump)

        # Make unique name
        aId = getObjectIndex(aName)
        if aId != 0:
            aName = aName + '_' + str(aId)

        self.geompy.addToStudy(aBrep, aName)
        self.brep = aBrep

    ## Exports all groups
    def exportGroups(self):
        # iterate all features to find groups
        aFeaturesNum = self.Part.size("Features")
        groupIndex = 0
        for anIndex in range(0, aFeaturesNum):
            aFeature = self.Part.object("Features", anIndex)
            aSelectionList = aFeature.data().selectionList("group_list")
            # if a group has been found
            if aSelectionList:
                aFeature = ModelAPI.objectToFeature(aFeature)
                if aFeature.firstResult() is not None:
                  aName = aFeature.firstResult().data().name()
                groupIndex = groupIndex + 1
                self.createGroupFromList(aSelectionList, aName)

    ## Returns a type of the shape in the old GEOM representation
    def shapeType(self, shape):
        if shape.isVertex():
            return "VERTEX"
        elif shape.isEdge():
            return "EDGE"
        elif shape.isFace():
            return "FACE"

        return "SOLID"

    ## Creates a group by given list of selected objects and the name
    #  @param theSelectionList: list of selected objects
    #  @param theGroupName: name of the group to create
    def createGroupFromList(self, theSelectionList, theGroupName):
        # iterate on all selected entities of the group
        # and get the corresponding ID
        aSelectionNum = theSelectionList.size()
        Ids = []
        groupType = ""
        for aSelIndex in range(0, aSelectionNum):
            aSelection = theSelectionList.value(aSelIndex)
            # issue 1326: bodies that are already concealed did not exported, so groups should not be invalid
            aContext =  ModelAPI.modelAPI_Result(aSelection.context())
            if aContext is None or aContext.isConcealed() or aContext.isDisabled():
                continue

            anID = GeomAlgoAPI.GeomAlgoAPI_CompoundBuilder.id(self.shape, aSelection.value())
            if anID == 0:
                #it may be a compound of objects if movement of the group to the end
                # splits the original element to several (issue 1146)
                anExp = GeomAPI.GeomAPI_ShapeExplorer(aSelection.value(), GeomAPI.GeomAPI_Shape.SHAPE)
                while anExp.more():
                    anID = GeomAlgoAPI.GeomAlgoAPI_CompoundBuilder.id(self.shape, anExp.current())
                    if anID != 0:
                        Ids.append(anID)
                        groupType = self.shapeType(anExp.current())
                    anExp.next()
            else:
                Ids.append(anID)
                groupType = self.shapeType(aSelection.value())

        if len(Ids) <> 0:
          aGroup = self.geompy.CreateGroup(self.brep, self.geompy.ShapeType[groupType])
          self.geompy.UnionIDs(aGroup,Ids)
          self.geompy.addToStudyInFather(self.brep, aGroup, theGroupName)

    ## Exports all shapes and groups into the GEOM module.
    def execute(self):
        aSession = ModelAPI.ModelAPI_Session.get()
        ## Get active document
        self.Part = aSession.activeDocument()
        ## List of objects created in the old geom for later use
        self.geomObjects = []
        ## geomBuilder tool
        self.geompy = geomBuilder.New(salome.myStudy)

        # Export bodies and groups
        self.exportBodies()
        self.exportGroups()
        pass
