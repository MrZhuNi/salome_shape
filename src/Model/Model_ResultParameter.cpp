// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        Model_ResultParameter.cpp
// Created:     02 Apr 2015
// Author:      Sergey BELASH

#include "Model_ResultParameter.h"
#include <ModelAPI_AttributeDouble.h>
#include <ModelAPI_AttributeString.h>

Model_ResultParameter::~Model_ResultParameter()
{

}

void Model_ResultParameter::initAttributes()
{
  data()->addAttribute(ModelAPI_ResultParameter::VALUE(),
                       ModelAPI_AttributeDouble::typeId());
}

Model_ResultParameter::Model_ResultParameter()
{
  myIsDisabled = true; // by default it is not initialized and false to be after created
  setIsConcealed(false);
}
