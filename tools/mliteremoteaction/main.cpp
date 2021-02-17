/***************************************************************************
**
** Copyright (C) 2021 Open Mobile Platform LLC.
** Copyright (C) 2016 - 2019 Jolla Ltd.
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

#include <unistd.h>

int RemoteActionTimeout = 120 * 1000; // D-Bus activation timeout

int main(int argc, char *argv[])
{
    const gid_t gid = getgid();
    const uid_t uid = getuid();

    if (setregid(gid, gid) < 0) {
        fprintf(stderr, "Could not setegid to actual group\n");
        return EXIT_FAILURE;
    } else if (setreuid(uid, uid) < 0) {
        fprintf(stderr, "Could not seteuid to actual user\n");
        return EXIT_FAILURE;
    }

    QCoreApplication application(argc, argv);

    const QStringList arguments = application.arguments();

    MRemoteAction action(arguments.value(1));

    if (!action.isValid()) {
        fprintf(stderr, "Usage: /usr/libexec/mliteremoteaction \"<service> <path> <interface> <method> [<arguments>]\"\n");
        return 1;
    }

    QDBusMessage message = QDBusMessage::createMethodCall(
                action.serviceName(), action.objectPath(), action.interface(), action.methodName());
    message.setArguments(action.arguments());

    QDBusMessage reply = QDBusConnection::sessionBus().call(message, QDBus::Block, RemoteActionTimeout);

    if (reply.type() == QDBusMessage::ErrorMessage) {
        fprintf(stderr, "Remote action failed: %s\n", qPrintable(reply.errorMessage()));

        return 1;
    }

    return 0;
}
