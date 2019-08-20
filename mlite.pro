TEMPLATE = subdirs
SUBDIRS = \
        src \
        tests \
        tools

tests.depends = src
tools.depends = src

OTHER_FILES += rpm/mlite-qt5.spec

# Adds 'coverage' make target
include(coverage.pri)
