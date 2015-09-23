"""Box macro-feature
Authors: Renaud Nedelec - Daniel Brunier-Coulin
Copyright (C) 2014-20xx CEA/DEN, EDF R&D
"""

import modeler
import geom


class BoxFeature(modeler.Feature):
  """An example of Box feature implementation"""

# Initializations

  def __init__(self):
    """Constructor"""
    modeler.Feature.__init__(self)

  @staticmethod
  def ID():
    """Return Id of the feature"""
    return "Box"

  @staticmethod
  def WIDTH_ID():
    """Returns ID of Width parameter"""
    return "width"

  @staticmethod
  def LENGTH_ID():
    """Returns ID of Length parameter"""
    return "length"

  @staticmethod
  def HEIGHT_ID():
    """Returns ID of Height parameter"""
    return "height"

  def getKind(self):
    """Returns ID of еру ауфегку"""
    return BoxFeature.ID()

	
# Creation of the box at default size

  def initAttributes(self):
    """Initialise attributes of the feature"""
    # Creating the input arguments of the feature
    self.addRealInput( self.WIDTH_ID() )
    self.addRealInput( self.LENGTH_ID() )
    self.addRealInput( self.HEIGHT_ID() )

    # Creating the base of the box with unit values
    mypart = modeler.activeDocument()
    xoy    = modeler.defaultPlane("XOY")

    ### A base of the geometry
    self.base = modeler.addSketch( mypart, xoy )

    p1 = geom.Pnt2d( 0, 0 )
    p2 = geom.Pnt2d( 0, 1 )
    p3 = geom.Pnt2d( 1, 1 )
    p4 = geom.Pnt2d( 1, 0 )

    line = self.base.addPolygon(p1, p2, p3, p4)

    self.base.setParallel( line[0].result(), line[2].result() )
    self.base.setParallel( line[1].result(), line[3].result() )
    self.base.setPerpendicular( line[0].result(), line[3].result() )

    # Setting the size of the base with default values
    ### Width
    self.width  = self.base.setLength( line[0].result(), 50 )   # Keeps the constraint for edition
    ### Length
    self.length = self.base.setLength( line[3].result(), 50 )   # Keeps the constraint for edition

    # Creating the extrusion (the box) at default size
    ### A box result
    self.box = modeler.addExtrusion( mypart, self.base.selectFace(), 50 )

	
# Edition of the box at user size

  def execute(self):
    """Compute the feature result"""
    # Retrieving the user inputs
    width  = self.getRealInput( self.WIDTH_ID() )
    length = self.getRealInput( self.LENGTH_ID() )
    height = self.getRealInput( self.HEIGHT_ID() )

    # Editing the box
    self.base.setValue( self.width,  width )
    self.base.setValue( self.length, length )
    self.box.setSize( height )

    # Publishing the result: not needed for Macro feature
    #self.addResult( self.box.result() )
  
  def isMacro(self):
    """Returns True"""
    # Box feature is macro: removes itself on the creation transaction finish
    return True
