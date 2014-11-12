include(testapplication.pri)

TRANSLATIONS = ut_mdesktopentry.ts

lrelease.input = TRANSLATIONS
lrelease.output = ${QMAKE_FILE_BASE}.qm
lrelease.commands = lrelease -idbased ${QMAKE_FILE_IN} -qm ${QMAKE_FILE_BASE}.qm
lrelease.CONFIG = target_predeps no_link
QMAKE_EXTRA_COMPILERS += lrelease

translations.CONFIG = no_check_exist
translations.files = $$replace(TRANSLATIONS, ".ts", ".qm")
translations.path = $${INSTALL_TESTDIR}
INSTALLS += translations
