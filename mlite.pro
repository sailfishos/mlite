QT     = core dbus
TARGET = $$qtLibraryTarget(mlite)
TEMPLATE = lib

DEFINES += MLITE_LIBRARY

CONFIG += link_pkgconfig
packagesExist(gconf-2.0) {
    PKGCONFIG += gconf-2.0
    DEFINES += HAVE_GCONF
    HEADERS += mgconfitem.h \
               MGConfItem
    SOURCES += mgconfitem.cpp

    INSTALL_HEADERS += mgconfitem.h \
                       MGConfItem

} else {
    warning("gconf-2.0 not found; MGConfItem will not be built")
}

equals(QT_MAJOR_VERSION, 4) {
    QT += gui
    SOURCES += maction.cpp \
               mremoteaction.cpp \
    mnotification.cpp \
    mnotificationgroup.cpp \
    mnotificationmanager.cpp \
    mnotificationmanagerproxy.cpp

    HEADERS += maction.h \
               maction_p.h \
               MAction \
               mremoteaction.h \
               mremoteaction_p.h \
               mnotification.h \
               mnotification_p.h \
               mnotificationgroup.h \
               mnotificationgroup_p.h \
               mnotificationmanager.h \
               mnotificationmanagerproxy.h \
               MNotification \
               MNotificationGroup \

    INSTALL_HEADERS += mremoteaction.h \
                       MAction \
                       mnotification.h \
                       mnotificationgroup.h \
                       MNotification \
                       MNotificationGroup \
                       maction.h

}

OBJECTS_DIR = .obj
MOC_DIR = .moc

SOURCES += \
    mdesktopentry.cpp \
    mfiledatastore.cpp

HEADERS += \
    mdesktopentry_p.h \
    mdesktopentry.h \
    mlite-global.h \
    mfiledatastore.h \
    mfiledatastore_p.h \
    mdataaccess.h \
    mdatastore.h \
    MDesktopEntry

# TODO: sanitize based on conditional builds
INSTALL_HEADERS += \
    mdesktopentry.h \
    mlite-global.h \
    mfiledatastore.h \
    MDesktopEntry

pcfiles.files += mlite.pc
pcfiles.path += $$INSTALL_ROOT/$$[QT_INSTALL_LIBS]/pkgconfig

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
