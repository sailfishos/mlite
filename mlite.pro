QT     = gui core dbus
TARGET = $$qtLibraryTarget(mlite)
TEMPLATE = lib

CONFIG += link_pkgconfig
PKGCONFIG += gconf-2.0

DEFINES += MLITE_LIBRARY

OBJECTS_DIR = .obj
MOC_DIR = .moc

SOURCES += \
    mgconfitem.cpp \
    mdesktopentry.cpp \
    mremoteaction.cpp \
    maction.cpp \
    mfiledatastore.cpp \
    mnotification.cpp \
    mnotificationgroup.cpp \
    mnotificationmanager.cpp \
    mnotificationmanagerproxy.cpp

HEADERS += \
    mdesktopentry_p.h \
    mdesktopentry.h \
    mgconfitem.h \
    mlite-global.h \
    mremoteaction.h \
    mremoteaction_p.h \
    maction.h \
    maction_p.h \
    MAction \
    mfiledatastore.h \
    mfiledatastore_p.h \
    mdataaccess.h \
    mdatastore.h \
    mnotification.h \
    mnotification_p.h \
    mnotificationgroup.h \
    mnotificationgroup_p.h \
    mnotificationmanager.h \
    mnotificationmanagerproxy.h \
    MNotification \
    MGConfItem \
    MNotificationGroup

INSTALL_HEADERS += \
    mgconfitem.h \
    mdesktopentry.h \
    mremoteaction.h \
    maction.h \
    mlite-global.h \
    MAction \
    mfiledatastore.h \
    mnotification.h \
    mnotificationgroup.h \
    MNotification \
    MGConfItem \
    MNotificationGroup

pcfiles.files += mlite.pc
pcfiles.path += $$INSTALL_ROOT/usr/lib/pkgconfig

headers.files += $$INSTALL_HEADERS
headers.path += $$INSTALL_ROOT/usr/include/mlite

target.path += $$[QT_INSTALL_LIBS]

INSTALLS += target headers pcfiles

TRANSLATIONS += $${SOURCES} $${HEADERS} $${OTHER_FILES}
VERSION = 0.0.5
PROJECT_NAME = mlite

dist.commands += rm -fR $${PROJECT_NAME}-$${VERSION} &&
dist.commands += git clone . $${PROJECT_NAME}-$${VERSION} &&
dist.commands += rm -fR $${PROJECT_NAME}-$${VERSION}/.git &&
dist.commands += tar jcpvf $${PROJECT_NAME}-$${VERSION}.tar.bz2 $${PROJECT_NAME}-$${VERSION}
QMAKE_EXTRA_TARGETS += dist
