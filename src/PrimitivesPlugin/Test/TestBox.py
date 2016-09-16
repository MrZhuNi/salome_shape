"""
Test case for Primitive Box feature. 
Written on High API.
"""
from ModelAPI import *
from GeomAPI import *

import model

# Get session
aSession = ModelAPI_Session.get()

# Create a part
aDocument = aSession.activeDocument()
aSession.startOperation()
model.addPart(aDocument)
aDocument = aSession.activeDocument()
aSession.finishOperation()

# Create a box with dimensions
aSession.startOperation()
aBox1 = model.addBox(aDocument, 10, 10, 10)
aSession.finishOperation()
assert (len(aBox1.result()) > 0)

# Create a first point
aSession.startOperation()
aFirstPoint = model.addPoint(aDocument, 0, 0, 0)
aSession.finishOperation()

# Create a second point
aSession.startOperation()
aSecondPoint = model.addPoint(aDocument, 50, 50, 50)
aSession.finishOperation()

# Create a box with 2 points
aSession.startOperation()
aBox2 = model.addBox(aDocument, aFirstPoint.result()[0], aSecondPoint.result()[0])
aSession.finishOperation()
assert (len(aBox2.result()) > 0)

# Create a box with dimensions (error)
aSession.startOperation()
aBox3 = model.addBox(aDocument, -10, 10, 10)
aSession.finishOperation()
assert (len(aBox3.result()) == 0)

# Create a box with 2 points (error)
aSession.startOperation()
aBox4 = model.addBox(aDocument, aFirstPoint.result()[0], aFirstPoint.result()[0])
aSession.finishOperation()
assert (len(aBox4.result()) == 0)

