// Name   : ModelHighAPI_Double.h
// Purpose: 
//
// History:
// 29/03/16 - Sergey POKHODENKO - Creation of the file

#ifndef SRC_MODELHIGHAPI_MODELHIGHAPI_DOUBLE_H_
#define SRC_MODELHIGHAPI_MODELHIGHAPI_DOUBLE_H_

//--------------------------------------------------------------------------------------
#include "ModelHighAPI.h"

#include <memory>
#include <string>

#include <boost/variant.hpp>
//--------------------------------------------------------------------------------------
class ModelAPI_AttributeDouble;
//--------------------------------------------------------------------------------------
/**\class ModelHighAPI_Double
 * \ingroup CPPHighAPI
 * \brief Class for filling ModelAPI_AttributeDouble
 */
class ModelHighAPI_Double
{
public:
  /// Constructor for double
  MODELHIGHAPI_EXPORT ModelHighAPI_Double(double theValue = 0.);
  /// Constructor for std::string
  MODELHIGHAPI_EXPORT ModelHighAPI_Double(const std::string & theValue);
  /// Constructor for char *
  MODELHIGHAPI_EXPORT ModelHighAPI_Double(const char * theValue);
  /// Destructor
  MODELHIGHAPI_EXPORT virtual ~ModelHighAPI_Double();

  /// Fill attribute values
  MODELHIGHAPI_EXPORT virtual void fillAttribute(std::shared_ptr<ModelAPI_AttributeDouble> & theAttribute) const;

private:
  boost::variant<double, std::string> myValue;
};

//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------
#endif /* SRC_MODELHIGHAPI_MODELHIGHAPI_DOUBLE_H_ */ 
