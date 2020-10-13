// Copyright (C) 2014-2020  CEA/DEN, EDF R&D
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

#ifndef SRC_BUILDPLUGIN_EVALLISTENER_H_
#define SRC_BUILDPLUGIN_EVALLISTENER_H_

#include <BuildPlugin.h>
#include <Events_Loop.h>

class ModelAPI_Attribute;
class ModelAPI_Document;
class ModelAPI_Feature;
class ModelAPI_ResultParameter;
class BuildPlugin_PyInterp;

/**
 * \class BuildPlugin_EvalListener
 * \ingroup Plugins
 * \brief Class which process the events from the event loop.
 */
class BuildPlugin_EvalListener : public Events_Listener
{
 public:
  BUILDPLUGIN_EXPORT BuildPlugin_EvalListener();
  BUILDPLUGIN_EXPORT virtual ~BuildPlugin_EvalListener();

  /// Reimplemented from Events_Listener::processEvent().
  BUILDPLUGIN_EXPORT
    virtual void processEvent(const std::shared_ptr<Events_Message>& theMessage);

  // performs the python call to initialize high level data model on internal data model creation
  void initDataModel();

 protected:
  /// Evaluates theExpression and returns its value.
  double evaluate(std::wstring& theVariable,
                  double theValueVariable,std::shared_ptr<ModelAPI_Feature> theParameter,
                  const std::wstring& theExpression, std::string& theError,
                  std::list<std::shared_ptr<ModelAPI_ResultParameter> >& theParamsList,
                  const bool theIsParameter = false);

  double evaluate(std::shared_ptr<ModelAPI_Feature> theParameter,
                  const std::wstring& theExpression, std::string& theError,
                  std::list<std::shared_ptr<ModelAPI_ResultParameter> >& theParamsList,
                  const bool theIsParameter = false);

  /// Processes Evaluation event.
  void processEvaluationEvent(const std::shared_ptr<Events_Message>& theMessage);

 private:
  std::shared_ptr<BuildPlugin_PyInterp> myInterp;
};

#endif /* SRC_BUILDPLUGIN_EVALLISTENER_H_ */
