TEMPLATE = subdirs
SUBDIRS = \
        src \
        tests \
        tools

tests.depends = src
tools.depends = src

# Adds 'coverage' make target
include(coverage.pri)
