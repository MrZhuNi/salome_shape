/* ConstructionAPI.i */
%module ConstructionAPI
%{
  #include "ConstructionAPI_swig.h"
%}

// import other modules
%import "ModelHighAPI.i"

// to avoid error on this
#define CONSTRUCTIONAPI_EXPORT

// standard definitions
%include "typemaps.i"
%include "std_shared_ptr.i"

// shared pointers
%shared_ptr(ConstructionAPI_Point)

// all supported interfaces
%include "ConstructionAPI_Point.h"
