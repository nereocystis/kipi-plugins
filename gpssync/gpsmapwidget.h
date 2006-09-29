/* ============================================================
 * Authors: Caulier Gilles <caulier dot gilles at kdemail dot net>
 * Date   : 2006-09-28
 * Description : a widget to display a GPS web map locator.
 * 
 * Copyright 2006 by Gilles Caulier
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option)
 * any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#ifndef GPSMAPWIDGET_H
#define GPSMAPWIDGET_H

// Qt includes.

#include <qstring.h>

// KDE includes.

#include <khtml_part.h>

class QResizeEvent;

namespace KIPIGPSSyncPlugin
{

class GPSMapWidget : public KHTMLPart
{
    Q_OBJECT

public:

    GPSMapWidget(QWidget* parent, const QString& lat, const QString& lon, int zoomLevel=8);
    ~GPSMapWidget();

    void resized();

signals:

    void signalNewGPSLocationFromMap(const QString&, const QString&);

protected:

    void khtmlMouseReleaseEvent(khtml::MouseReleaseEvent *);

private:

    QString m_latitude;
    QString m_longitude;
    QString m_zoomLevel;
};

}  // namespace KIPIGPSSyncPlugin

#endif /* GPSMAPWIDGET_H */
