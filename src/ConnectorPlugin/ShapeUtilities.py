import EventsAPI
import ModelAPI
import GeomAlgoAPI

## Make the shape from the part
# Utility function used in Shaper's ExportToGeom and Geom's RestoreShape
def makeShape(aPart):
  kResultBodyType = "Bodies"
  aPartSize = aPart.size(kResultBodyType)
  if aPartSize == 0:
    EventsAPI.Events_InfoMessage("ExportFeature","No results in the active document").send()
    return

  anObjList = [aPart.object(kResultBodyType, idx) for idx in xrange(aPartSize)]
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
    aShape = GeomAlgoAPI.GeomAlgoAPI_CompoundBuilder.compound(aShapesList)
    aName = "ShaperResults"
  elif len(aShapesList) == 1:
    aShape = aShapesList[0]

  aShape.name = aName

  return aShape