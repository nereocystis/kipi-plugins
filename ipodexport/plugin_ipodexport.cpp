/***************************************************************************
 * copyright            : (C) 2006 Seb Ruiz <me@sebruiz.net>               *
 **************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

extern "C" {
#include <glib-object.h> //g_type_init
}

#include "ipodexportdialog.h"
#include "plugin_ipodexport.h"

#include <libkipi/imagecollection.h>
#include <KActionCollection>

#include <kaction.h>
#include <kapplication.h>
#include <kconfig.h>
#include <kdebug.h>
#include <kgenericfactory.h>
#include <klibloader.h>
#include <klocale.h>


#define debug() kDebug( 51000 )

K_PLUGIN_FACTORY( IpodFactory, registerPlugin<Plugin_iPodExport>(); )
K_EXPORT_PLUGIN ( IpodFactory("kipiplugin_ipodexport") )


Plugin_iPodExport::Plugin_iPodExport( QObject *parent, const QVariantList& )
    : KIPI::Plugin( IpodFactory::componentData(), parent, "iPodExport")
{
    kDebug( 51001 ) << "Plugin_iPodExport plugin loaded" << endl;

    g_type_init();
}

void Plugin_iPodExport::setup( QWidget* widget )
{
    KIPI::Plugin::setup( widget );

    // this is our action shown in the menubar/toolbar of the mainwindow
    //m_actionImageUpload = new KAction( i18n( "Export to iPod..." ), "ipod_unmount", 0, this,
    //                                  SLOT( slotImageUpload() ), actionCollection(), "connectipod");

    m_actionImageUpload = new KAction(i18n( "Export to iPod..." ), actionCollection());
    
    m_actionImageUpload->setIcon(KIcon("ipod_unmount"));
    connect(m_actionImageUpload, SIGNAL(triggered(bool)),
            this, SLOT(slotImageUpload()));
    addAction( m_actionImageUpload );

    m_interface = dynamic_cast< KIPI::Interface* >( parent() );
}

KIPI::Category Plugin_iPodExport::category( KAction* action ) const
{
    if ( action == m_actionImageUpload )
        return KIPI::ExportPlugin;

    return KIPI::ImagesPlugin; // no warning from compiler, please
}


void Plugin_iPodExport::slotImageUpload()
{
    IpodExport::UploadDialog *dlg = new IpodExport::UploadDialog( m_interface, i18n("iPod Export"),
                                                                  kapp->activeWindow() );
    dlg->setMinimumWidth( 460 );
    dlg->show();
}
