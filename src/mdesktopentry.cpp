/***************************************************************************
** This file was derived from the MDesktopEntry implementation in the
** libmeegotouch library.
**
** Copyright (C) 2010, 2011 Nokia Corporation and/or its subsidiary(-ies).
** Copyright (C) 2011 Intel Corp.
** Copyright (C) 2012 - 2016 Jolla Ltd.
** Copyright (C) 2020 - 2021 Open Mobile Platform LLC.
**
** This library is free software; you can redistribute it and/or
** modify it under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation
** and appearing in the file LICENSE.LGPL included in the packaging
** of this file.
**
****************************************************************************/

#include <QRegExp>
#include <QFile>
#include <QStringList>
#include <QCoreApplication>
#include <QTranslator>
#include <QTextStream>
#include <QTextCodec>
#include <QDebug>

#include "mdesktopentry.h"
#include "mdesktopentry_p.h"

const QString DesktopEntrySection("Desktop Entry");
const QString TypeKey("Type");
const QString VersionKey("Version");
const QString NameKey("Name");
const QString GenericNameKey("GenericName");
const QString NoDisplayKey("NoDisplay");
const QString CommentKey("Comment");
const QString IconKey("Icon");
const QString HiddenKey("Hidden");
const QString OnlyShowInKey("OnlyShowIn");
const QString NotShowInKey("NotShowIn");
const QString TryExecKey("TryExec");
const QString ExecKey("Exec");
const QString PathKey("Path");
const QString TerminalKey("Terminal");
const QString MimeTypeKey("MimeType");
const QString CategoriesKey("Categories");
const QString StartupNotifyKey("StartupNotify");
const QString StartupWMClassKey("StartupWMClass");
const QString URLKey("URL");
const QString LogicalIdKey("X-MeeGo-Logical-Id");
const QString TranslationCatalogKey("X-MeeGo-Translation-Catalog");
const QString XMaemoServiceKey("X-Maemo-Service");
const QString SailjailSection("X-Sailjail");
const QString SandboxingKey("Sandboxing");
const QString ActionsKey("Actions");
const QString DisabledValue("Disabled");


GKeyFileWrapper::GKeyFileWrapper()
    : m_key_file(g_key_file_new())
{
}

GKeyFileWrapper::~GKeyFileWrapper()
{
    g_key_file_free(m_key_file);
}

bool GKeyFileWrapper::load(QIODevice &device)
{
    QByteArray contents = device.readAll();

    GError *err = NULL;
    if (!g_key_file_load_from_data(m_key_file, contents.constData(), contents.size(), G_KEY_FILE_NONE, &err)) {
        qWarning() << "Could not load .desktop file:" << QString::fromUtf8(err->message);
        g_clear_error(&err);
        return false;
    }

    return true;
}

QString GKeyFileWrapper::startGroup() const
{
    QString result;

    gchar *section = g_key_file_get_start_group(m_key_file);
    result = QString::fromUtf8(section);
    g_free(section);

    return result;
}

QStringList GKeyFileWrapper::sections() const
{
    QStringList result;

    gchar **sections = g_key_file_get_groups(m_key_file, NULL);
    gchar **section = sections;
    while (*section) {
        result << QString::fromUtf8(*section++);
    }
    g_strfreev(sections);

    return result;
}

QStringList GKeyFileWrapper::keys(const QString &section) const
{
    QStringList result;
    QByteArray section_utf8 = section.toUtf8();

    GError *err = NULL;

    gchar **keys = g_key_file_get_keys(m_key_file, section_utf8.constData(), NULL, &err);
    if (keys == NULL) {
        qWarning() << "Could not get keys:" << QString::fromUtf8(err->message);
        g_clear_error(&err);
        return result;
    }

    gchar **key = keys;
    while (*key) {
        result << QString::fromUtf8(*key++);
    }
    g_strfreev(keys);

    return result;
}

