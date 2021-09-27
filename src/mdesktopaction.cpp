/***************************************************************************
** This file was derived from the MDesktopAction implementation in the
** libmeegotouch library.
**
** Copyright (C) 2021 Chupligin Sergey (NeoChapay) <neochapay@gmail.com>
**
** This library is free software; you can redistribute it and/or
** modify it under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation
** and appearing in the file LICENSE.LGPL included in the packaging
** of this file.
**
****************************************************************************/

#include "mdesktopaction.h"
#include <QString>
#include <QIODevice>

#include "mdesktopentry_p.h"
#include "mdesktopaction_p.h"

MDesktopActionPrivate::MDesktopActionPrivate(const QString &fileName, const QString &action)
    :q_ptr(new MDesktopEntryPrivate(fileName))
{
    actionName = action;
}

MDesktopActionPrivate::~MDesktopActionPrivate()
{
}

bool MDesktopActionPrivate::valid()
{
    return q_ptr->valid;
}

QString MDesktopActionPrivate::action()
{
    return actionName;
}

QString MDesktopActionPrivate::name()
{
    QString localName = q_ptr->keyFile.localizedValue(actionEntryName(), "Name");
    if (localName.isEmpty()) {
        return nameUnlocalized();
    }
    return localName;
}

QString MDesktopActionPrivate::nameUnlocalized()
{
    return q_ptr->keyFile.stringValue(actionEntryName(), "Name");
}

QString MDesktopActionPrivate::exec()
{
    return q_ptr->keyFile.stringValue(actionEntryName(), "Exec");
}

QString MDesktopActionPrivate::icon()
{
    return q_ptr->keyFile.stringValue(actionEntryName(), "Icon");
}

QString MDesktopActionPrivate::actionEntryName()
{
    return "Desktop Action " + actionName;
}


MDesktopAction::MDesktopAction(const QString &fileName, const QString &action)
    : d_ptr(new MDesktopActionPrivate(fileName, action))
{
}

MDesktopAction::~MDesktopAction() {
}

QString MDesktopAction::name() const
{
    return d_ptr->name();
}

QString MDesktopAction::exec() const
{
    return d_ptr->exec();
}

QString MDesktopAction::icon() const
{
    return d_ptr->icon();
}

QString MDesktopAction::actionName() const
{
    return d_ptr->action();
}

QString MDesktopAction::nameUnlocalized() const
{
    return d_ptr->nameUnlocalized();
}
