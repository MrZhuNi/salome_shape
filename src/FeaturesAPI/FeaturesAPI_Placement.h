// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

// File:        FeaturesAPI_Placement.h
// Created:     07 June 2016
// Author:      Dmitry Bobylev

#ifndef FeaturesAPI_Placement_H_
#define FeaturesAPI_Placement_H_

#include "FeaturesAPI.h"

#include <FeaturesPlugin_Placement.h>

#include <ModelHighAPI_Interface.h>
#include <ModelHighAPI_Macro.h>

class ModelHighAPI_Dumper;
class ModelHighAPI_Selection;

/// \class FeaturesAPI_Placement
/// \ingroup CPPHighAPI
/// \brief Interface for Placement feature.
class FeaturesAPI_Placement: public ModelHighAPI_Interface
{
public:
  /// Constructor without values.
  FEATURESAPI_EXPORT
  explicit FeaturesAPI_Placement(const std::shared_ptr<ModelAPI_Feature>& theFeature);

  /// Constructor with values.
  FEATURESAPI_EXPORT
  FeaturesAPI_Placement(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                        const std::list<ModelHighAPI_Selection>& theObjects,
                        const ModelHighAPI_Selection& theStartShape,
                        const ModelHighAPI_Selection& theEndShape,
                        const bool theReverseDirection = false,
                        const bool theCentering = false);

  /// Destructor.
  FEATURESAPI_EXPORT
  virtual ~FeaturesAPI_Placement();

  INTERFACE_5(FeaturesPlugin_Placement::ID(),
              objects, FeaturesPlugin_Placement::OBJECTS_LIST_ID(), 
              ModelAPI_AttributeSelectionList, /** Main objects */,
              startShape, FeaturesPlugin_Placement::START_SHAPE_ID(),
              ModelAPI_AttributeSelection, /** Start shape */,
              endShape, FeaturesPlugin_Placement::END_SHAPE_ID(), 
              ModelAPI_AttributeSelection, /** End shape */,
              reverseDirection, FeaturesPlugin_Placement::REVERSE_ID(),
              ModelAPI_AttributeBoolean, /** Reverse direction flag */,
              centering, FeaturesPlugin_Placement::CENTERING_ID(),
              ModelAPI_AttributeBoolean, /** Centering flag */)

  /// Set objects.
  FEATURESAPI_EXPORT
  void setObjects(const std::list<ModelHighAPI_Selection>& theObjects);

  /// Set start shape.
  FEATURESAPI_EXPORT
  void setStartShape(const ModelHighAPI_Selection& theStartShape);

  /// Set end shape.
  FEATURESAPI_EXPORT
  void setEndShape(const ModelHighAPI_Selection& theEndShape);

  /// Set reverse direction flag.
  FEATURESAPI_EXPORT
  void setReverseDirection(const bool theReverseDirection);

  /// Set centering flag.
  FEATURESAPI_EXPORT
  void setCentering(const bool theCentering);

  /// Dump wrapped feature
  FEATURESAPI_EXPORT
  virtual void dump(ModelHighAPI_Dumper& theDumper) const;
};

/// Pointer on Placement object.
typedef std::shared_ptr<FeaturesAPI_Placement> PlacementPtr;

/// \ingroup CPPHighAPI
/// \brief Create Placement feature.
FEATURESAPI_EXPORT
PlacementPtr addPlacement(const std::shared_ptr<ModelAPI_Document>& thePart,
                          const std::list<ModelHighAPI_Selection>& theObjects,
                          const ModelHighAPI_Selection& theStartShape,
                          const ModelHighAPI_Selection& theEndShape,
                          const bool theReverseDirection = false,
                          const bool theCentering = false);

#endif // FeaturesAPI_Placement_H_