QString GKeyFileWrapper::localizedValue(const QString &section, const QString &key) const
{
    QString result;

    QByteArray section_utf8 = section.toUtf8();
    QByteArray key_utf8 = key.toUtf8();

    GError *err = NULL;
    gchar *value = g_key_file_get_locale_string(m_key_file, section_utf8.constData(), key_utf8.constData(), NULL, &err);
    if (!value) {
        qWarning() << "Could not read value:" << QString::fromUtf8(err->message);
        g_clear_error(&err);
        return result;
    }

    result = QString::fromUtf8(value);
    g_free(value);

    return result;
}

QString GKeyFileWrapper::stringValue(const QString &section, const QString &key) const
{
    QString result;

    QByteArray section_utf8 = section.toUtf8();
    QByteArray key_utf8 = key.toUtf8();

    GError *err = NULL;
    gchar *value = g_key_file_get_string(m_key_file, section_utf8.constData(), key_utf8.constData(), &err);
    if (!value) {
        qWarning() << "Could not read value:" << QString::fromUtf8(err->message);
        g_clear_error(&err);
        return result;
    }

    result = QString::fromUtf8(value);
    g_free(value);

    return result;
}

bool GKeyFileWrapper::booleanValue(const QString &section, const QString &key) const
{
    gboolean result = FALSE;

    QByteArray section_utf8 = section.toUtf8();
    QByteArray key_utf8 = key.toUtf8();

    GError *err = NULL;
    result = g_key_file_get_boolean(m_key_file, section_utf8.constData(), key_utf8.constData(), &err);
    if (err != NULL) {
        qWarning() << "Could not read boolean value for "
                   << section << "/" << key << ":" << QString::fromUtf8(err->message);
        g_clear_error(&err);
    }

    return result;
}

QStringList GKeyFileWrapper::stringList(const QString &section, const QString &key) const
{
    QStringList result;

    QByteArray section_utf8 = section.toUtf8();
    QByteArray key_utf8 = key.toUtf8();

    gchar **values = g_key_file_get_string_list(m_key_file, section_utf8.constData(), key_utf8.constData(), NULL, NULL);
    gchar **value = values;
    while (value && *value) {
        result << QString::fromUtf8(*value);
        value++;
    }
    g_strfreev(values);

    return result;
}

bool GKeyFileWrapper::contains(const QString &section, const QString &key) const
{
    QByteArray section_utf8 = section.toUtf8();
    QByteArray key_utf8 = key.toUtf8();
    return g_key_file_has_key(m_key_file, section_utf8, key_utf8, NULL);
}

bool GKeyFileWrapper::hasSection(const QString &section) const
{
    QByteArray section_utf8 = section.toUtf8();
    return g_key_file_has_group(m_key_file, section_utf8);
}


MDesktopEntryPrivate::MDesktopEntryPrivate(const QString &fileName)
    : sourceFileName(fileName)
    , keyFile()
    , valid(true)
    , q_ptr(NULL)
{
    QFile file(fileName);

    //Checks if the file exists and opens it in readonly mode
    if (file.exists() && file.open(QIODevice::ReadOnly)) {
        valid = keyFile.load(file);

        // File is invalid if it doesn't start with DesktopEntrySection
        if (keyFile.startGroup() != DesktopEntrySection) {
            valid = false;
        }
    } else {
        qDebug() << "Specified Desktop file does not exist" << fileName;
    }
}

MDesktopEntryPrivate::~MDesktopEntryPrivate()
{
}

QTranslator *MDesktopEntryPrivate::loadTranslator() const
{
    QTranslator *translator = new QTranslator;
    QString catalog = keyFile.stringValue(DesktopEntrySection, TranslationCatalogKey);
    if (catalog.isNull() || !translator->load(QLocale(), catalog, "-", "/usr/share/translations")) {
        qDebug() << "Unable to load catalog" << catalog;
        delete translator;
        translator = 0;
    }
    return translator;
}

/**
 * Unfortunately this is public API, so we need to provide the ::readDesktopFile() implementation
 * even though we don't use a QMap<QString, QString> internally anymore to represent the file.
 **/
