# Copyright (C) 2014-2019  CEA/DEN, EDF R&D
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
#
# See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
#


from ModelAPI import *
from GeomAPI import *
from GeomAlgoAPI import GeomAlgoAPI_MapShapesAndAncestors as mapShapesAndAncestors

FILTER_ID = "SameNormalFaces"

def singleton(cls):
  instance = cls()
  instance.__call__ = lambda: instance
  return instance

@singleton
class FiltersPlugin_SameNormalFaces(ModelAPI_Filter):
  """
  Filter for faces topologically connected to the selected object.
  """

  def __init__(self):
    """x.__init__(...) initializes x; see x.__class__.__doc__ for signature"""
    ModelAPI_Filter.__init__(self)
    self.myCached = {}

  def name(self):
    """ Description of the filter """
    return "Same normal faces"

  def isSupported(self, theType):
    """ Supported types of filtered shapes """
    return theType == GeomAPI_Shape.FACE

  def isOk(self, theShape, theResult, theArgs):
    """ True if theShape is applicable for the filter """

    selectedShapeAttr = modelAPI_AttributeSelection(theArgs.argument("SameNormalFaces"))
    if selectedShapeAttr is None:
      return False
    selectedShape = selectedShapeAttr.value()
    self.myCached[selectedShape] = []
    selectedFace = GeomAPI_Face(selectedShape)
    selectedPlane = selectedFace.getPlane()
    selectedNormal = selectedPlane.direction()

    Face = GeomAPI_Face(theShape)
    if not Face.isPlanar():
        return self.myCached[selectedShape]
    Plane = Face.getPlane()
    Normal = Plane.direction()
    if selectedPlane.intersect(Plane) is None and selectedNormal.angle(Normal) < math.pi/360:
      self.myCached[selectedShape].append(theShape)

    return theShape in self.myCached[selectedShape]

  def xmlRepresentation(self):
    """ Returns XML string which represents GUI of the filter """
    return self.xmlFromFile("filter-SameNormalFaces.xml")

  def initAttributes(self, theArgs):
    """ Initializes arguments of a filter """
    theArgs.initAttribute("SameNormalFaces", ModelAPI_AttributeSelection_typeId())

  def adjacentFaces(self, theFace, theMapSA, theShapeType, theApplicableFaces, theRecursive = True):
    """ Find all faces neighbour to theFace """
    exp = GeomAPI_ShapeExplorer(theFace, theShapeType)
    while exp.more():
      if  exp.current() in theMapSA:
        faces = theMapSA[exp.current()]
        for ind in range(faces.size()):
          f = faces[ind]
          if f not in theApplicableFaces:
            theApplicableFaces.insert(f)
            if theRecursive:
              self.adjacentFaces(f, theMapSA, theShapeType, theApplicableFaces)
      exp.next()


# Register the filter object
filter = FiltersPlugin_SameNormalFaces
aSession = ModelAPI_Session.get()
aSession.filters().registerFilter(FILTER_ID, filter)
