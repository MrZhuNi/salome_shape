// Copyright (C) 2014-201x CEA/DEN, EDF R&D -->

// File:        FeaturesAPI_Scale.h
// Created:     24 Jan 2017
// Author:      Clarisse Genrault (CEA)

#ifndef FEATURESAPI_SCALE_H_
#define FEATURESAPI_SCALE_H_

#include "FeaturesAPI.h"

#include <FeaturesPlugin_Scale.h>

#include <ModelHighAPI_Interface.h>
#include <ModelHighAPI_Macro.h>

class ModelHighAPI_Double;
class ModelHighAPI_Dumper;
class ModelHighAPI_Selection;

/// \class FeaturesAPI_Scale
/// \ingroup CPPHighAPI
/// \brief Interface for Scale feature.
class FeaturesAPI_Scale: public ModelHighAPI_Interface
{
public:
  /// Constructor without values.
  FEATURESAPI_EXPORT
  explicit FeaturesAPI_Scale(const std::shared_ptr<ModelAPI_Feature>& theFeature);

  /// Constructor with values.
  FEATURESAPI_EXPORT
  explicit FeaturesAPI_Scale(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                             const std::list<ModelHighAPI_Selection>& theMainObjects,
                             const ModelHighAPI_Selection& theCenterPoint,
                             const ModelHighAPI_Double& theScaleFactor);

  /// Constructor with values.
  FEATURESAPI_EXPORT
  explicit FeaturesAPI_Scale(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                             const std::list<ModelHighAPI_Selection>& theMainObjects,
                             const ModelHighAPI_Selection& theCenterPoint,
                             const ModelHighAPI_Double& theScaleFactorX,
                             const ModelHighAPI_Double& theScaleFactorY,
                             const ModelHighAPI_Double& theScaleFactorZ);

  /// Destructor.
  FEATURESAPI_EXPORT
  virtual ~FeaturesAPI_Scale();

  INTERFACE_7(FeaturesPlugin_Scale::ID(),
              creationMethod, FeaturesPlugin_Scale::CREATION_METHOD(),
              ModelAPI_AttributeString, /** Creation method */,
              mainObjects, FeaturesPlugin_Scale::OBJECTS_LIST_ID(),
              ModelAPI_AttributeSelectionList, /** Main objects */,
              centerPoint, FeaturesPlugin_Scale::CENTER_POINT_ID(),
              ModelAPI_AttributeSelection, /** Center point */,
              scaleFactor, FeaturesPlugin_Scale::SCALE_FACTOR_ID(),
              ModelAPI_AttributeDouble, /** Scale factor */,
              scaleFactorX, FeaturesPlugin_Scale::SCALE_FACTOR_X_ID(),
              ModelAPI_AttributeDouble, /** Scale factor in X */,
              scaleFactorY, FeaturesPlugin_Scale::SCALE_FACTOR_Y_ID(),
              ModelAPI_AttributeDouble, /** Scale factor in Y */,
              scaleFactorZ, FeaturesPlugin_Scale::SCALE_FACTOR_Z_ID(),
              ModelAPI_AttributeDouble, /** Scale factor in Z */)

  /// Set main objects.
  FEATURESAPI_EXPORT
  void setMainObjects(const std::list<ModelHighAPI_Selection>& theMainObjects);

  /// Modify center_point attribute of the feature.
  FEATURESAPI_EXPORT
  void setCenterPoint(const ModelHighAPI_Selection& theCenterPoint);

  /// Modify Creation Method and scale_factor attribute of the feature.
  FEATURESAPI_EXPORT
  void setScaleFactor(const ModelHighAPI_Double& theScaleFactor);

  /// Modify Creation Method and scale_factor_x, scale_factor_y and scale_factor_z
  /// attributes of the feature.
  FEATURESAPI_EXPORT
  void setDimensions(const ModelHighAPI_Double& theScaleFactorX,
                     const ModelHighAPI_Double& theScaleFactorY,
                     const ModelHighAPI_Double& theScaleFactorZ);

  /// Dump wrapped feature
  FEATURESAPI_EXPORT
  virtual void dump(ModelHighAPI_Dumper& theDumper) const;
};

/// Pointer on Scale object.
typedef std::shared_ptr<FeaturesAPI_Scale> ScalePtr;

/// \ingroup CPPHighAPI
/// \brief Create Scale feature.
FEATURESAPI_EXPORT
ScalePtr addScale(const std::shared_ptr<ModelAPI_Document>& thePart,
                  const std::list<ModelHighAPI_Selection>& theMainObjects,
                  const ModelHighAPI_Selection& theCenterPoint,
                  const ModelHighAPI_Double& theScaleFactor);

/// \ingroup CPPHighAPI
/// \brief Create Scale feature.
FEATURESAPI_EXPORT
ScalePtr addScale(const std::shared_ptr<ModelAPI_Document>& thePart,
                  const std::list<ModelHighAPI_Selection>& theMainObjects,
                  const ModelHighAPI_Selection& theCenterPoint,
                  const ModelHighAPI_Double& theScaleFactorX,
                  const ModelHighAPI_Double& theScaleFactorY,
                  const ModelHighAPI_Double& theScaleFactorZ);

#endif // FEATURESAPI_SCALE_H_