bool MDesktopEntry::readDesktopFile(QIODevice &device, QMap<QString, QString> &desktopEntriesMap)
{
    GKeyFileWrapper f;

    if (!f.load(device)) {
        return false;
    }

    /**
     * From the spec: The basic format of the desktop entry file requires that
     * there be a group header named "Desktop Entry". [...] There should be
     * nothing preceding this group in the desktop entry file but possibly one
     * or more comments.
     **/
    if (f.startGroup() != DesktopEntrySection) {
        return false;
    }

    foreach (const QString &section, f.sections()) {
        foreach (const QString &key, f.keys(section)) {
            desktopEntriesMap[section + "/" + key] = f.stringValue(section, key);
        }
    }

    return true;
}

bool MDesktopEntryPrivate::boolValue(const QString &section, const QString &key) const
{
    if (!keyFile.contains(section, key)) {
        return false;
    }

    return keyFile.booleanValue(section, key);
}

QStringList MDesktopEntryPrivate::stringListValue(const QString &section, const QString &key) const
{
    return keyFile.stringList(section, key);
}

MDesktopEntry::MDesktopEntry(const QString &fileName) :
    d_ptr(new MDesktopEntryPrivate(fileName))
{
}

MDesktopEntry::MDesktopEntry(MDesktopEntryPrivate &dd) :
    d_ptr(&dd)
{
}

MDesktopEntry::~MDesktopEntry()
{
    delete d_ptr;
}

QString MDesktopEntry::fileName() const
{
    return d_ptr->sourceFileName;
}

bool MDesktopEntry::contains(const QString &key) const
{
    QStringList v = key.split('/');
    return (v.length() == 2) ? contains(v[0], v[1]) : false;
}

bool MDesktopEntry::contains(const QString &group, const QString &key) const
{
    return d_ptr->keyFile.contains(group, key);
}

QString MDesktopEntry::value(const QString &key) const
{
    QStringList v = key.split('/');
    return (v.length() == 2) ? value(v[0], v[1]) : QString();
}

QString MDesktopEntry::value(const QString &group, const QString &key) const
{
    if (!contains(group, key)) {
        return QString();
    }

    return d_ptr->keyFile.stringValue(group, key);
}

QStringList MDesktopEntry::stringListValue(const QString &key) const
{
    QStringList v = key.split('/');
    return (v.length() == 2) ? stringListValue(v[0], v[1]) : QStringList();
}

QStringList MDesktopEntry::stringListValue(const QString &group, const QString &key) const
{
    if (!contains(group, key)) {
        return QStringList();
    }

    return d_ptr->keyFile.stringList(group, key);
}

bool MDesktopEntry::isValid() const
{
    // The Type and Name keys always have to be present
    if (!contains(DesktopEntrySection, TypeKey)) {
        return false;
    }

    if (!contains(DesktopEntrySection, NameKey)) {
        return false;
    }

    // In case of an application either the Exec key needs to be present or D-Bus activation supported
    if (type() == QStringLiteral("Application") &&
            !contains(DesktopEntrySection, ExecKey) &&
            // MER#1557: Migrate to freedesktop.org DBusActivatable API
            !contains(DesktopEntrySection, XMaemoServiceKey)) {
        return false;
    }

    // In case of a link the URL key needs to be present
    if (type() == "Link" && !contains(DesktopEntrySection, URLKey)) {
        return false;
    }

    // In case the desktop entry is invalid for some explicit reason
    // Some cases are:
    // 1. Group name defined multiple times
    // 2. Desktop entry's first group should be "Desktop Entry"
    if (!d_ptr->valid) {
        return false;
    }
    return true;
}

uint MDesktopEntry::hash() const
{
    return qHash(type() + name());
}

QString MDesktopEntry::type() const
{
    // Type always has to be present (see isValid())
    return value(DesktopEntrySection, TypeKey);
}

QString MDesktopEntry::version() const
{
    if (!contains(DesktopEntrySection, VersionKey)) {
        return QString();
    }

    return value(DesktopEntrySection, VersionKey);
}

