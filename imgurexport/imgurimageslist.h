/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2010-02-04
 * Description : a tool to export images to imgur.com
 *
 * Copyright (C) 2010-2012 by Marius Orcsik <marius at habarnam dot ro>
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

#ifndef IMGURIMAGESLIST_H
#define IMGURIMAGESLIST_H

// Qt includes

#include <QWidget>

// LibKIPI includes

#include <libkipi/interface.h>

// Local includes

#include "kpimageslist.h"

using namespace KIPI;
using namespace KIPIPlugins;

namespace KIPIImgurExportPlugin
{

class ImgurImagesList : public KPImagesList
{
    Q_OBJECT

public:
    /* The different columns in a list. */
    enum FieldType
    {
        URL = KPImagesListView::User1,
        DELETEURL = KPImagesListView::User2
    };

    ImgurImagesList(Interface* const iface, QWidget* const parent = 0);
    ~ImgurImagesList();

public:

    // implement this, if you have special item widgets, e.g. an edit line
    // they will be set automatically when adding items, changing order, etc.
    virtual void updateItemWidgets();

public Q_SLOTS:
    virtual void slotAddImages(const KUrl::List& list);
    virtual void slotImageChanged (const KUrl& imageUrl, ImgurSuccess success);
};

// -------------------------------------------------------------------------

class ImgurImageListViewItem : public KPImagesListViewItem
{

public:

    ImgurImageListViewItem(KPImagesListView* const view, const KUrl& url);
    virtual ~ImgurImageListViewItem();

    void setUrl(const QString& str);
    QString Url() const;

    void setDeleteUrl(const QString& str);
    QString deleteUrl() const;

//    QString destPath() const;

private:

    QString m_Url;
    QString m_deleteUrl;
};

} // namespace KIPIImgurTalkerPlugin

#endif // IMGURIMAGESLIST_H
