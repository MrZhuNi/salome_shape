@echo off

IF NOT EXIST "%VS100COMNTOOLS%..\..\VC\vcvarsall.bat" GOTO ERROR1

@REM Load local settings from file localenv.bat that should be located in the root directory
if "%ROOT_DIR%" == "" (
  set ROOT_DIR=%~dp0..
)

if exist "%ROOT_DIR%\localenv.bat" (
  echo Setting local environment from %ROOT_DIR%\localenv.bat
  call "%ROOT_DIR%\localenv.bat"
) else (
  echo Warning: file %ROOT_DIR%\localenv.bat does not exist; using default environment
)

if "%SALOME_ROOT_DIR%" == "" (
  set SALOME_ROOT_DIR=%ROOT_DIR%\SALOME
)

if "%SOLVESPACE_ROOT_DIR%" == "" (
  SET SOLVESPACE_ROOT_DIR=%ROOT_DIR%\products\solvespace-2.0
)
SET PATH=%SOLVESPACE_ROOT_DIR%\lib;%PATH%

cd %SALOME_ROOT_DIR%\WORK
call set_env.bat %1
call "%PDIR%\env_compile.bat"
cd %ROOT_DIR%

@REM -------------------------
@REM Python
@SET PYTHON_LIB_DIR=%PYTHON_ROOT_DIR%\libs
@SET PYTHON_INC_DIR=%PYTHON_ROOT_DIR%\include
@REM -------------------------

@REM -------------------------
@REM CASCADE
@SET PATH=%CASROOT%;%CASROOT%\win32\bin%OCC_LIB_PREFIX%;%PATH%
@SET LIB=%CASROOT%\win32\lib%OCC_LIB_PREFIX%;%LIB%
@set CSF_MDTVFontDirectory=%CASROOT%\src\FontMFT
@set CSF_LANGUAGE=us
@set MMGT_CLEAR=1
@set CSF_EXCEPTION_PROMPT=1
@set CSF_SHMessage=%CASROOT%\src\SHMessage
@set CSF_MDTVTexturesDirectory=%CASROOT%\src\Textures
@set CSF_XSMessage=%CASROOT%\src\XSMessage
@set CSF_StandardDefaults=%CASROOT%\src\StdResource
@set CSF_PluginDefaults=%CASROOT%\src\StdResource
@set CSF_XCAFDefaults=%CASROOT%\src\StdResource
@set CSF_StandardLiteDefaults=%CASROOT%\src\StdResource
@set CSF_UnitsLexicon=%CASROOT%\src\UnitsAPI\Lexi_Expr.dat
@set CSF_UnitsDefinition=%CASROOT%\src\UnitsAPI\Units.dat
@set CSF_IGESDefaults=%CASROOT%\src\XSTEPResource
@set CSF_STEPDefaults=%CASROOT%\src\XSTEPResource
@set CSF_XmlOcafResource=%CASROOT%\src\XmlOcafResource
@REM -------------------------

@REM -------------------------
@SET PYTHONHOME=%PYTHON_ROOT_DIR%
@SET PYTHON_INCLUDE=%PYTHONHOME%\include
@REM -------------------------


@REM -------------------------
@REM Some OCCT headers include FREETYPE headers
@SET FREETYPE_DIR=%PDIR%\freetype-2.4.11
@REM -------------------------

@SET PATH=%GEOM_ROOT_DIR%\lib\salome;%PATH%

@SET _NO_DEBUG_HEAP=1

IF "%ARCH%" == "Win64" (
  call "%VS100COMNTOOLS%..\..\VC\vcvarsall.bat" x64
) ELSE (
  IF "%ARCH%" == "Win32" (
    call "%VS100COMNTOOLS%..\..\VC\vcvarsall.bat" x86
  ) ELSE (
    echo Wrong architecture is used. Win32 or Win64 architecture is allowed only.
    echo Refer to the set_env.bat script.
  )
)

@SET NEW_GEOM_CONFIG_FILE=%ROOT_DIR%\install\plugins
@SET NEWGEOM_ROOT_DIR=%ROOT_DIR%\install
@SET PATH=%NEWGEOM_ROOT_DIR%\swig;%NEWGEOM_ROOT_DIR%\plugins;%NEWGEOM_ROOT_DIR%\bin;%PATH%
@SET PYTHONPATH=%NEWGEOM_ROOT_DIR%\swig;%NEWGEOM_ROOT_DIR%\plugins;%NEWGEOM_ROOT_DIR%\addons;%NEWGEOM_ROOT_DIR%\pythonAPI;%PYTHONPATH%

@SET LightAppConfig=%%NEWGEOM_ROOT_DIR%\share\salome\resources\newgeom;%GUI_ROOT_DIR%\share\salome\resources\gui
@SET NewGeomResources=%NEWGEOM_ROOT_DIR%\resources
