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

#ifndef MPERMISSION_P_H
#define MPERMISSION_P_H

#include <QHash>
#include <QString>

class QTranslator;

class MPermissionPrivate
{
public:
    MPermissionPrivate(const QString &fileName);
    MPermissionPrivate(const MPermissionPrivate &other);

    virtual ~MPermissionPrivate();

    QTranslator *translator() const;

    QString fileName;
    QString fallbackDescription;
    QString fallbackLongDescription;
    QString translationCatalog;
    QString descriptionTranslationKey;
    QString longDescriptionTranslationKey;

    static QHash<QString, QTranslator *> s_translators;
};

#endif /* MPERMISSION_P_H */
