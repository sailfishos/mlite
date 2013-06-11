isEmpty(COMMON_PRI_INCLUDED) {
COMMON_PRI_INCLUDED = 1

# Use the following variables to differentiate build against Qt 4.x vs Qt 5.x
# Example:
#       TARGET = foo$${DASH_QT_VERSION}
#       LIBS += -lbar$${DASH_QT_VERSION}
equals(QT_MAJOR_VERSION, 4): DASH_QT_VERSION = ""
equals(QT_MAJOR_VERSION, 5): DASH_QT_VERSION = "-qt5"
equals(QT_MAJOR_VERSION, 4): NODASH_QT_VERSION = ""
equals(QT_MAJOR_VERSION, 5): NODASH_QT_VERSION = "5"

}
