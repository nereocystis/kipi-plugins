/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2006-10-12
 * Description : IPTC date and time settings page.
 *
 * Copyright (C) 2006-2007 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * ============================================================ */

#ifndef IPTC_DATETIME_H
#define IPTC_DATETIME_H

// Qt includes.

#include <QWidget>
#include <QByteArray>

namespace KIPIMetadataEditPlugin
{

class IPTCDateTimePriv;

class IPTCDateTime : public QWidget
{
    Q_OBJECT
    
public:

    IPTCDateTime(QWidget* parent);
    ~IPTCDateTime();

    void applyMetadata(QByteArray& iptcData);
    void readMetadata(QByteArray& iptcData);

signals:

    void signalModified();

private slots:

    void slotSetTodayReleased();
    void slotSetTodayExpired();

private:

    IPTCDateTimePriv* d;
};

}  // namespace KIPIMetadataEditPlugin

#endif // IPTC_DATETIME_H 
