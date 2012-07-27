/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2008-12-26
 * Description : a kipi plugin to import/export images to Facebook web service
 *
 * Copyright (C) 2005-2008 by Vardhman Jain <vardhman at gmail dot com>
 * Copyright (C) 2008-2012 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2008-2009 by Luka Renko <lure at kubuntu dot org>
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

// To disable warnings under MSVC2008 about POSIX methods().
#ifdef _MSC_VER
#pragma warning(disable : 4996)
#endif

#include "plugin_facebook.moc"

// C ANSI includes

extern "C"
{
#include <unistd.h>
}

// KDE includes

#include <kdebug.h>
#include <kapplication.h>
#include <kaction.h>
#include <kactioncollection.h>
#include <kstandarddirs.h>
#include <kwindowsystem.h>
#include <kgenericfactory.h>

// LibKIPI includes

#include <libkipi/interface.h>
#include <libkipi/imagecollection.h>

// Local includes

#include "fbwindow.h"
#include "facebookjob.h"

namespace KIPIFacebookPlugin
{

K_PLUGIN_FACTORY( FacebookFactory, registerPlugin<Plugin_Facebook>(); )
K_EXPORT_PLUGIN ( FacebookFactory("kipiplugin_facebook") )

Plugin_Facebook::Plugin_Facebook(QObject* const parent, const QVariantList& /*args*/)
    : Plugin(FacebookFactory::componentData(), parent, "Facebook Import/Export"),
      m_iface(0)
{
    kDebug(AREA_CODE_LOADING) << "Plugin_Facebook plugin loaded";

    KIconLoader::global()->addAppDir("kipiplugin_facebook");

    m_iface = dynamic_cast<Interface*>(parent);
    if (!m_iface)
    {
        kError() << "Kipi interface is null!";
        return;
    }

    setUiBaseName("kipiplugin_facebookui.rc");
    setupXML();
}

Plugin_Facebook::~Plugin_Facebook()
{
}

void Plugin_Facebook::setup(QWidget* const widget)
{
    m_dlgImport = 0;
    m_dlgExport = 0;

    Plugin::setup(widget);

    clearActions();
    setupActions();

    if (!m_iface)
        return;

    m_actionExport->setEnabled(true);
    m_actionImport->setEnabled(true);
}

void Plugin_Facebook::setupXML()
{
    KXMLGUIClient* host = dynamic_cast<KXMLGUIClient*>(m_iface->parent());
    mergeXMLFile(host);
}

void Plugin_Facebook::setupActions()
{
    m_actionExport = actionCollection()->addAction("facebookexport");
    m_actionExport->setText(i18n("Export to &Facebook..."));
    m_actionExport->setIcon(KIcon("facebook"));
    m_actionExport->setShortcut(KShortcut(Qt::ALT+Qt::SHIFT+Qt::Key_F));
    m_actionExport->setEnabled(false);

    connect(m_actionExport, SIGNAL(triggered(bool)),
            this, SLOT(slotExport()) );

    addAction(m_actionExport);

    m_actionImport = actionCollection()->addAction("facebookimport");
    m_actionImport->setText(i18n("Import from &Facebook..."));
    m_actionImport->setIcon(KIcon("facebook"));
    m_actionImport->setShortcut(KShortcut(Qt::ALT+Qt::SHIFT+Qt::CTRL+Qt::Key_F));
    m_actionImport->setEnabled(false);

    connect(m_actionImport, SIGNAL(triggered(bool)),
            this, SLOT(slotImport()) );

    addAction(m_actionImport);
}

void Plugin_Facebook::slotExport()
{
    KStandardDirs dir;
    QString tmp = dir.saveLocation("tmp", QString("kipi-fb-") + QString::number(getpid()) + QString("/"));

    if (!m_dlgExport)
    {
        // We clean it up in the close button
        m_dlgExport = new FbWindow(tmp, false, kapp->activeWindow());
    }
    else
    {
        if (m_dlgExport->isMinimized())
            KWindowSystem::unminimizeWindow(m_dlgExport->winId());

        KWindowSystem::activateWindow(m_dlgExport->winId());
    }

    m_dlgExport->reactivate();
}

void Plugin_Facebook::slotImport()
{
    KStandardDirs dir;
    QString tmp = dir.saveLocation("tmp", QString("kipi-fb-") + QString::number(getpid()) + QString("/"));

    if (!m_dlgImport)
    {
        // We clean it up in the close button
        m_dlgImport = new FbWindow(tmp, true, kapp->activeWindow());
    }
    else
    {
        if (m_dlgImport->isMinimized())
            KWindowSystem::unminimizeWindow(m_dlgImport->winId());

        KWindowSystem::activateWindow(m_dlgImport->winId());
    }

    m_dlgImport->show();
}

KJob* Plugin_Facebook::exportFiles(const QString& album)
{
    Interface* interface = dynamic_cast<Interface*>(parent());
    return new FacebookJob(album, interface->currentSelection().images());
}

Category Plugin_Facebook::category(KAction* const action) const
{
    if (action == m_actionExport)
        return ExportPlugin;
    else if (action == m_actionImport)
        return ImportPlugin;

    kWarning() << "Unrecognized action for plugin category identification";
    return ExportPlugin;
}

} // namespace KIPIFacebookPlugin
