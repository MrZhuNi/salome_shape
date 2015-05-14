// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        SketchPlugin_Validators.cpp
// Created:     01 Aug 2014
// Author:      Vitaly SMETANNIKOV

#include <ExchangePlugin_Validators.h>
#include <ModelAPI_Feature.h>
#include <ModelAPI_Object.h>
#include <ModelAPI_Session.h>
#include <ModelAPI_AttributeString.h>

#include <list>
#include <string>
#include <sstream>
#include <algorithm>

bool ExchangePlugin_FormatValidator::parseFormats(const std::list<std::string>& theArguments,
                                                  std::list<std::string>& outFormats)
{
  std::list<std::string>::const_iterator it = theArguments.begin();
  bool result = true;
  for (; it != theArguments.end(); ++it) {
    std::string anArg = *it;
    int aSepPos = anArg.find(":");
    if (aSepPos == std::string::npos) {
      result = false;
      continue;
    }
    std::string aFormatList = anArg.substr(0, aSepPos);
    std::transform(aFormatList.begin(), aFormatList.end(), aFormatList.begin(), toupper);
    std::istringstream aStream(aFormatList);
    std::string aFormat;
    while (std::getline(aStream, aFormat, '|'))
      outFormats.push_back(aFormat);
  }
  return result;
}

bool ExchangePlugin_FormatValidator::isValid(const AttributePtr& theAttribute,
                                             const std::list<std::string>& theArguments) const
{
  if (!theAttribute->isInitialized())
    return false;

  const AttributeStringPtr aStrAttr =
      std::dynamic_pointer_cast<ModelAPI_AttributeString>(theAttribute);
  if (!aStrAttr)
    return false;

  std::string aFileName = aStrAttr->value();
  if (aFileName.empty())
    return false;

  std::list<std::string> aFormats;
  ExchangePlugin_FormatValidator::parseFormats(theArguments, aFormats);
  std::list<std::string>::const_iterator itFormats = aFormats.begin();
  size_t aFileNameLen = aFileName.length();
  std::transform(aFileName.begin(), aFileName.end(), aFileName.begin(), toupper);
  // Is file name ends with the format
  for (; itFormats != aFormats.end(); ++itFormats) {
    size_t aFormatBeginPos = aFileNameLen - (*itFormats).length();
    if (aFileName.compare(aFormatBeginPos, std::string::npos, *itFormats) == 0) {
      return true;
    }
  }
  return false;
}