QString MDesktopEntry::name() const
{
    Q_D(const MDesktopEntry);

    if (!d->translatedName.isNull())
        return d->translatedName;

    QString name;

    if (contains(DesktopEntrySection, LogicalIdKey)) {
        QString key = value(DesktopEntrySection, LogicalIdKey);
        QString translation;
        QScopedPointer<QTranslator> translator(d->loadTranslator());
        if (translator) {
            translation = translator->translate(0, key.toLatin1().data(), 0, -1);
        } else {
            translation = qtTrId(key.toLatin1().data());
        }

        if (!translation.isEmpty() && translation != key) {
            name = translation;
        }
    }

    if (name.isEmpty()) {
        name = d->keyFile.localizedValue(DesktopEntrySection, NameKey);
    }

    d->translatedName = name;
    return name;
}

QString MDesktopEntry::nameUnlocalized() const
{
    // Name always has to be present (see isValid())
    return value(DesktopEntrySection, NameKey);
}

QString MDesktopEntry::genericName() const
{
    return value(DesktopEntrySection, GenericNameKey);
}

bool MDesktopEntry::noDisplay() const
{
    if (!contains(DesktopEntrySection, NoDisplayKey)) {
        return false;
    }

    return d_ptr->boolValue(DesktopEntrySection, NoDisplayKey);
}

QString MDesktopEntry::comment() const
{
    return value(DesktopEntrySection, CommentKey);
}

QString MDesktopEntry::icon() const
{
    return value(DesktopEntrySection, IconKey);
}

bool MDesktopEntry::hidden() const
{
    if (!contains(DesktopEntrySection, HiddenKey)) {
        return false;
    }

    return d_ptr->boolValue(DesktopEntrySection, HiddenKey);
}

QStringList MDesktopEntry::onlyShowIn() const
{
    if (!contains(DesktopEntrySection, OnlyShowInKey)) {
        return QStringList();
    }

    return d_ptr->stringListValue(DesktopEntrySection, OnlyShowInKey);
}

QStringList MDesktopEntry::notShowIn() const
{
    if (!contains(DesktopEntrySection, NotShowInKey)) {
        return QStringList();
    }

    return d_ptr->stringListValue(DesktopEntrySection, NotShowInKey);
}

QString MDesktopEntry::tryExec() const
{
    return value(DesktopEntrySection, TryExecKey);
}

QString MDesktopEntry::exec() const
{
    return value(DesktopEntrySection, ExecKey);
}

QString MDesktopEntry::xMaemoService() const
{
    return value(DesktopEntrySection, XMaemoServiceKey);
}

QString MDesktopEntry::path() const
{
    return value(DesktopEntrySection, PathKey);
}

bool MDesktopEntry::terminal() const
{
    if (!contains(DesktopEntrySection, TerminalKey)) {
        return false;
    }

    return d_ptr->boolValue(DesktopEntrySection, TerminalKey);
}

QStringList MDesktopEntry::mimeType() const
{
    if (!contains(DesktopEntrySection, MimeTypeKey)) {
        return QStringList();
    }

    return d_ptr->stringListValue(DesktopEntrySection, MimeTypeKey);
}

QStringList MDesktopEntry::categories() const
{
    if (!contains(DesktopEntrySection, CategoriesKey)) {
        return QStringList();
    }

    return d_ptr->stringListValue(DesktopEntrySection, CategoriesKey);
}

bool MDesktopEntry::startupNotify() const
{
    if (!contains(DesktopEntrySection, StartupNotifyKey)) {
        return false;
    }

    return d_ptr->boolValue(DesktopEntrySection, StartupNotifyKey);
}

QString MDesktopEntry::startupWMClass() const
{
    return value(DesktopEntrySection, StartupWMClassKey);
}

QString MDesktopEntry::url() const
{
    return value(DesktopEntrySection, URLKey);
}

bool MDesktopEntry::isSandboxed() const
{
    return d_ptr->keyFile.hasSection(SailjailSection) && value(SailjailSection, SandboxingKey) != DisabledValue;
}

QList<MDesktopAction> MDesktopEntry::actions() const
{
    QList<MDesktopAction> actionsList;
    QStringList actions = value(DesktopEntrySection, ActionsKey).split(";", Qt::SkipEmptyParts);
    for (int i = 0; i < actions.size(); ++i) {
        actionsList.append(MDesktopAction(d_ptr->sourceFileName, actions.at(i)));
    }

    return actionsList;
}
