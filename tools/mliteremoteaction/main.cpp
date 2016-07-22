/***************************************************************************
**
** Copyright (C) 2016 Jolla Ltd.
**
** This file is part of mlite.
**
** This library is free software; you can redistribute it and/or
** modify it under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation
** and appearing in the file LICENSE.LGPL included in the packaging
** of this file.
**
****************************************************************************/

#include <MRemoteAction>

#include <QCoreApplication>
#include <QDBusConnection>
#include <QDBusMessage>
#include <QDBusReply>
#include <QDebug>

int main(int argc, char *argv[])
{
    QCoreApplication application(argc, argv);

    const QStringList arguments = application.arguments();

    MRemoteAction action(arguments.value(1));

    if (!action.isValid()) {
        fprintf(stderr, "Usage: /usr/libexec/mliteremoteaction \"<service> <path> <interface> <method> [<arguments>]\"");
        return 1;
    }

    QDBusMessage message = QDBusMessage::createMethodCall(
                action.serviceName(), action.objectPath(), action.interface(), action.methodName());
    message.setArguments(action.arguments());

    QDBusMessage reply = QDBusConnection::sessionBus().call(message);

    if (reply.type() == QDBusMessage::ErrorMessage) {
        fprintf(stderr, "%s", qPrintable(reply.errorMessage()));

        return 1;
    }

    return 0;
}
