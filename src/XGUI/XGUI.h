// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

#ifndef XGUI_H
#define XGUI_H

//#define BEFORE_TRIHEDRON_PATCH

#if defined XGUI_EXPORTS
#if defined WIN32
#define XGUI_EXPORT              __declspec( dllexport )
#else
#define XGUI_EXPORT
#endif
#else
#if defined WIN32
#define XGUI_EXPORT              __declspec( dllimport )
#else
#define XGUI_EXPORT
#endif
#endif

#endif
