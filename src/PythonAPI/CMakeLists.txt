## Copyright (C) 2014-20xx  OPEN CASCADE


SET(CMAKE_AUTOMOC ON)

# configuration
CONFIGURE_FILE(
  "${CMAKE_CURRENT_SOURCE_DIR}/Test/config.py.in"
  "${CMAKE_BINARY_DIR}/config.py"
  )

INSTALL(DIRECTORY extension geom model examples plugins DESTINATION ${PYTHONAPI_DIR})
INSTALL(FILES __init__.py DESTINATION ${PYTHONAPI_DIR})

# --------- Unit tests -----------
INCLUDE(UnitTest)

ADD_UNIT_TESTS(
  TestPythonAPI.py
  TestModel.py

  TestSketcherAddPoint.py
  TestSketcherAddLine.py
  TestSketcherAddArc.py
  TestSketcherAddCircle.py
  TestSketcherAddMirror.py
  TestSketcherSetCoincident.py
  TestSketcherSetTangent.py
  TestSketcherSetParallel.py
  TestSketcherSetPerpendicular.py
  TestSketcherSetHorizontal.py
  TestSketcherSetVertical.py
  TestSketcherSetRigid.py
  TestSketcherSetLength.py
  TestSketcherSetRadius.py
  TestSketcherSetAngle.py
  TestSketcherSetEqual.py
  TestSketcherSetFillet.py

  TestFeatures.py
  TestFeaturesExtrusion.py
  TestFeaturesRevolution.py

  TestMakeBrick1.py
  TestMakeBrick2.py
  TestMakeBrick3.py
  TestPlatine.py
  )
