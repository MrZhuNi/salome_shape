# Copyright (C) 2014-2016 CEA/DEN, EDF R&D

# File:        APIDirectTestSphere.py
# Created:     15 Apr 2016
# Author:      CEA (delegation to Alyotech)

from GeomAlgoAPI import GeomAlgoAPI_ShapeAPI as shaperpy
from GeomAlgoAPI import GeomAlgoAPI_Exception as myExcept
from GeomAPI import GeomAPI_Pnt as pnt

# Create a sphere with radius 5. at origin   
try :    
  sphere1 = shaperpy.makeSphere(5.)
  
except myExcept,ec:
  print ec.what()

# Create a sphere with a point defining the center and radius 8.   
try :    
  center = pnt(10.,0.,0.)
  sphere2 = shaperpy.makeSphere(center, 8.)
  
except myExcept,ec:
  print ec.what()
  

# Create a sphere with null radius
try :    
  sphere3 = shaperpy.makeSphere(0.)
  
except myExcept,ec:
  print ec.what()
