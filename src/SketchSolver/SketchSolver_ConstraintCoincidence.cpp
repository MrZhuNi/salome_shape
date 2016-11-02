// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

#include <SketchSolver_ConstraintCoincidence.h>
#include <SketchSolver_Error.h>
#include <SketchSolver_Manager.h>

void SketchSolver_ConstraintCoincidence::getAttributes(
    double& theValue,
    std::vector<EntityWrapperPtr>& theAttributes)
{
  SketchSolver_Constraint::getAttributes(theValue, theAttributes);
  if (!myErrorMsg.empty() || !theAttributes[0]) {
    theAttributes.clear();
    return;
  }

  if (theAttributes[1]) {
    myType = CONSTRAINT_PT_PT_COINCIDENT;
    if (myStorage->isFixed(theAttributes[1]) && !myStorage->isFixed(theAttributes[0])) {
      // fixed point should go first
      EntityWrapperPtr aTemp = theAttributes[0];
      theAttributes[0] = theAttributes[1];
      theAttributes[1] = aTemp;
    }
    // Set the slave (second) point the same as master (first) point.
    // This will allow to skip adding point-point coincidence to the set of constraints
    // and give us speed-up in solving the set of equations
    myStorage->addCoincidentPoints(theAttributes[0], theAttributes[1]);
  }
  else if (theAttributes[2]) {
    // check the type of entity (line or circle)
    SketchSolver_EntityType anEntType = theAttributes[2]->type();
    if (anEntType == ENTITY_LINE)
      myType = CONSTRAINT_PT_ON_LINE;
    else if (anEntType == ENTITY_CIRCLE || anEntType == ENTITY_ARC)
      myType = CONSTRAINT_PT_ON_CIRCLE;
    else
      myErrorMsg = SketchSolver_Error::INCORRECT_ATTRIBUTE();
  } else
    myErrorMsg = SketchSolver_Error::INCORRECT_ATTRIBUTE();
}
