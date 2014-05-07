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
extern "C" {
#include <dconf.h>
};

#include <QString>
#include <QStringList>
#include <QByteArray>
#include <QVariant>
#include <QDebug>

#include "mgconfitem.h"

struct MGConfItemPrivate {
    MGConfItemPrivate() :
        client(dconf_client_new()),
	handler(0)
    {}

    QString key;
    QVariant value;
    DConfClient *client;
    gulong handler;
    static void notify_trampoline(DConfClient *, gchar *, GStrv, gchar *, gpointer);
};

static QByteArray convertKey(const QString &key)
{
    if (key.startsWith('/'))
        return key.toUtf8();
    else {
        QString replaced = key;
        replaced.replace('.', '/');
        qWarning() << "Using dot-separated key names with MGConfItem is deprecated.";
        qWarning() << "Please use" << '/' + replaced << "instead of" << key;
        return '/' + replaced.toUtf8();
    }
}

static QString convertKey(const char *key)
{
    return QString::fromUtf8(key);
}

static QVariant convertValue(GVariant *src)
{
    if (!src) {
        return QVariant();
    } else {
        switch (g_variant_classify(src)) {
        case G_VARIANT_CLASS_BOOLEAN:
	    return QVariant((bool)g_variant_get_boolean(src));
        case G_VARIANT_CLASS_INT32:
            return QVariant(g_variant_get_int32(src));
        case G_VARIANT_CLASS_DOUBLE:
            return QVariant(g_variant_get_double(src));
        case G_VARIANT_CLASS_STRING:
	    return QVariant(QString::fromUtf8(g_variant_get_string(src, NULL)));
        case G_VARIANT_CLASS_ARRAY: {
	    if (g_variant_type_equal(g_variant_get_type(src), G_VARIANT_TYPE_STRING_ARRAY)) {
                QStringList result;
		for (uint x = 0; x < g_variant_n_children(src); x++) {
		    GVariant *child = g_variant_get_child_value(src, x);
		    result.append(QString::fromUtf8(g_variant_get_string(child, NULL)));
		    g_variant_unref(child);
		}
		return QVariant(result);
	    } else {
	        QList<QVariant> result;
		for (uint x = 0; x < g_variant_n_children(src); x++) {
		    GVariant *child = g_variant_get_child_value(src, x);
		    result.append(convertValue(child));
		    g_variant_unref(child);
		}
	        return QVariant(result);
	    }
	}

        default:
            return QVariant();
        }
    }
}

static const GVariantType *primitiveType(const QVariant &elt)
{
    switch (elt.type()) {
    case QVariant::String:
        return G_VARIANT_TYPE_STRING;
    case QVariant::Int:
        return G_VARIANT_TYPE_INT32;
    case QVariant::Double:
        return G_VARIANT_TYPE_DOUBLE;
    case QVariant::Bool:
        return G_VARIANT_TYPE_BOOLEAN;
    default:
        return NULL;
    }
}

static const GVariantType *uniformType(const QList<QVariant> &list)
{
    const GVariantType *result = NULL;

    foreach(const QVariant & elt, list) {
        const GVariantType *elt_type = primitiveType(elt);

        if (elt_type == NULL)
            return NULL;

        if (result == NULL)
            result = elt_type;
        else if (result != elt_type)
            return NULL;
    }

    if (result == NULL)
        return G_VARIANT_TYPE_ARRAY;  // empty list.
    else
        return result;
}

static bool convertValue(const QVariant &src, GVariant **valp)
{
    GVariant *v = NULL;

    switch (src.type()) {
    case QVariant::Invalid:
        v = NULL;
        break;
    case QVariant::Bool:
        v = g_variant_new_boolean(src.toBool());
        break;
    case QVariant::Int:
        v = g_variant_new_int32(src.toInt());
        break;
    case QVariant::Double:
        v = g_variant_new_double(src.toDouble());
        break;
    case QVariant::String:
        v = g_variant_new_string(src.toString().toUtf8().data());
        break;
    case QVariant::StringList: {
        if (src.toStringList().isEmpty()) {
	    v = g_variant_new_array(G_VARIANT_TYPE_STRING, NULL, 0);
	} else {
	    GVariantBuilder *builder = g_variant_builder_new(G_VARIANT_TYPE_ARRAY);
	    foreach (const QString & str, src.toStringList())
	        g_variant_builder_add_value(builder, g_variant_new_string(str.toUtf8().data()));
	    v = g_variant_builder_end(builder);
	    g_variant_builder_unref(builder);
	}
	break;
    }
    case QVariant::List: {
        const GVariantType *elt_type = uniformType(src.toList());

        if (elt_type == NULL)
            v = NULL;
        else {
	    GVariantBuilder *builder = g_variant_builder_new(G_VARIANT_TYPE_ARRAY);
            foreach(const QVariant & elt, src.toList()) {
                GVariant *val = NULL;
                convertValue(elt, &val);  // guaranteed to succeed.
	        g_variant_builder_add_value(builder, val);
            }

	    v = g_variant_builder_end(builder);
	    g_variant_builder_unref(builder);
        }
        break;
    }
    default:
        return false;
    }

    *valp = v;
    return true;
}

void MGConfItemPrivate::notify_trampoline(DConfClient *, gchar *, GStrv, gchar *, gpointer data)
{
    MGConfItem *item = (MGConfItem *)data;
    item->update_value(true);
}

void MGConfItem::update_value(bool emit_signal)
{
    QVariant new_value;
    QByteArray k = convertKey(priv->key);
    GVariant *v = dconf_client_read(priv->client, k.data());
    if (!v) {
        qWarning() << "MGConfItem Failed to read" << priv->key;
	new_value = priv->value;
    }

    new_value = convertValue(v);
    if (v)
        g_variant_unref(v);

    if (new_value != priv->value) {
        priv->value = new_value;
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
    QByteArray k = convertKey(priv->key);
    GVariant *v = NULL;
    GError *error = NULL;

    if (convertValue(val, &v)) {
	dconf_client_write_fast(priv->client, k.data(), v, &error);
	if (v) {
	    g_variant_unref(v);
	}

	if (error) {
	    qWarning() << error->message;
	    g_error_free(error);
	}

	// We will not emit any signals because dconf should take care of that for us.
    } else
        qWarning() << "Can't store a" << val.typeName();
}

void MGConfItem::unset()
{
    set(QVariant());
}

QStringList MGConfItem::listDirs() const
{
    QStringList children;
    gint length = 0;
    QByteArray k = convertKey(priv->key);
    gchar **dirs = dconf_client_list(priv->client, k.data(), &length);
    GError *error = NULL;

    for (gint x = 0; x < length; x++) {
      if (dconf_is_dir(dirs[x], &error)) {
	children.append(convertKey(dirs[x]));
      }

      // If we have error set then dconf_is_dir() has returned false so we should be safe here
      if (error) {
	qWarning() << "MGConfItem" << error->message;
	g_error_free(error);
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
{
    priv = new MGConfItemPrivate;
    priv->key = key;
    priv->handler =
      g_signal_connect(priv->client, "changed", G_CALLBACK(MGConfItemPrivate::notify_trampoline), this);

    QByteArray k = convertKey(priv->key);
    dconf_client_watch_fast(priv->client, k.data());
    update_value(false);
}

MGConfItem::~MGConfItem()
{
    g_signal_handler_disconnect(priv->client, priv->handler);
    QByteArray k = convertKey(priv->key);
    dconf_client_unwatch_fast(priv->client, k.data());
    g_object_unref(priv->client);
    delete priv;
}
