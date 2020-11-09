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

#ifndef MPERMISSION_H_
#define MPERMISSION_H_

#include <mlite-global.h>

class MDesktopEntry;
class MPermissionPrivate;

/*!
 * MPermission is subject to change in future versions.
 *
 * \internal
 */

class MLITESHARED_EXPORT MPermission
{
public:
    /*!
     * Reads profile file for the permission and constructs a new
     * MPermission instance.
     *
     * \param file name of the profile file to read.
     */
    MPermission(const QString &fileName);

    /*!
     * Copy constructor.
     *
     * \param instance to copy.
     */
    MPermission(const MPermission &other);

    /*!
     * Destroys MPermission instance.
     */
    ~MPermission();

    /*!
     * Constructs a list of MPermission instances from profiles named
     * by MDesktopEntry.
     *
     * \param MDesktopEntry to read permissions from.
     */
    static QList<MPermission> fromDesktopEntry(const MDesktopEntry &entry);

    /*!
     * Returns name for the permission.
     */
    QString name() const;

    /*!
     * Returns whether the permission is valid and contains the needed
     * information.
     */
    bool isValid() const;

    /*!
     * Returns description for the permission. Localized if available,
     * otherwise the same as descriptionUnlocalized.
     */
    QString description() const;

    /*!
     * Returns unlocalized description for the permission. Empty string
     * if the permission is not valid.
     */
    QString descriptionUnlocalized() const;

protected:
    /*! \internal */
    //! Pointer to the private class
    MPermissionPrivate *const d_ptr;
    /*! \internal_end */

private:
    MPermission &operator=(const MPermission &);
    Q_DECLARE_PRIVATE(MPermission);
};

#endif /* MPERMISSION_H_ */
