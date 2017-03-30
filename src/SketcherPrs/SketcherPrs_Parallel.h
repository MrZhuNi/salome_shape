// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        SketcherPrs_Parallel.h
// Created:     16 February 2015
// Author:      Vitaly SMETANNIKOV

#ifndef SketcherPrs_Parallel_H
#define SketcherPrs_Parallel_H

#include "SketcherPrs_SymbolPrs.h"


DEFINE_STANDARD_HANDLE(SketcherPrs_Parallel, SketcherPrs_SymbolPrs)

/**
* \ingroup GUI
* A redefinition of standard AIS Interactive Object in order to provide  
* presentation of parallel constraint
*/
class SketcherPrs_Parallel: public SketcherPrs_SymbolPrs
{
public:
  /// Constructor
  /// \param theConstraint a constraint feature
  /// \param thePlane a coordinate plane of current sketch
  Standard_EXPORT SketcherPrs_Parallel(ModelAPI_Feature* theConstraint,
                                       const std::shared_ptr<GeomAPI_Ax3>& thePlane);
  DEFINE_STANDARD_RTTIEXT(SketcherPrs_Parallel, SketcherPrs_SymbolPrs)

  /// Returns true if the constraint feature arguments are correcly filled to build AIS presentation
  /// \param theConstraint a constraint feature
  /// \param thePlane a coordinate plane of current sketch
  /// \return boolean result value
  static bool IsReadyToDisplay(ModelAPI_Feature* theConstraint,
                               const std::shared_ptr<GeomAPI_Ax3>& thePlane);

protected:
  virtual const char* iconName() const { return "parallel.png"; }

  virtual void drawLines(const Handle(Prs3d_Presentation)& thePrs, Quantity_Color theColor) const;

  /// Update myPntArray according to presentation positions
  /// \return true in case of success
  virtual bool updateIfReadyToDisplay(double theStep, bool withColor) const;
};

#endif