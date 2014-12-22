// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

#ifndef TEMPLATEPLUGIN_H
#define TEMPLATEPLUGIN_H

#if defined TEMPLATEPLUGIN_EXPORTS
#if defined WIN32
#define TEMPLATEPLUGIN_EXPORT              __declspec( dllexport )
#else
#define TEMPLATEPLUGIN_EXPORT
#endif
#else
#if defined WIN32
#define TEMPLATEPLUGIN_EXPORT              __declspec( dllimport )
#else
#define TEMPLATEPLUGIN_EXPORT
#endif
#endif

#endif
