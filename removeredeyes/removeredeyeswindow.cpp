/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.kipi-plugins.org
 *
 * Date        : 2008-06-08
 * Description : the main window of the removeredeyes plugin
 *
 * Copyright 2008 by Andi Clemens <andi dot clemens at gmx dot net>
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

#include "removeredeyeswindow.h"
#include "removeredeyeswindow.moc"

// Qt includes.

#include <QProgressBar>
#include <QVBoxLayout>

// KDE includes.

#include <kapplication.h>
#include <kconfig.h>
#include <kdebug.h>
#include <khelpmenu.h>
#include <klocale.h>
#include <kmenu.h>
#include <kmessagebox.h>
#include <kpushbutton.h>
#include <ktabwidget.h>
#include <ktoolinvocation.h>

// LibKIPI includes.

#include <libkipi/interface.h>

// Local includes.

#include "imageslist.h"
#include "kpaboutdata.h"
#include "removalsettings.h"
#include "settingstab.h"
#include "simplesettings.h"
#include "storagesettingsbox.h"
#include "workerthread.h"
#include "workerthreaddata.h"

namespace KIPIRemoveRedEyesPlugin
{

class RedEyesWindowPriv
{

public:

    RedEyesWindowPriv()
    {
        interface           = 0;
        about               = 0;
        progress            = 0;
        tabWidget           = 0;
        imageList           = 0;
        thread              = 0;
        settingsTab         = 0;
    }

    bool                        busy;
    int                         runtype;

    KIPI::Interface*            interface;
    KIPIPlugins::KPAboutData*   about;

    QProgressBar*               progress;

    KTabWidget*                 tabWidget;

