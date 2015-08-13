/***************************************************************************
** Copyright (C) 2014 Jolla Mobile <andrew.den.exter@jollamobile.com>
**
** This library is free software; you can redistribute it and/or
** modify it under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation
** and appearing in the file LICENSE.LGPL included in the packaging
** of this file.
**
****************************************************************************/

#include <dconf/dconf.h>

#include <QVariant>
#include <QEvent>

namespace MDConf
{
    class Event : public QEvent
    {
    public:
        enum { TYPE = QEvent::User };

        Event(const char *aPrefix, GStrv aChanges) : QEvent((Type)TYPE),
            prefix(aPrefix), changes(aChanges) {}

        const char *prefix;
        GStrv changes;
    };


QVariant convertValue(GVariant *value, int typeHint = QMetaType::UnknownType);
bool convertValue(const QVariant &variant, GVariant **valp);

QVariant read(DConfClient *client, const QByteArray &key, int typeHint = QMetaType::UnknownType);

void write(DConfClient *client, const QByteArray &key, const QVariant &value, bool synchronous = false);

void clear(DConfClient *client, const QByteArray &key, bool synchronous = false);

void watch(DConfClient *client, const QByteArray &key, bool synchronous = false);
void unwatch(DConfClient *client, const QByteArray &key, bool synchronous = false);

void sync(DConfClient *client);

DConfClient *client();

}
