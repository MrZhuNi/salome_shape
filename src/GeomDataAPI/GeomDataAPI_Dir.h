// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        GeomDataAPI_Dir.h
// Created:     24 Apr 2014
// Author:      Mikhail PONIKAROV

#ifndef GeomDataAPI_Dir_H_
#define GeomDataAPI_Dir_H_

#include "GeomDataAPI.h"
#include <ModelAPI_Attribute.h>

class GeomAPI_Dir;
class GeomAPI_XYZ;

/**\class GeomDataAPI_Dir
 * \ingroup DataModel
 * \brief Attribute that contains 3D direction coordinates. 
 */

class GeomDataAPI_Dir : public ModelAPI_Attribute
{
 public:
  /// Defines the double value
  virtual void setValue(const double theX, const double theY, const double theZ) = 0;
  /// Defines the direction
  virtual void setValue(const std::shared_ptr<GeomAPI_Dir>& theDir) = 0;

  /// Returns the X double value
  virtual double x() const = 0;
  /// Returns the Y double value
  virtual double y() const = 0;
  /// Returns the Z double value
  virtual double z() const = 0;
  /// Returns the direction of this attribute
  virtual std::shared_ptr<GeomAPI_Dir> dir() = 0;
  /// Returns the coordinates of this attribute
  virtual std::shared_ptr<GeomAPI_XYZ> xyz() = 0;

  /// Returns the type of this class of attributes
  static std::string typeId()
  {
    return std::string("Dir");
  }

  /// Returns the type of this class of attributes, not static method
  GEOMDATAAPI_EXPORT virtual std::string attributeType();

 protected:
  /// Objects are created for features automatically
  GEOMDATAAPI_EXPORT GeomDataAPI_Dir();
  GEOMDATAAPI_EXPORT virtual ~GeomDataAPI_Dir();
};

//! Pointer on attribute object
typedef std::shared_ptr<GeomDataAPI_Dir> AttributeDirPtr;

#endif
