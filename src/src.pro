include(../common.pri)

isEmpty(VERSION) {
    GIT_TAG = $$system(git describe --tags --abbrev=0)
    GIT_VERSION = $$find(GIT_TAG, ^\\d+(\\.\\d+)?(\\.\\d+)?$)
    isEmpty(GIT_VERSION) {
        # if you're trying to build this from a tarball, I'm sorry. but being
        # able to specify the version in just one place (git tags) is a lot less
        # error-prone and easy.
        warning("Can't find a valid git tag version, got: $$GIT_TAG")
        GIT_VERSION = 0.0.0
    }
    !isEmpty(GIT_VERSION): VERSION = $$GIT_VERSION
}
CONFIG_SUBST += VERSION

QT     = core dbus
TARGET = $$qtLibraryTarget(mlite$${NODASH_QT_VERSION})
TEMPLATE = lib

CONFIG += link_pkgconfig
PKGCONFIG += glib-2.0

packagesExist(dconf) {
    PKGCONFIG += dconf
    DEFINES += HAVE_DCONF
    HEADERS += mdconfitem.h \
               MDConfItem \
               mdconf_p.h \
               mdconfgroup.h \
               MDConfGroup \
               mgconfitem.h \
               MGConfItem
    SOURCES += mdconfitem.cpp \
               mdconf.cpp \
               mdconfgroup.cpp \
               mgconfitem.cpp

    INSTALL_HEADERS += mdconfitem.h \
                       MDConfItem \
                       mdconfgroup.h \
                       MDConfGroup \
                       mgconfitem.h \
                       MGConfItem

} else {
    warning("dconf not found; MDConfItem will not be built")
}

QDBUSXML2CPP = $$[QT_INSTALL_BINS]/qdbusxml2cpp

system($$QDBUSXML2CPP notificationmanager.xml -p mnotificationmanagerproxy -c MNotificationManagerProxy -i metatypedeclarations.h)

SOURCES += mnotificationmanagerproxy.cpp \
           mnotification.cpp \
           mnotificationgroup.cpp \
           mremoteaction.cpp \
           mdesktopentry.cpp \
           mpermission.cpp \
           mfiledatastore.cpp \
           logging.cpp

HEADERS += mnotificationmanagerproxy.h \
           mnotification.h \
           mnotification_p.h \
           mnotificationgroup.h \
           mnotificationgroup_p.h \
           MNotification \
           MNotificationGroup \
           mremoteaction.h \
           mremoteaction_p.h \
           mdesktopentry_p.h \
           mpermission_p.h \
           mlite-global.h \
           mfiledatastore_p.h \
           mdataaccess.h \
           mdatastore.h \
           logging.h

INSTALL_HEADERS += mnotification.h \
                   mnotificationgroup.h \
                   mremoteaction.h \
                   MNotification \
                   MNotificationGroup \
                   MRemoteAction \
                   mdesktopentry.h \
                   mpermission.h \
                   mlite-global.h \
                   mfiledatastore.h \
                   MDesktopEntry \
                   MPermission

HEADERS *= $$INSTALL_HEADERS

DEFINES += MLITE_LIBRARY

PCFILE=mlite$${NODASH_QT_VERSION}.pc
configure($${PWD}/$${PCFILE}.in)
pcfiles.files = $$OUT_PWD/$$PCFILE
pcfiles.CONFIG = no_check_exist
pcfiles.path += $$INSTALL_ROOT/$$[QT_INSTALL_LIBS]/pkgconfig

headers.files += $$INSTALL_HEADERS
headers.path += /usr/include/mlite$${NODASH_QT_VERSION}

target.path += $$[QT_INSTALL_LIBS]

INSTALLS += target headers pcfiles
