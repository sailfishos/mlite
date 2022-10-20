isEmpty(COMMON_PRI_INCLUDED) {
COMMON_PRI_INCLUDED = 1

# Creates Makefile target to substitute all occurencies of @VARIABLE@ in the given file.
# To substitute a VARIABLE, add it to the CONFIG_SUBST list.
defineTest(configure) {
    in_file = $$1
    target_file = $$basename(in_file)
    target_file ~= s/\\.in$//
    target = configure_$${target_file}
    target ~= s/[^a-zA-Z0-9]/_/g

    eval($${target}.target = $${target_file})
    eval($${target}.depends = $${in_file} $(MAKEFILE_LIST))
    eval($${target}.commands = sed)
    for(v, CONFIG_SUBST) {
        eval($${target}.commands += '-e \\\"s!@$${v}@!\$${$${v}}!g\\\"')
    }
    eval($${target}.commands += '<$${in_file} >$${target_file} || { rm -f $${target_file}; exit 1; }')
    QMAKE_EXTRA_TARGETS += $${target}

    equals(TEMPLATE, subdirs) {
        first = make_first
    } else {
        first = all
    }

    eval($${first}.depends += $${target})
    eval($${first}.CONFIG = phony)
    QMAKE_EXTRA_TARGETS += $${first}

    export($${target}.target)
    export($${target}.depends)
    export($${target}.commands)
    export($${first}.depends)
    export($${first}.CONFIG)
    export(QMAKE_EXTRA_TARGETS)
}

DASH_QT_VERSION = "-qt$${QT_MAJOR_VERSION}"
CONFIG_SUBST += DASH_QT_VERSION
NODASH_QT_VERSION = $${QT_MAJOR_VERSION}
CONFIG_SUBST += NODASH_QT_VERSION

}