    ImagesList*                 imageList;
    RemovalSettings             settings;
    SettingsTab*                settingsTab;
    WorkerThread*               thread;
};

RemoveRedEyesWindow::RemoveRedEyesWindow(KIPI::Interface *interface, QWidget *parent)
                   : KDialog(parent),
                     d(new RedEyesWindowPriv)
{
    setWindowTitle(i18n("Remove Red-Eyes From Your Photos"));
    setButtons(Help|User1|User2|Close);
    setDefaultButton(Close);
    setModal(false);

    d->busy                 = false;

    d->thread               = new WorkerThread(this);
    d->runtype              = WorkerThread::TestRun;
    d->interface            = interface;
    d->tabWidget            = new KTabWidget;

    d->imageList            = new ImagesList(interface);

    d->progress             = new QProgressBar;
    d->progress->setMaximumHeight(fontMetrics().height() + 2);
    d->progress->hide();

    // about & help ----------------------------------------------------------

    d->about = new KIPIPlugins::KPAboutData(ki18n("Remove Red-Eyes"),
                                            0,
                                            KAboutData::License_GPL,
                                            ki18n("A plugin to automatically "
                                                  "detect and remove red-eyes"),
                                            ki18n("(c) 2008, Andi Clemens"));

    d->about->addAuthor(ki18n("Andi Clemens"), ki18n("Author and Maintainer"),
                              "andi dot clemens at gmx dot net");

    KHelpMenu* helpMenu = new KHelpMenu(this, d->about, false);
    helpMenu->menu()->removeAction(helpMenu->menu()->actions().first());
    QAction *handbook = new QAction(i18n("Plugin Handbook"), this);

    helpMenu->menu()->insertAction(helpMenu->menu()->actions().first(), handbook);
    button(Help)->setDelayedMenu(helpMenu->menu());

    // ----------------------------------------------------------

    setButtonGuiItem(User1, KGuiItem(i18n("Correct Photos"), KIcon("dialog-ok-apply")));
    setButtonGuiItem(User2, KGuiItem(i18n("Test Run"), KIcon("dialog-information")));
    setButtonToolTip(User1, i18n("Start correcting the listed images"));
    setButtonToolTip(User2, i18n("Simulate the correction process, without saving the results."));
    setButtonToolTip(Close, i18n("Exit"));

    // ----------------------------------------------------------

    d->settingsTab          = new SettingsTab;

    // ----------------------------------------------------------

    QWidget* imagesTab           = new QWidget;
    QVBoxLayout* imagesTabLayout = new QVBoxLayout;
    imagesTabLayout->addWidget(d->imageList);
    imagesTabLayout->addWidget(d->progress);
    imagesTab->setLayout(imagesTabLayout);

    // ----------------------------------------------------------

    QWidget* mainWidget     = new QWidget;
    QVBoxLayout* mainLayout = new QVBoxLayout;
    d->tabWidget->insertTab(FileList, imagesTab,        i18n("Files List"));
    d->tabWidget->insertTab(Settings, d->settingsTab,   i18n("Settings"));

    mainLayout->addWidget(d->tabWidget, 5);
    mainWidget->setLayout(mainLayout);
    setMainWidget(mainWidget);

    // connections ------------------------------------------------------------------

    disconnect(this, SIGNAL(helpClicked()),
               this, SLOT(helpClicked()));

    connect(handbook, SIGNAL(triggered(bool)),
            this, SLOT(helpClicked()));

    connect(this, SIGNAL(testRunFinished()),
            this, SLOT(checkForUnprocessedImages()));

    connect(d->imageList, SIGNAL(foundRAWImages(bool)),
            this, SLOT(foundRAWImages(bool)));

    connect(this, SIGNAL(user1Clicked()),
            this, SLOT(startCorrection()));

    connect(this, SIGNAL(user2Clicked()),
            this, SLOT(startTestrun()));

    connect(this, SIGNAL(myCloseClicked()),
            this, SLOT(closeClicked()));

    connect(d->thread, SIGNAL(finished()),
            this, SLOT(threadFinished()));

    // ------------------------------------------------------------------

    KIPI::ImageCollection images = interface->currentSelection();

    if (images.isValid())
        d->imageList->addImages(images.images());

    readSettings();
    setBusy(false);
}

RemoveRedEyesWindow::~RemoveRedEyesWindow()
{
    delete d->about;
    delete d;
}

void RemoveRedEyesWindow::readSettings()
{
    KConfig config("kipirc");
    KConfigGroup group = config.group("RemoveRedEyes Settings");

    d->settings.storageMode            = group.readEntry("Storage Mode", (int)StorageSettingsBox::Subfolder);
    d->settings.subfolderName          = group.readEntry("Subfolder Name", "corrected");
    d->settings.simpleMode             = group.readEntry("Simple Mode", (int)SimpleSettings::Fast);
    d->settings.prefixName             = group.readEntry("Filename Prefix", "_corr");
    d->settings.minBlobsize            = group.readEntry("Minimum Blob Size", 10);
    d->settings.minRoundness           = group.readEntry("Minimum Roundness", 3.2);
    d->settings.neighborGroups         = group.readEntry("Neighbor Groups", 2);
    d->settings.scaleFactor            = group.readEntry("Scaling Factor", 1.2);
    d->settings.useStandardClassifier  = group.readEntry("Use Standard Classifier", true);
    d->settings.classifierFile         = group.readEntry("Classifier", STANDARD_CLASSIFIER);

    d->settingsTab->loadSettings(d->settings);
}

void RemoveRedEyesWindow::writeSettings()
{
    d->settings = d->settingsTab->readSettingsForSave();

    KConfig config("kipirc");
    KConfigGroup grp = config.group("RemoveRedEyes Settings");

    grp.writeEntry("Simple Mode",               d->settings.simpleMode);
    grp.writeEntry("Storage Mode",              d->settings.storageMode);
    grp.writeEntry("Subfolder Name",            d->settings.subfolderName);
    grp.writeEntry("Filename Prefix",           d->settings.prefixName);

    grp.writeEntry("Minimum Blob Size",         d->settings.minBlobsize);
    grp.writeEntry("Minimum Roundness",         d->settings.minRoundness);
    grp.writeEntry("Neighbor Groups",           d->settings.neighborGroups);
    grp.writeEntry("Scaling Factor",            d->settings.scaleFactor);
    grp.writeEntry("Use Standard Classifier",   d->settings.useStandardClassifier);
    grp.writeEntry("Classifier",                d->settings.classifierFile);

    KConfigGroup dialogGroup = config.group("RemoveRedEyes Dialog");
    saveDialogSize(dialogGroup);
    config.sync();
}

void RemoveRedEyesWindow::updateSettings()
{
    d->settings = d->settingsTab->readSettings();
}

void RemoveRedEyesWindow::startCorrection()
{
    updateSettings();
    d->runtype = WorkerThread::Correction;
    startWorkerThread();
}

void RemoveRedEyesWindow::startTestrun()
{
    updateSettings();
    d->runtype = WorkerThread::TestRun;
    startWorkerThread();
}

void RemoveRedEyesWindow::cancelCorrection()
{
    if (d->busy && d->thread->isRunning())
    {
        d->thread->cancel();
        KApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    }
}

void RemoveRedEyesWindow::closeClicked()
{
    writeSettings();
    done(Close);
}

void RemoveRedEyesWindow::helpClicked()
{
    KToolInvocation::invokeHelp("removeredeyes", "kipi-plugins");
}

void RemoveRedEyesWindow::startWorkerThread()
{
    KUrl::List urls = d->imageList->imageUrls();
    if (urls.isEmpty())
        return;

    if (d->busy)
        return;

    if (!d->thread)
    {
        kError(51000) << "Creation of WorkerThread failed!" << endl;
        setBusy(false);
        return;
    }

    d->thread->setImagesList(urls);
    d->thread->setRunType(d->runtype);
    d->thread->loadSettings(d->settings);

    setBusy(true);

    if (d->progress->isHidden())
        d->progress->show();
    d->progress->reset();
    d->progress->setRange(0, urls.count());
    d->progress->setValue(0);

    connect(d->thread, SIGNAL(calculationFinished(WorkerThreadData*)),
            this, SLOT(calculationFinished(WorkerThreadData*)));

    // start image processing
    if (!d->thread->isRunning())
        d->thread->start();
}

void RemoveRedEyesWindow::setBusy(bool busy)
{
    d->busy = busy;

    if (busy)
    {
        // disable connection to make sure that the "test run" and "correct photos"
        // buttons are not enabled again on ImageListChange
        disconnect(d->imageList, SIGNAL(imageListChanged(bool)),
                this, SLOT(imageListChanged(bool)));

        disconnect(this, SIGNAL(myCloseClicked()),
                   this, SLOT(closeClicked()));

        d->imageList->resetEyeCounterColumn();
        d->tabWidget->setCurrentIndex(FileList);

        setButtonGuiItem(Close, KStandardGuiItem::cancel());
        enableButton(User1, false);
        enableButton(User2, false);

        connect(this, SIGNAL(myCloseClicked()),
                this, SLOT(cancelCorrection()));
    }
    else
    {
        // enable connection again to make sure that an empty image list will
        // disable the "test run" and "correct photos" buttons
        connect(d->imageList, SIGNAL(imageListChanged(bool)),
                this, SLOT(imageListChanged(bool)));

        disconnect(this, SIGNAL(myCloseClicked()),
                   this, SLOT(cancelCorrection()));

        setButtonGuiItem(Close, KStandardGuiItem::close());
        enableButton(User1, true);
        enableButton(User2, true);

        connect(this, SIGNAL(myCloseClicked()),
                this, SLOT(closeClicked()));
    }
}

void RemoveRedEyesWindow::checkForUnprocessedImages()
{
    // check if imageslist has none corrected eyes
    if (d->imageList->hasUnprocessedImages())
    {
        if (KMessageBox::questionYesNo(this,
                                       i18n("<p>Some of the images could not be analyzed "
                                            "with the current settings or they do not "
                                            "contain any red-eyes at all.</p>"
                                            "<p><b>Would you like to remove those images "
                                            "from the list?</b></p>"),
                                       i18n("Remove unprocessed images?")) == KMessageBox::Yes)
        {
            d->imageList->removeUnprocessedImages();
        }
    }
}

void RemoveRedEyesWindow::imageListChanged(bool)
{
    if (d->imageList->imageUrls().isEmpty())
    {
        enableButton(User1, false);
        enableButton(User2, false);
    }
    else
    {
        enableButton(User1, true);
        enableButton(User2, true);
    }
}

void RemoveRedEyesWindow::foundRAWImages(bool raw)
{
    if (raw)
    {
        KMessageBox::information(this,
                                 i18n("<p>You tried to add <b>RAW images</b> to the red-eye batch removal plugin, "
                                      "but those filetypes are not supported.</p>"
                                      "<p><b>They were automatically removed from the list.</b></p>"),
                                 i18n("RAW images found"));
    }
}

void RemoveRedEyesWindow::calculationFinished(WorkerThreadData* data)
{
    if (!data)
        return;

    int current     = data->current;
    const KUrl& url = data->urls;
    int eyes        = data->eyes;
    delete data;

    d->progress->setValue(current);
    d->imageList->addEyeCounterByUrl(url, eyes);
}

void RemoveRedEyesWindow::slotButtonClicked(int button)
{
    emit buttonClicked(static_cast<KDialog::ButtonCode> (button));

    switch (button)
    {
        case User2:
            emit user2Clicked();
            break;
        case User1:
            emit user1Clicked();
            break;
        case Cancel:
            emit cancelClicked();
            break;
        case Close:
            emit myCloseClicked();
            break;
        case Help:
            emit helpClicked();
            break;
        case Default:
            emit defaultClicked();
            break;
    }
}

void RemoveRedEyesWindow::threadFinished()
{
    d->progress->hide();
    setBusy(false);
    KApplication::restoreOverrideCursor();

    if (d->runtype == WorkerThread::TestRun)
    {
        emit testRunFinished();
    }

    disconnect(d->thread, SIGNAL(calculationFinished(WorkerThreadData*)),
               this, SLOT(calculationFinished(WorkerThreadData*)));
}

} // namespace KIPIRemoveRedEyesPlugin
