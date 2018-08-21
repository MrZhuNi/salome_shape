@ECHO OFF

REM Find absolute path to ROOT_DIR 
@SET PARENT_DIR=%~dp0..
@SET ROOT_DIR=
pushd %PARENT_DIR%\..
@SET ROOT_DIR=%CD%
popd

@SET SRC_DIR=%ROOT_DIR%\sources\shaper

call %SRC_DIR%\env_Salome.bat
mkdir %ROOT_DIR%\build
cd %ROOT_DIR%\build

REM Add -DADD_MODELS_TESTS=TRUE to enable test models
cmake %SRC_DIR% -G "Visual Studio 14 Win64" -DCMAKE_BUILD_TYPE=Release -DCMAKE_CONFIGURATION_TYPES=Release -DCMAKE_INSTALL_PREFIX:PATH=%ROOT_DIR%\install

start "" %MSVC_EXE% SHAPER.sln
