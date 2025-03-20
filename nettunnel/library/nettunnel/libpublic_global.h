#ifndef LIBPUBLIC_GLOBAL_H
#define LIBPUBLIC_GLOBAL_H

#include <QtCore/qglobal.h>

#ifdef QT_NO_DEBUG
#define LIBPUBLIC_EXPORT
#else
#if defined(LIBPUBLIC_LIBRARY)
#define LIBPUBLIC_EXPORT Q_DECL_EXPORT
#else
#define LIBPUBLIC_EXPORT Q_DECL_IMPORT
#endif
#endif

#endif // LIBPUBLIC_GLOBAL_H
