// Copyright (C) 2018-2020  CEA/DEN, EDF R&D
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

#include "FeaturesAPI_SharedFaces.h"

#include <FeaturesPlugin_SharedFaces.h>
#include <ModelAPI_AttributeDoubleArray.h>
#include <ModelHighAPI_Services.h>
#include <ModelHighAPI_Tools.h>

#include <ModelHighAPI_Dumper.h>
#include <ModelHighAPI_Selection.h>
#include <ModelHighAPI_Tools.h>


FeaturesAPI_SharedFaces::
          FeaturesAPI_SharedFaces(const std::shared_ptr<ModelAPI_Feature>& theFeature)
  : ModelHighAPI_Interface(theFeature)
{
  initialize();
}

FeaturesAPI_SharedFaces::FeaturesAPI_SharedFaces(
                                    const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                    const ModelHighAPI_Selection& theobject,
                                    const double theTransparency,
                                    const std::string & theNameGroup)
:ModelHighAPI_Interface(theFeature)
{
  if (initialize()) {
    fillAttribute(theobject, myobjectselected);
    fillAttribute(theTransparency, mytransparency);
    if( theNameGroup != "" )
    {
      fillAttribute(true,
                feature()->boolean(FeaturesPlugin_SharedFaces::CREATE_GROUP_ID()));
      fillAttribute(theNameGroup, mygroupname);
    }
    execute();
  }
}


FeaturesAPI_SharedFaces::~FeaturesAPI_SharedFaces()
{
}

void FeaturesAPI_SharedFaces::dump(ModelHighAPI_Dumper& theDumper) const
{
  FeaturePtr aBase = feature();
  const std::string& aDocName = theDumper.name(aBase->document());

  AttributeSelectionPtr anAttrObject;
    anAttrObject = aBase->selection(FeaturesPlugin_SharedFaces::OBJECT_ID());

  theDumper << aBase << " = model.getSharedFaces(" << aDocName << ", " << anAttrObject;
  theDumper << ", " << aBase->integer(FeaturesPlugin_SharedFaces::TRANSPARENCY_ID());

  if(aBase->boolean(FeaturesPlugin_SharedFaces::CREATE_GROUP_ID())->value() )
    theDumper << ", " << aBase->string(FeaturesPlugin_SharedFaces::GROUP_NAME_ID());

  theDumper << ")" << std::endl;
}

SharedFacesPtr getSharedFaces(const std::shared_ptr<ModelAPI_Document>& thePart,
                              const ModelHighAPI_Selection& theobject,
                              const double theTransparency,
                              const std::string & theNameGroup)
{

  FeaturePtr aFeature = thePart->addFeature(FeaturesPlugin_SharedFaces::ID());

  SharedFacesPtr aSharedFaces;

  aSharedFaces.reset(new FeaturesAPI_SharedFaces(aFeature,
                                                 theobject,
                                                 theTransparency,
                                                 theNameGroup));

  return aSharedFaces;
}

