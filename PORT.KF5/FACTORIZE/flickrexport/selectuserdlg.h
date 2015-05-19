/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2005-16-05
 * Description : a kipi plugin to export images to Flickr web service
 *
 * Copyright (C) 2015 by Shourya Singh Gupta <shouryasgupta at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#ifndef SELECTUSERDLG_H
#define SELECTUSERDLG_H

// Qt includes

#include <QHash>
#include <QList>
#include <QPair>
#include <QLabel>

// KDE includes

// LibKIPI includes

#include <libkipi/interface.h>

// Local includes

#include "kptooldialog.h"
#include "comboboxintermediate.h"

class QProgressDialog;
class QPushButton;
class QSpinBox;
class QCheckBox;

class KLineEdit;
class KHTMLPart;
class KUrl;
class KComboBox;

namespace KIPIPlugins
{
    class KPAboutData;
}

using namespace KIPI;
using namespace KIPIPlugins;

namespace KIPIFlickrExportPlugin
{
class SelectUserDlg : public KPToolDialog
{
    Q_OBJECT
public:
    SelectUserDlg(QWidget* const parent,const QString& serviceName);
    ~SelectUserDlg();
    void reactivate();
    QString getUname();
    SelectUserDlg* getDlg();
    
private:
    KComboBox*   userComboBox;
    QLabel*      label;
    QString      m_uname;
    QString      m_serviceName;

protected Q_SLOTS:
    virtual void slotButtonClicked(int button);
  
};  
}
#endif