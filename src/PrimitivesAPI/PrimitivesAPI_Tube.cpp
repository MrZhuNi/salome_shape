// Copyright (C) 2014-2019  CEA/DEN, EDF R&D
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

#include "PrimitivesAPI_Tube.h"

#include <ModelHighAPI_Dumper.h>
#include <ModelHighAPI_Tools.h>

//==================================================================================================
PrimitivesAPI_Tube::PrimitivesAPI_Tube(const std::shared_ptr<ModelAPI_Feature>& theFeature)
: ModelHighAPI_Interface(theFeature)
{
}

//==================================================================================================
PrimitivesAPI_Tube::PrimitivesAPI_Tube(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                       const ModelHighAPI_Double& theRMin,
                                       const ModelHighAPI_Double& theRMax,
                                       const ModelHighAPI_Double& theZ)
: ModelHighAPI_Interface(theFeature)
{
  if (initialize()) {
    fillAttribute(PrimitivesPlugin_Tube::METHODE_SIMPLE_TUBE_ID(), creationMethod());
    fillAttribute(theRMin, rmin());
    fillAttribute(theRMax, rmax());
    fillAttribute(theZ, z());

    execute();
  }
}

//==================================================================================================
PrimitivesAPI_Tube::PrimitivesAPI_Tube(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                       const ModelHighAPI_Double& theRMin,
                                       const ModelHighAPI_Double& theRMax,
                                       const ModelHighAPI_Double& theZ,
                                       const ModelHighAPI_Double& theStartPhi,
                                       const ModelHighAPI_Double& theDeltaPhi)
: ModelHighAPI_Interface(theFeature)
{
  if (initialize()) {
    fillAttribute(PrimitivesPlugin_Tube::METHODE_SEGMENT_TUBE_ID(), creationMethod());
    fillAttribute(theRMin, rmin());
    fillAttribute(theRMax, rmax());
    fillAttribute(theZ, z());
    fillAttribute(theStartPhi, startphi());
    fillAttribute(theDeltaPhi, deltaphi());

    execute();
  }
}

//==================================================================================================
PrimitivesAPI_Tube::~PrimitivesAPI_Tube()
{
}

//==================================================================================================
void PrimitivesAPI_Tube::dump(ModelHighAPI_Dumper& theDumper) const
{
  FeaturePtr aBase = feature();
  const std::string& aDocName = theDumper.name(aBase->document());

  theDumper << aBase << " = model.addTube(" << aDocName;

  std::string aCreationMethod = aBase->string(PrimitivesPlugin_Tube::CREATION_METHOD())->value();
  
  if(aCreationMethod == PrimitivesPlugin_Tube::METHODE_SIMPLE_TUBE_ID()) {
    AttributeDoublePtr anAttrRMin = aBase->real(PrimitivesPlugin_Tube::RMIN_ID());
    AttributeDoublePtr anAttrRMax = aBase->real(PrimitivesPlugin_Tube::RMAX_ID());
    AttributeDoublePtr anAttrZ = aBase->real(PrimitivesPlugin_Tube::Z_ID());
    theDumper << ", " << anAttrRMin << ", " << anAttrRMax << ", " << anAttrZ;
  } else if (aCreationMethod == PrimitivesPlugin_Tube::METHODE_SEGMENT_TUBE_ID()) {
    AttributeDoublePtr anAttrRMin = aBase->real(PrimitivesPlugin_Tube::RMIN_ID());
    AttributeDoublePtr anAttrRMax = aBase->real(PrimitivesPlugin_Tube::RMAX_ID());
    AttributeDoublePtr anAttrZ = aBase->real(PrimitivesPlugin_Tube::Z_ID());
    AttributeDoublePtr anAttrStartPhi = aBase->real(PrimitivesPlugin_Tube::START_PHI_ID());
    AttributeDoublePtr anAttrDeltaPhi = aBase->real(PrimitivesPlugin_Tube::DELTA_PHI_ID());
    theDumper << ", " << anAttrRMin << ", " << anAttrRMax << ", " << anAttrZ;
    theDumper << ", " << anAttrStartPhi << ", " << anAttrDeltaPhi;
  }

  theDumper << ")" << std::endl;
}

//==================================================================================================
TubePtr addTube(const std::shared_ptr<ModelAPI_Document>& thePart,
                const ModelHighAPI_Double& theRMin,
                const ModelHighAPI_Double& theRMax,
                const ModelHighAPI_Double& theZ)
{
  std::shared_ptr<ModelAPI_Feature> aFeature = thePart->addFeature(PrimitivesAPI_Tube::ID());
  return TubePtr(new PrimitivesAPI_Tube(aFeature, theRMin, theRMax, theZ));
}

//==================================================================================================
TubePtr addTube(const std::shared_ptr<ModelAPI_Document>& thePart,
                const ModelHighAPI_Double& theRMin,
                const ModelHighAPI_Double& theRMax,
                const ModelHighAPI_Double& theZ,
                const ModelHighAPI_Double& theStartPhi,
                const ModelHighAPI_Double& theDeltaPhi)
{
  std::shared_ptr<ModelAPI_Feature> aFeature = thePart->addFeature(PrimitivesAPI_Tube::ID());
  return TubePtr(new PrimitivesAPI_Tube(aFeature, theRMin, theRMax, theZ,
                                        theStartPhi, theDeltaPhi));
}
