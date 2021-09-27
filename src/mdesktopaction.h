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

#ifndef MDESKTOPACTION_H
#define MDESKTOPACTION_H

#include <mlite-global.h>
#include <QString>

class MDesktopEntryPrivate;
class MDesktopActionPrivate;

class MLITESHARED_EXPORT MDesktopAction
{

public:
    MDesktopAction(const QString &fileName, const QString &action);
    virtual ~MDesktopAction();

    QString name() const;
    QString nameUnlocalized() const;
    QString exec() const;
    QString icon() const;
    QString actionName() const;

protected:
    /*! \internal */
    //! Pointer to the private class
    MDesktopActionPrivate *const d_ptr;
};

#endif // MDESKTOPACTION_H
