from GeomAlgoAPI import GeomAlgoAPI_DirectAPI as shaperpy
from GeomAlgoAPI import GeomAlgoAPI_Exception as myExcept
from GeomAPI import GeomAPI_Pnt as pnt

import GEOM
import salome
from salome.geom import geomBuilder
geompy = geomBuilder.New(salome.myStudy)

# Create a box with dimensions    
try :    
  box1 = shaperpy.MakeBox(5.,15.,5.)
  aDump1 = box1.getShapeStream()
  aBrep1 = geompy.RestoreShape(aDump1)
  geompy.addToStudy(aBrep1, "box1")
  
except myExcept,ec:
  print ec.what()

# Create a box with two points defining the diagonal   
try :    
  pnt1 = pnt(0.,0.,0.)
  pnt2 = pnt(10.,10.,10.)
  box2 = shaperpy.MakeBox(pnt1,pnt2)
  aDump2 = box2.getShapeStream()
  aBrep2 = geompy.RestoreShape(aDump2)
  geompy.addToStudy(aBrep2, "box2")
  
except myExcept,ec:
  print ec.what()
  

# Create a box with null dimensions
try :    
  box3 = shaperpy.MakeBox(0.,0.,0.)
  aDump3 = box1.getShapeStream()
  aBrep3 = geompy.RestoreShape(aDump3)
  geompy.addToStudy(aBrep1, "box3")
  
except myExcept,ec:
  print ec.what()