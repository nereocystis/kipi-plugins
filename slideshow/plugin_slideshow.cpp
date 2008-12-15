/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2003-01-31
 * Description : a kipi plugin to slide images.
 *
 * Copyright (C) 2006-2008 by Valerio Fuoglio <valerio dot fuoglio at gmail dot com>
 * Copyright (C) 2003-2005 by Renchi Raju <renchi@pooh.tam.uiuc.edu>
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

#include "plugin_slideshow.h"
#include "plugin_slideshow.moc"

// C ANSI includes.

extern "C"
{
#include <sys/time.h>
}

// C++ includes.

#include <ctime>
#include <cstdlib>

// Qt includes.

#include <Q3ValueList>
#include <QPair>
#include <QStringList>

// KDE includes.

#include <klocale.h>
#include <kaction.h>
#include <kapplication.h>
#include <kgenericfactory.h>
#include <kactioncollection.h>
#include <klibloader.h>
#include <kconfig.h>
#include <kdebug.h>
#include <kmessagebox.h>

// LibKIPI includes.

#include <libkipi/interface.h>
#include <libkipi/imagecollection.h>

// Local includes.

#include "slideshowconfig.h"
#include "slideshow.h"
#include "slideshowgl.h"
#include "slideshowkb.h"

using namespace KIPISlideShowPlugin;

K_PLUGIN_FACTORY( SlideShowFactory, registerPlugin<Plugin_SlideShow>(); )
K_EXPORT_PLUGIN ( SlideShowFactory("kipiplugin_slideshow") )

Plugin_SlideShow::Plugin_SlideShow(QObject *parent, const QVariantList &args)
                : KIPI::Plugin( SlideShowFactory::componentData(), parent, "SlideShow")
{
    // Useless: to please the compiler
    QVariantList argsList = args;

    kDebug( 51001 ) << "Plugin_SlideShow plugin loaded" << endl;

    m_sharedData = 0;
}

void Plugin_SlideShow::setup( QWidget* widget )
{
    KIPI::Plugin::setup( widget );

    m_actionSlideShow = new KAction(KIcon("slideshow"), i18n("Advanced Slideshow..."), actionCollection());
    m_actionSlideShow->setObjectName("slideshow");
    connect(m_actionSlideShow, SIGNAL(triggered(bool)),
            this, SLOT(slotActivate()));

    m_interface = dynamic_cast< KIPI::Interface* >( parent() );

    if ( !m_interface )
    {
        kError( 51000 ) << "Kipi m_interface is null!" << endl;
        return;
    }

    m_urlList   = new KUrl::List();

    m_actionSlideShow->setEnabled( false );

    connect(m_interface, SIGNAL( currentAlbumChanged( bool )),
            this, SLOT( slotAlbumChanged( bool )));

    addAction( m_actionSlideShow );
}

Plugin_SlideShow::~Plugin_SlideShow()
{
    delete m_urlList;
}

void Plugin_SlideShow::slotActivate()
{
    if ( !m_interface )
    {
        kError( 51000 ) << "Kipi m_interface is null!" << endl;
        return;
    }

    m_sharedData = new SharedData();

    m_sharedData->showSelectedFilesOnly = true;

    m_sharedData->interface         = m_interface;
    m_sharedData->ImagesHasComments = m_interface->hasFeature(KIPI::ImagesHasComments);
    m_sharedData->urlList           = m_urlList;
    KIPI::ImageCollection currSel   = m_interface->currentSelection();

    if ( !currSel.isValid() || currSel.images().isEmpty() )
    {
        m_sharedData->showSelectedFilesOnly = false;
    }

    SlideShowConfig *slideShowConfig = new SlideShowConfig( kapp->activeWindow(), m_sharedData );

    connect(slideShowConfig, SIGNAL(buttonStartClicked()),
            this, SLOT(slotSlideShow()));

    slideShowConfig->show();
}

void Plugin_SlideShow::slotAlbumChanged(bool anyAlbum)
{
    if (!anyAlbum)
    {
        m_actionSlideShow->setEnabled( false );
        return;
    }

    KIPI::Interface* m_interface = dynamic_cast<KIPI::Interface*>( parent() );

    if ( !m_interface )
    {
        kError( 51000 ) << "Kipi m_interface is null!";
        m_actionSlideShow->setEnabled( false );
        return;
    }

    KIPI::ImageCollection currAlbum = m_interface->currentAlbum();

    if ( !currAlbum.isValid() )
    {
        kError( 51000 ) << "Current image collection is not valid." << endl;
        m_actionSlideShow->setEnabled( false );
        return;
    }

    m_actionSlideShow->setEnabled(true);
}

void Plugin_SlideShow::slotSlideShow()
{
    if ( !m_interface )
    {
        kError( 51000 ) << "Kipi m_interface is null!" << endl;
        return;
    }

    KConfig config("kipirc");

    bool    opengl;
    bool    shuffle;
    bool    wantKB;
    KConfigGroup grp = config.group("SlideShow Settings");
    opengl           = grp.readEntry("OpenGL", false);
    shuffle          = grp.readEntry("Shuffle", false);
    wantKB           = grp.readEntry("Effect Name (OpenGL)") == QString("Ken Burns");

    if ( m_urlList->isEmpty() )
    {
        KMessageBox::sorry(kapp->activeWindow(), i18n("There are no images to show."));
        return;
    }

    typedef QPair<QString, int> FileAnglePair;

    typedef Q3ValueList<FileAnglePair > FileList;
    FileList fileList;
    QStringList commentsList;

    for ( KUrl::List::ConstIterator urlIt = m_urlList->constBegin(); urlIt != m_urlList->constEnd(); ++urlIt )
    {
        KIPI::ImageInfo info = m_interface->info( *urlIt );
        fileList.append( FileAnglePair((*urlIt).path(), info.angle()) );
        commentsList.append(info.description());
    }

    m_urlList->clear();

    if (shuffle)
    {

        struct timeval tv;
        gettimeofday(&tv, 0);
        srand(tv.tv_sec);

        FileList::iterator it  = fileList.begin();
        FileList::iterator it1;

        QStringList::iterator itcom = commentsList.begin();
        QStringList::iterator itcom1;

        for (uint i = 0; i < (uint) fileList.size(); i++)
        {
            int inc = (int) (float(fileList.count()) * rand() / (RAND_MAX + 1.0));

            it1 = fileList.begin();
            it1 += inc;

            itcom1 = commentsList.begin();
            itcom1 += inc;

            qSwap(*(it++), *(it1));
            qSwap(*(itcom++), *(itcom1));
        }
    }

    if (!opengl)
    {
        SlideShow* slideShow = new SlideShow(fileList, commentsList, m_sharedData);
        slideShow->show();
    }
    else
    {
        if (!QGLFormat::hasOpenGL())
            KMessageBox::error(kapp->activeWindow(),
                               i18n("Sorry. OpenGL support not available on your system"));
        else
        {
            if (wantKB)
            {
                SlideShowKB* slideShow = new SlideShowKB(fileList, commentsList, m_sharedData);
                slideShow->show();
            }
            else
            {
                SlideShowGL* slideShow = new SlideShowGL(fileList, commentsList, m_sharedData);
                slideShow->show();
            }
        }
    }
}

KIPI::Category Plugin_SlideShow::category( KAction* action ) const
{
    if ( action == m_actionSlideShow )
        return KIPI::ToolsPlugin;

    kWarning( 51000 ) << "Unrecognized action for plugin category identification";

    return KIPI::ToolsPlugin; // no warning from compiler, please
}
