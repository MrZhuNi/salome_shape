// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:    GEOMALGOAPI_STEPExport.h
// Created: May 14, 2015
// Author:  Sergey POKHODENKO

#ifndef GEOMALGOAPI_STEPEXPORT_H_
#define GEOMALGOAPI_STEPEXPORT_H_

#include <GeomAlgoAPI.h>

#include <string>

#include <TopoDS_Shape.hxx>

/// Implementation of the export STEP files algorithms
GEOMALGOAPI_EXPORT
bool STEPExport(const std::string& theFileName,
                const std::string& theFormatName,
                const TopoDS_Shape& theShape,
                std::string& theError);

#endif /* GEOMALGOAPI_STEPEXPORT_H_ */
