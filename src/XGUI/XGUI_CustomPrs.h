// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        XGUI_CustomPrs.hxx
// Created:     10 Mar 2015
// Author:      Natalia ERMOLAEVA

#ifndef XGUI_CustomPrs_H
#define XGUI_CustomPrs_H

#include "XGUI.h"
#include <GeomAPI_ICustomPrs.h>
#include <GeomAPI_AISObject.h>
#include <ModelAPI_Result.h>

class XGUI_Workshop;

/**
* Interface of a class which can provide specific customization of
* object presentation
*/ 
class XGUI_CustomPrs : public GeomAPI_ICustomPrs
{
public:
  /// Constructor
  /// \param theWorkshop the current workshop instance
  XGUI_EXPORT XGUI_CustomPrs(XGUI_Workshop* theWorkshop);

  XGUI_EXPORT virtual ~XGUI_CustomPrs() {};

  /// Modifies the given presentation in the custom way.
  virtual bool customisePresentation(ResultPtr theResult, AISObjectPtr thePrs,
                                     std::shared_ptr<GeomAPI_ICustomPrs> theCustomPrs);

  /// Returns color of a result object
  /// \param theResult a result object
  /// \param theColor a color in form of RGB vector
  static void getResultColor(ResultPtr theResult, std::vector<int>& theColor);

  /// Returns the default object color. It obtains colorConfigInfo of the object
  /// and find it in preferences. If there are no this color in preference and an empty
  /// color is interpreted as invalid, it shows error message
  /// \param theObject an investigated object
  /// \param isEmptyColorValid boolean state about interpretation of empty color
  /// \param theColor the result color
  static void XGUI_EXPORT getDefaultColor(ObjectPtr theObject, const bool isEmptyColorValid,
                                          std::vector<int>& theColor);


protected:
  XGUI_Workshop* myWorkshop; ///< the current workshop
};

#endif
