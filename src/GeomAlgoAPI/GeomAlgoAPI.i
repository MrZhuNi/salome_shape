/* GeomAPI.i */
%module GeomAlgoAPI
%{
  #include "GeomAlgoAPI.h"
  #include "GeomAlgoAPI_Boolean.h"
  #include "GeomAlgoAPI_CompoundBuilder.h"
  #include "GeomAlgoAPI_DFLoader.h"
  #include "GeomAlgoAPI_EdgeBuilder.h"
  #include "GeomAlgoAPI_Extrusion.h"
  #include "GeomAlgoAPI_FaceBuilder.h"
  #include "GeomAlgoAPI_MakeShape.h"
  #include "GeomAlgoAPI_MakeShapeList.h"
  #include "GeomAlgoAPI_Movement.h"
  #include "GeomAlgoAPI_Placement.h"
  #include "GeomAlgoAPI_PointBuilder.h"
  #include "GeomAlgoAPI_Prism.h"
  #include "GeomAlgoAPI_Revolution.h"
  #include "GeomAlgoAPI_Rotation.h"
  #include "GeomAlgoAPI_ShapeProps.h"
  #include "GeomAlgoAPI_SketchBuilder.h"
  #include "GeomAlgoAPI_BREPExport.h"
  #include "GeomAlgoAPI_IGESExport.h"
  #include "GeomAlgoAPI_STEPExport.h"
  #include "GeomAlgoAPI_BREPImport.h"
  #include "GeomAlgoAPI_IGESImport.h"
  #include "GeomAlgoAPI_STEPImport.h"
  #include "GeomAlgoAPI_Tools.h"

  #include <memory>
  #include <string>
  #include <list>
%}

// to avoid error on this
#define GEOMALGOAPI_EXPORT

// standard definitions
%include "typemaps.i"
%include "std_string.i"
%include "std_list.i"
%include "std_shared_ptr.i"

// all supported interfaces
%include "GeomAlgoAPI_Boolean.h"
%include "GeomAlgoAPI_CompoundBuilder.h"
%include "GeomAlgoAPI_DFLoader.h"
%include "GeomAlgoAPI_EdgeBuilder.h"
%include "GeomAlgoAPI_Extrusion.h"
%include "GeomAlgoAPI_FaceBuilder.h"
%include "GeomAlgoAPI_MakeShape.h"
%include "GeomAlgoAPI_MakeShapeList.h"
%include "GeomAlgoAPI_Movement.h"
%include "GeomAlgoAPI_Placement.h"
%include "GeomAlgoAPI_PointBuilder.h"
%include "GeomAlgoAPI_Prism.h"
%include "GeomAlgoAPI_Revolution.h"
%include "GeomAlgoAPI_Rotation.h"
%include "GeomAlgoAPI_ShapeProps.h"
%include "GeomAlgoAPI_SketchBuilder.h"
%include "GeomAlgoAPI_BREPExport.h"
%include "GeomAlgoAPI_IGESExport.h"
%include "GeomAlgoAPI_STEPExport.h"
%include "GeomAlgoAPI_BREPImport.h"
%include "GeomAlgoAPI_IGESImport.h"
%include "GeomAlgoAPI_STEPImport.h"
%include "GeomAlgoAPI_Tools.h"

%typemap(out) std::list< std::shared_ptr< GeomAPI_Shape > >::value_type & {
  $result = SWIG_NewPointerObj(SWIG_as_voidptr(new std::shared_ptr<GeomAPI_Shape>(*$1)), $descriptor(std::shared_ptr<GeomAPI_Shape> *), SWIG_POINTER_OWN | 0 );
}
%template(ShapeList) std::list<std::shared_ptr<GeomAPI_Shape> >;