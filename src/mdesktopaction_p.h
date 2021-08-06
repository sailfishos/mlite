/***************************************************************************
** This file was derived from the MDesktopEntry implementation in the
** libmeegotouch library.
**
** Copyright (C) 2021 Chupligin Sergey <neochapay@gmail.com>
**
** This library is free software; you can redistribute it and/or
** modify it under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation
** and appearing in the file LICENSE.LGPL included in the packaging
** of this file.
**
****************************************************************************/

#ifndef MDESKTOPACTION_P_H
#define MDESKTOPACTION_P_H

#include <QObject>
#include "mdesktopentry_p.h"

class MDesktopActionPrivate
{
public:
    MDesktopActionPrivate(const QString &fileName, const QString &action);
    virtual ~MDesktopActionPrivate();

    bool valid();
    QString action();
    QString name();
    QString nameUnlocalized();
    QString exec();
    QString icon();
    QString actionEntryName();

protected:
    /*! \internal */
    //! Pointer to the private class
    MDesktopEntryPrivate *const q_ptr;

private:
    QString actionName;
};

#endif // MDESKTOPACTION_P_H
