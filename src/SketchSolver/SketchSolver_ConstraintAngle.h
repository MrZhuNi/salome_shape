// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:    SketchSolver_ConstraintAngle.h
// Created: 24 August 2015
// Author:  Artem ZHIDKOV

#ifndef SketchSolver_ConstraintAngle_H_
#define SketchSolver_ConstraintAngle_H_

#include <SketchSolver_Constraint.h>

/** \class   SketchSolver_ConstraintAngle
 *  \ingroup Plugins
 *  \brief   Convert Agnle constraint to SolveSpace structure
 */
class SketchSolver_ConstraintAngle : public SketchSolver_Constraint
{
public:
  /// Constructor based on SketchPlugin constraint
  SketchSolver_ConstraintAngle(ConstraintPtr theConstraint)
    : SketchSolver_Constraint(theConstraint)
  {}

  /// \brief This method is used in derived objects to check consistence of constraint.
  virtual void adjustConstraint() override;

protected:
  /// \brief Generate list of attributes of constraint in order useful for constraints
  /// \param[out] theValue      numerical characteristic of constraint (e.g. distance)
  /// \param[out] theAttributes list of attributes to be filled
  virtual void getAttributes(EntityWrapperPtr&              theValue,
                             std::vector<EntityWrapperPtr>& theAttributes) override;

private:
  int    myType;
};

#endif
