// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        GeomData_Dir.h
// Created:     24 Apr 2014
// Author:      Mikhail PONIKAROV

#ifndef GeomData_Dir_H_
#define GeomData_Dir_H_

#include "GeomData.h"
#include "GeomDataAPI_Dir.h"
#include <TDataStd_RealArray.hxx>
#include <TDF_Label.hxx>
#include <memory>

class GeomAPI_Dir;
class GeomAPI_XYZ;

/**\class GeomData_Dir
 * \ingroup DataModel
 * \brief Attribute that contains direction.
 */
class GeomData_Dir : public GeomDataAPI_Dir
{
  TDF_Label myLab; ///< the main label of the attribute
  Handle_TDataStd_RealArray myCoords;  ///< X, Y and Z doubles as real array attribute [0; 2]
 public:
  /// Defines the double value
  GEOMDATA_EXPORT virtual void setValue(const double theX, const double theY, const double theZ);
  /// Defines the direction
  GEOMDATA_EXPORT virtual void setValue(const std::shared_ptr<GeomAPI_Dir>& theDir);

  /// Returns the X double value
  GEOMDATA_EXPORT virtual double x() const;
  /// Returns the Y double value
  GEOMDATA_EXPORT virtual double y() const;
  /// Returns the Z double value
  GEOMDATA_EXPORT virtual double z() const;
  /// Returns the direction of this attribute
  GEOMDATA_EXPORT virtual std::shared_ptr<GeomAPI_Dir> dir();
  /// Returns the coordinates of this attribute
  GEOMDATA_EXPORT virtual std::shared_ptr<GeomAPI_XYZ> xyz();

 protected:
  /// Initializes attributes
  GEOMDATA_EXPORT GeomData_Dir(TDF_Label& theLabel);
  /// Reinitializes the internal state of the attribute (may be needed on undo/redo, abort, etc)
  virtual void reinit();

  friend class Model_Data;
};

#endif
