/***************************************************************************
** This file was derived from the MDesktopEntry implementation in the
** libmeegotouch library.
**
** Original Copyright:
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
**
** Copyright on new work:
** Copyright 2011 Intel Corp.
**
** This library is free software; you can redistribute it and/or
** modify it under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation
** and appearing in the file LICENSE.LGPL included in the packaging
** of this file.
**
****************************************************************************/

// This has to be the first include otherwise gdbusintrospection.h causes an error.
#include <dconf.h>

#include <QCoreApplication>
#include <QString>
#include <QStringList>
#include <QByteArray>
#include <QVariant>
#include <QDebug>

#include "mgconfitem.h"
#include "mdconf_p.h"
#include "logging.h"

struct MGConfItemPrivate : public QObject
{
    MGConfItemPrivate(QString aKey, MGConfItem* aParent);
    ~MGConfItemPrivate();

    void customEvent(QEvent* event);
    static QByteArray convertKey(const QString &key);
    static void changed(DConfClient *, gchar *, GStrv, gchar *, gpointer);

    QString key;
    QVariant value;
    DConfClient *client;
    gulong handler;
    QByteArray dconf_key;
};

MGConfItemPrivate::MGConfItemPrivate(QString aKey, MGConfItem* aParent)
    : QObject(aParent)
    , key(aKey)
    , client(dconf_client_new())
    , handler(g_signal_connect(client, "changed", G_CALLBACK(changed), this))
    , dconf_key(convertKey(aKey))
{
    dconf_client_watch_fast(client, dconf_key);
}

MGConfItemPrivate::~MGConfItemPrivate()
{
    g_signal_handler_disconnect(client, handler);
    dconf_client_unwatch_fast(client, dconf_key);
    g_object_unref(client);
}

QByteArray MGConfItemPrivate::convertKey(const QString &key)
{
    if (key.startsWith('/')) {
        return key.toUtf8();
    } else {
        QString replaced = key;
        replaced.replace('.', '/');
        qCWarning(lcMlite) << "Using dot-separated key names with MGConfItem is deprecated.";
        qCWarning(lcMlite) << "Please use" << '/' + replaced << "instead of" << key;
        return '/' + replaced.toUtf8();
    }
}

void MGConfItemPrivate::changed(DConfClient*, gchar *prefix, GStrv changes, gchar*, gpointer data)
{
    MDConf::Event event(prefix, changes);
    QCoreApplication::sendEvent((MGConfItemPrivate*)data, &event);
}

void MGConfItemPrivate::customEvent(QEvent* event)
{
    if (event->type() == (QEvent::Type)MDConf::Event::TYPE) {
        ((MGConfItem*)parent())->update_value(true);
    }
}

void MGConfItem::update_value(bool emit_signal)
{
    QVariant new_value;
    GVariant *v = dconf_client_read(priv->client, priv->dconf_key);
    if (!v) {
        new_value = priv->value;
    }

    new_value = MDConf::convertValue(v);
    if (v)
        g_variant_unref(v);

    if (new_value != priv->value
            || new_value.userType() != priv->value.userType()
            || (new_value.type() == QVariant::Double
                && priv->value.type() == QVariant::Double
                // The "!=" equality check may fail for doubles depending on precision, check again
                && !qFuzzyCompare(new_value.toDouble(), priv->value.toDouble()))) {
        priv->value = std::move(new_value);
        if (emit_signal)
            emit valueChanged();
    }
}

QString MGConfItem::key() const
{
    return priv->key;
}

QVariant MGConfItem::value() const
{
    return priv->value;
}

QVariant MGConfItem::value(const QVariant &def) const
{
    if (priv->value.isNull())
        return def;
    else
        return priv->value;
}

void MGConfItem::set(const QVariant &val)
{
    GVariant *v = NULL;

    if (MDConf::convertValue(val, &v)) {
        GError *error = NULL;
        dconf_client_write_fast(priv->client, priv->dconf_key, v, &error);

        if (error) {
            qCWarning(lcMlite) << error->message;
            g_error_free(error);
        }

        // We will not emit any signals because dconf should take care of that for us.
    } else {
        qCWarning(lcMlite) << "Can't store a" << val.typeName();
    }
}

void MGConfItem::unset()
{
    set(QVariant());
}

QStringList MGConfItem::listDirs() const
{
    QStringList children;
    gint length = 0;
    QByteArray k = priv->dconf_key;
    if (!k.endsWith("/")) {
        k.append("/");
    }

    gchar **dirs = dconf_client_list(priv->client, k.data(), &length);
    GError *error = NULL;

    for (gint x = 0; x < length; x++) {
        const gchar *dir = g_strdup_printf ("%s%s", k.data(), dirs[x]);
        if (dconf_is_dir(dir, &error)) {
            // We have to mimic how gconf was behaving.
            // so we need to chop off trailing slashes.
            // dconf will also barf if it gets a "path" with 2 slashes.
            QString d = QString::fromUtf8(dir);
            if (d.endsWith("/")) {
                d.chop(1);
            }

            children.append(d);
        }

        g_free((gpointer)dir);

        // If we have error set then dconf_is_dir() has returned false so we should be safe here
        if (error) {
            qCWarning(lcMlite) << "MGConfItem" << error->message;
            g_error_free(error);
            error = NULL;
        }
    }

    g_strfreev(dirs);
    return children;
}

bool MGConfItem::sync()
{
    dconf_client_sync(priv->client);
    return true;
}

MGConfItem::MGConfItem(const QString &key, QObject *parent)
    : QObject(parent)
    , priv(new MGConfItemPrivate(key, this))
{
    update_value(false);
}

MGConfItem::~MGConfItem()
{
}
