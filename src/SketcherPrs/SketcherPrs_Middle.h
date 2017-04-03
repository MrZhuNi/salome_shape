// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        SketcherPrs_Middle.h
// Created:     29 February 2016
// Author:      Natalia ERMOLAEVA

#ifndef SketcherPrs_Middle_H
#define SketcherPrs_Middle_H

#include "SketcherPrs_SymbolPrs.h"


DEFINE_STANDARD_HANDLE(SketcherPrs_Middle, SketcherPrs_SymbolPrs)

/**
* \ingroup GUI
* A redefinition of standard AIS Interactive Object in order to provide  
* presentation of Equal constraint
*/
class SketcherPrs_Middle: public SketcherPrs_SymbolPrs
{
public:
  /// Constructor
  /// \param theConstraint a constraint feature
  /// \param thePlane a coordinate plane of current sketch
  Standard_EXPORT SketcherPrs_Middle(ModelAPI_Feature* theConstraint,
                                       const std::shared_ptr<GeomAPI_Ax3>& thePlane);
  DEFINE_STANDARD_RTTIEXT(SketcherPrs_Middle, SketcherPrs_SymbolPrs)

  /// Returns true if the constraint feature arguments are correcly filled to build AIS presentation
  /// \param theConstraint a constraint feature
  /// \param thePlane a coordinate plane of current sketch
  /// \return boolean result value
  static bool IsReadyToDisplay(ModelAPI_Feature* theConstraint,
                               const std::shared_ptr<GeomAPI_Ax3>& thePlane);
protected:

  virtual const char* iconName() const { return "middlepoint.png"; }

  virtual void drawLines(const Handle(Prs3d_Presentation)& thePrs, Quantity_Color theColor) const;

  /// Update myPntArray according to presentation positions
  /// \return true in case of success
  virtual bool updateIfReadyToDisplay(double theStep, bool withColor) const;

  /// Draw shape of the object. Find shape result if the object is feature
  void drawLine(const Handle(Prs3d_Presentation)& thePrs, Quantity_Color theColor,
                const ObjectPtr& theObject) const;
};

#endif