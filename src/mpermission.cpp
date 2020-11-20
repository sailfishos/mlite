/***************************************************************************
** Copyright (C) 2020 Open Mobile Platform LLC.
**
** This library is free software; you can redistribute it and/or
** modify it under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation
** and appearing in the file LICENSE.LGPL included in the packaging
** of this file.
**
****************************************************************************/

#include <QDebug>
#include <QFile>
#include <QTextStream>
#include <QTranslator>

#include "mdesktopentry.h"
#include "mpermission.h"
#include "mpermission_p.h"

namespace {
const auto PermissionFileDirectory = QStringLiteral("/etc/sailjail/permissions/");
const auto ProfileSuffix = QStringLiteral(".profile");
const auto Prefix = QStringLiteral("x-sailjail-");
const auto Description = QStringLiteral("description");
const auto TranslationKey = QStringLiteral("translation-key");
const auto TranslationCatalog = QStringLiteral("translation-catalog");
const auto TranslationDirectory = QStringLiteral("/usr/share/translations");
const auto TranslationSeparator = QStringLiteral("-");
const auto SailjailSection = QStringLiteral("X-Sailjail");
const auto SailjailPermissionsKey = QStringLiteral("Permissions");

QPair<QStringRef, QStringRef> getField(const QString &line)
{
    QStringRef remaining = line.midRef(0).trimmed();
    if (!remaining.startsWith('#'))
        return QPair<QStringRef, QStringRef>();

    remaining = remaining.mid(1).trimmed();
    if (!remaining.startsWith(Prefix))
        return QPair<QStringRef, QStringRef>();

    remaining = remaining.mid(Prefix.length());
    int separator = remaining.indexOf('=');
    if (separator == -1)
        return QPair<QStringRef, QStringRef>();

    QStringRef key = remaining.left(separator).trimmed();
    QStringRef value = remaining.mid(separator+1).trimmed();
    return QPair<QStringRef, QStringRef>(key, value);
}
} // namespace

QHash<QString, QTranslator *> MPermissionPrivate::s_translators;

MPermissionPrivate::MPermissionPrivate(const QString &fileName) :
    fileName(fileName)
{
    QFile file(fileName);
    if (!file.exists()) {
        qWarning() << "Permission file" << file.fileName() << "does not exist!";
        return;
    }

    if (!file.open(QFile::ReadOnly)) {
        qWarning() << "Permission file" << file.fileName() << "could not be opened!";
        return;
    }

    QTextStream stream(&file);
    QString description;
    QString key;
    QString catalog;
    while (!stream.atEnd() && (description.isEmpty() || key.isEmpty() || catalog.isEmpty())) {
        QString line = stream.readLine();
        QPair<QStringRef, QStringRef> field = getField(line);
        if (field.first.isEmpty()) {
            continue;
        } else if (field.first == Description) {
            description = field.second.toString();
        } else if (field.first == TranslationKey) {
            key = field.second.toString();
        } else if (field.first == TranslationCatalog) {
            catalog = field.second.toString();
        }
    }

    if (description.isEmpty() || key.isEmpty() || catalog.isEmpty()) {
        qWarning() << "Permission file" << file.fileName() << "is missing a required field.";
    } else {
        fallbackDescription = description;
        translationKey = key;
        translationCatalog = catalog;
    }
}

MPermissionPrivate::MPermissionPrivate(const MPermissionPrivate &other) :
    fileName(other.fileName),
    fallbackDescription(other.fallbackDescription),
    translationCatalog(other.translationCatalog),
    translationKey(other.translationKey)
{
}

MPermissionPrivate::~MPermissionPrivate()
{
}

QTranslator *MPermissionPrivate::translator() const
{
    if (!s_translators.contains(translationCatalog)) {
        QTranslator *translator = new QTranslator;
        if (translator->load(QLocale(), translationCatalog, TranslationSeparator, TranslationDirectory)) {
            qWarning() << "Failed to load translation catalog" << translationCatalog;
            delete translator;
            translator = 0; // Mark as failed
        }
        s_translators.insert(translationCatalog, translator);
    }
    return s_translators.value(translationCatalog);
}

MPermission::MPermission(const QString &fileName) :
    d_ptr(new MPermissionPrivate(fileName))
{
}

MPermission::MPermission(const MPermission &other) :
    d_ptr(new MPermissionPrivate(*other.d_ptr))
{
}

MPermission::~MPermission()
{
    delete d_ptr;
}

QList<MPermission> MPermission::fromDesktopEntry(const MDesktopEntry &entry)
{
    QList<MPermission> permissions;
    for (QString &name : entry.stringListValue(SailjailSection, SailjailPermissionsKey)) {
        name = name.trimmed();
        if (name.startsWith('!') || name.startsWith('?'))
            name = name.remove(0, 1).trimmed();
        if (!name.endsWith(ProfileSuffix))
            name.append(ProfileSuffix);
        MPermission permission(PermissionFileDirectory + name);
        if (permission.isValid())
            permissions.append(permission);
    }
    return permissions;
}

QString MPermission::name() const
{
    int first = d_ptr->fileName.lastIndexOf("/")+1;
    int last = d_ptr->fileName.lastIndexOf(".");
    return d_ptr->fileName.mid(first, last-first);
}

bool MPermission::isValid() const
{
    return !d_ptr->fallbackDescription.isEmpty();
}

QString MPermission::description() const
{
    if (d_ptr->translationCatalog.isEmpty() || d_ptr->translationKey.isEmpty())
        return d_ptr->fallbackDescription;

    QString description;
    QTranslator *translator = d_ptr->translator();
    if (translator)
        description = translator->translate(0, d_ptr->translationKey.toUtf8().constData(), 0, -1);
    return description.isEmpty() ? d_ptr->fallbackDescription : description;
}

QString MPermission::descriptionUnlocalized() const
{
    return d_ptr->fallbackDescription;
}
