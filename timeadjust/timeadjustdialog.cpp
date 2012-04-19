/* ============================================================
 *
 * This file is a part of kipi-plugins project
 * http://www.digikam.org
 *
 * Date        : 2004-05-16
 * Description : a plugin to set time stamp of picture files.
 *
 * Copyright (C) 2012      by Smit Mehta <smit dot meh at gmail dot com>
 * Copyright (C) 2003-2005 by Jesper Pedersen <blackie@kde.org>
 * Copyright (C) 2006-2012 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "timeadjustdialog.moc"

// Qt includes

#include <QButtonGroup>
#include <QCheckBox>
#include <QCloseEvent>
#include <QFile>
#include <QFileInfo>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QPushButton>
#include <QRadioButton>
#include <QSpinBox>
#include <QToolButton>
#include <QVBoxLayout>
#include <QTimeEdit>
#include <QComboBox>
#include <QPointer>
#include <QMap>
#include <QCursor>

// KDE includes

#include <kapplication.h>
#include <kconfig.h>
#include <kdebug.h>
#include <kiconloader.h>
#include <klocale.h>
#include <kstandarddirs.h>
#include <kvbox.h>
#include <kmessagebox.h>

// Local includes

#include "kpaboutdata.h"
#include "kpmetadata.h"
#include "kpimageinfo.h"
#include "kpversion.h"
#include "kpprogresswidget.h"
#include "myimagelist.h"
#include "clockphotodialog.h"
#include "actionthread.h"

namespace KIPITimeAdjustPlugin
{

class TimeAdjustDialog::TimeAdjustDialogPrivate
{

public:

    TimeAdjustDialogPrivate()
    {
        useGroupBox            = 0;
        adjustGroupBox         = 0;
        updateGroupBox         = 0;
        useButtonGroup         = 0;
        useApplDateBtn         = 0;
        useFileDateBtn         = 0;
        useMetaDateBtn         = 0;
        useCustomDateBtn       = 0;
        updAppDateCheck        = 0;
        updFileModDateCheck    = 0;
        updEXIFModDateCheck    = 0;
        updEXIFOriDateCheck    = 0;
        updEXIFDigDateCheck    = 0;
        updIPTCDateCheck       = 0;
        updXMPDateCheck        = 0;
        updFileNameCheck       = 0;
        useFileDateTypeChooser = 0;
        useMetaDateTypeChooser = 0;
        adjTypeChooser         = 0;
        adjDaysLabel           = 0;
        adjDaysInput           = 0;
        adjDetByClockPhotoBtn  = 0;
        useCustDateInput       = 0;
        useCustTimeInput       = 0;
        adjTimeInput           = 0;
        useCustomDateTodayBtn  = 0;
        progressBar            = 0;
        thread                 = 0;
        listView               = 0;
    }

    QGroupBox*            useGroupBox;
    QGroupBox*            adjustGroupBox;
    QGroupBox*            updateGroupBox;

    QButtonGroup*         useButtonGroup;

    QRadioButton*         useApplDateBtn;
    QRadioButton*         useFileDateBtn;
    QRadioButton*         useMetaDateBtn;
    QRadioButton*         useCustomDateBtn;

    QCheckBox*            updAppDateCheck;
    QCheckBox*            updFileModDateCheck;
    QCheckBox*            updEXIFModDateCheck;
    QCheckBox*            updEXIFOriDateCheck;
    QCheckBox*            updEXIFDigDateCheck;
    QCheckBox*            updIPTCDateCheck;
    QCheckBox*            updXMPDateCheck;
    QCheckBox*            updFileNameCheck;

    QComboBox*            useFileDateTypeChooser;
    QComboBox*            useMetaDateTypeChooser;
    QComboBox*            adjTypeChooser;

    QLabel*               adjDaysLabel;

    QSpinBox*             adjDaysInput;

    QPushButton*          adjDetByClockPhotoBtn;

    QDateEdit*            useCustDateInput;

    QTimeEdit*            useCustTimeInput;
    QTimeEdit*            adjTimeInput;

    QToolButton*          useCustomDateTodayBtn;

    QMap<KUrl, QDateTime> itemsUsedMap;           // Map of item urls and Used Timestamps.
    QMap<KUrl, QDateTime> itemsUpdatedMap;        // Map of item urls and Updated Timestamps.

    QStringList           fileTimeErrorFiles;
    QStringList           metaTimeErrorFiles;

    KPProgressWidget*     progressBar;
    MyImageList*          listView;

    ActionThread*         thread;
};

TimeAdjustDialog::TimeAdjustDialog(QWidget* const /*parent*/)
    : KPToolDialog(0), d(new TimeAdjustDialogPrivate)
{
    setButtons(Help | Apply | Close);
    setDefaultButton(Apply);
    setCaption(i18n("Adjust Time & Date"));
    setModal(false);
    setMinimumSize(700, 500);

    setMainWidget(new QWidget(this));
    QGridLayout* mainLayout = new QGridLayout(mainWidget());
    d->listView             = new MyImageList(mainWidget());

    // -- About data and help button ----------------------------------------

    KPAboutData* about = new KPAboutData(ki18n("Time Adjust"),
                             0,
                             KAboutData::License_GPL,
                             ki18n("A Kipi plugin for adjusting the timestamp of picture files"),
                             ki18n("(c) 2003-2005, Jesper K. Pedersen\n"
                                   "(c) 2006-2012, Gilles Caulier\n"
                                   "(c) 2012, Smit Mehta"));

    about->addAuthor(ki18n("Jesper K. Pedersen"),
                     ki18n("Author"),
                     "blackie at kde dot org");

    about->addAuthor(ki18n("Gilles Caulier"),
                     ki18n("Developer"),
                     "caulier dot gilles at gmail dot com");

    about->addAuthor(ki18n("Smit Mehta"),
                     ki18n("Developer"),
                     "smit dot meh at gmail dot com");

    about->addAuthor(ki18n("Pieter Edelman"),
                     ki18n("Developer"),
                     "p dot edelman at gmx dot net");

    about->handbookEntry = QString("timeadjust");
    setAboutData(about);

    // -- Progress Bar ------------------------------------------------------------

    d->progressBar = new KPProgressWidget(mainWidget());
    d->progressBar->reset();
    d->progressBar->hide();

    // -- Use ------------------------------------------------------------

    d->useGroupBox           = new QGroupBox(i18n("Timestamp Used"), mainWidget());
    QGridLayout* useGBLayout = new QGridLayout(d->useGroupBox);
    d->useButtonGroup        = new QButtonGroup(d->useGroupBox);
    d->useButtonGroup->setExclusive(true);

    QString applDateLabelString = i18n("%1 timestamp", KGlobal::mainComponent().aboutData()->programName());
    d->useApplDateBtn           = new QRadioButton("", d->useGroupBox);
    QLabel* useApplDateLbl      = new QLabel(applDateLabelString);
    useApplDateLbl->setIndent(5);

    d->useFileDateBtn         = new QRadioButton("", d->useGroupBox);
    d->useFileDateTypeChooser = new QComboBox(d->useGroupBox);

    /* NOTE: not supported by Linux, although supported by Qt (read-only)
    d->useFileDateTypeChooser->addItem(i18n("File created"));
    */

    d->useFileDateTypeChooser->addItem(i18n("File last modified"));

    d->useMetaDateBtn         = new QRadioButton(QString(), d->useGroupBox);
    d->useMetaDateTypeChooser = new QComboBox(d->useGroupBox);
    d->useMetaDateTypeChooser->addItem(i18n("EXIF/IPTC/XMP"));
    d->useMetaDateTypeChooser->addItem(i18n("EXIF: created"));
    d->useMetaDateTypeChooser->addItem(i18n("EXIF: original"));
    d->useMetaDateTypeChooser->addItem(i18n("EXIF: digitized"));
    d->useMetaDateTypeChooser->addItem(i18n("IPTC: created"));
    d->useMetaDateTypeChooser->addItem(i18n("XMP: created"));

    d->useCustomDateBtn      = new QRadioButton(d->useGroupBox);
    d->useCustDateInput      = new QDateEdit(d->useGroupBox);
    d->useCustDateInput->setDisplayFormat("dd MMMM yyyy");
    d->useCustDateInput->setCalendarPopup(true);
    d->useCustTimeInput      = new QTimeEdit(d->useGroupBox);
    d->useCustTimeInput->setDisplayFormat("hh:mm:ss");
    d->useCustomDateTodayBtn = new QToolButton(d->useGroupBox);
    d->useCustomDateTodayBtn->setIcon(SmallIcon("go-jump-today"));
    d->useCustomDateTodayBtn->setToolTip(i18n("Reset to current date"));

    useGBLayout->setMargin(spacingHint());
    useGBLayout->setSpacing(spacingHint());
    useGBLayout->setColumnStretch(1, 1);
    useGBLayout->setColumnStretch(2, 1);
    useGBLayout->addWidget(d->useApplDateBtn,         0, 0, 1, 1);
    useGBLayout->addWidget(useApplDateLbl,            0, 1, 1, 1);
    useGBLayout->addWidget(d->useFileDateBtn,         1, 0, 1, 1);
    useGBLayout->addWidget(d->useFileDateTypeChooser, 1, 1, 1, 1);
    useGBLayout->addWidget(d->useMetaDateBtn,         2, 0, 1, 1);
    useGBLayout->addWidget(d->useMetaDateTypeChooser, 2, 1, 1, 1);
    useGBLayout->addWidget(d->useCustomDateBtn,       3, 0, 1, 1);
    useGBLayout->addWidget(d->useCustDateInput,       3, 1, 1, 1);
    useGBLayout->addWidget(d->useCustTimeInput,       3, 2, 1, 1);
    useGBLayout->addWidget(d->useCustomDateTodayBtn,  3, 3, 1, 1);

    d->useButtonGroup->addButton(d->useApplDateBtn,   0);
    d->useButtonGroup->addButton(d->useFileDateBtn,   1);
    d->useButtonGroup->addButton(d->useMetaDateBtn,   2);
    d->useButtonGroup->addButton(d->useCustomDateBtn, 3);
    d->useApplDateBtn->setChecked(true);

    // -- Adjust-----------------------------------------------------------

    d->adjustGroupBox           = new QGroupBox(i18n("Timestamp Adjustments"), mainWidget());
    QGridLayout* adjustGBLayout = new QGridLayout(d->adjustGroupBox);

    d->adjTypeChooser        = new QComboBox(d->adjustGroupBox);
    d->adjTypeChooser->addItem(i18n("Copy value"));
    d->adjTypeChooser->addItem(i18nc("add a fixed time stamp to date",      "Add"));
    d->adjTypeChooser->addItem(i18nc("subtract a fixed time stamp to date", "Subtract"));
    d->adjDaysInput          = new QSpinBox(d->adjustGroupBox);
    d->adjDaysInput->setRange(0, 9999);
    d->adjDaysInput->setSingleStep(1);
    d->adjDaysLabel          = new QLabel(i18nc("time adjust offset, days value label", "days"), d->adjustGroupBox);
    d->adjTimeInput          = new QTimeEdit(d->adjustGroupBox);
    d->adjTimeInput->setDisplayFormat("hh:mm:ss");
    d->adjDetByClockPhotoBtn = new QPushButton(i18n("Determine difference from clock photo"));

    adjustGBLayout->setMargin(spacingHint());
    adjustGBLayout->setSpacing(spacingHint());
    adjustGBLayout->setColumnStretch(0, 1);
    adjustGBLayout->setColumnStretch(1, 1);
    adjustGBLayout->setColumnStretch(3, 1);
    adjustGBLayout->addWidget(d->adjTypeChooser,        0, 0, 1, 1);
    adjustGBLayout->addWidget(d->adjDaysInput,          0, 1, 1, 1);
    adjustGBLayout->addWidget(d->adjDaysLabel,          0, 2, 1, 1);
    adjustGBLayout->addWidget(d->adjTimeInput,          0, 3, 1, 1);
    adjustGBLayout->addWidget(d->adjDetByClockPhotoBtn, 1, 0, 1, 4);

    // -- Update ------------------------------------------------------------

    d->updateGroupBox           = new QGroupBox(i18n("Timestamp Updated"), mainWidget());
    QGridLayout* updateGBLayout = new QGridLayout(d->updateGroupBox);

    d->updAppDateCheck     = new QCheckBox(applDateLabelString,        d->updateGroupBox);
    d->updFileModDateCheck = new QCheckBox(i18n("File last modified"), d->updateGroupBox);
    d->updEXIFModDateCheck = new QCheckBox(i18n("EXIF: created"),      d->updateGroupBox);
    d->updEXIFOriDateCheck = new QCheckBox(i18n("EXIF: original"),     d->updateGroupBox);
    d->updEXIFDigDateCheck = new QCheckBox(i18n("EXIF: digitized"),    d->updateGroupBox);
    d->updIPTCDateCheck    = new QCheckBox(i18n("IPTC: created"),      d->updateGroupBox);
    d->updXMPDateCheck     = new QCheckBox(i18n("XMP"),                d->updateGroupBox);
    d->updFileNameCheck    = new QCheckBox(i18n("Filename"),           d->updateGroupBox);

    updateGBLayout->setMargin(spacingHint());
    updateGBLayout->setSpacing(spacingHint());
    updateGBLayout->setColumnStretch(0, 1);
    updateGBLayout->setColumnStretch(1, 1);
    updateGBLayout->addWidget(d->updAppDateCheck,     0, 0, 1, 1);
    updateGBLayout->addWidget(d->updEXIFModDateCheck, 0, 1, 1, 1);
    updateGBLayout->addWidget(d->updEXIFOriDateCheck, 1, 0, 1, 1);
    updateGBLayout->addWidget(d->updEXIFDigDateCheck, 1, 1, 1, 1);
    updateGBLayout->addWidget(d->updXMPDateCheck,     2, 0, 1, 1);
    updateGBLayout->addWidget(d->updIPTCDateCheck,    2, 1, 1, 1);
    updateGBLayout->addWidget(d->updFileModDateCheck, 3, 0, 1, 1);
    updateGBLayout->addWidget(d->updFileNameCheck,    3, 1, 1, 1);

    if (!KPMetadata::supportXmp())
    {
        d->updXMPDateCheck->setEnabled(false);
    }

    // -----------------------------------------------------------------------

    mainLayout->addWidget(d->listView,       0, 0, 5, 1);
    mainLayout->addWidget(d->useGroupBox,    0, 1, 1, 1);
    mainLayout->addWidget(d->adjustGroupBox, 1, 1, 1, 1);
    mainLayout->addWidget(d->updateGroupBox, 2, 1, 1, 1);
    mainLayout->addWidget(d->progressBar,    3, 1, 1, 1);
    mainLayout->setColumnStretch(0, 10);
    mainLayout->setRowStretch(4, 1);
    mainLayout->setMargin(0);
    mainLayout->setSpacing(spacingHint());

    // -- Thread and its signals ---------------------------------------------

    d->thread = new ActionThread(this);

    connect(d->thread, SIGNAL(signalProgressChanged(int)),
            this, SLOT(slotProgressChanged(int)));

    connect(d->thread, SIGNAL(signalErrorFilesUpdate(QString, QString)),
            this, SLOT(slotErrorFilesUpdate(QString, QString)));

    connect(d->thread, SIGNAL(finished()),
            this, SLOT(slotThreadFinished()));

    connect(d->thread, SIGNAL(signalProcessStarted(KUrl)),
            this, SLOT(slotProcessStarted(KUrl)));

    connect(d->thread, SIGNAL(signalProcessEnded(KUrl)),
            this, SLOT(slotProcessEnded(KUrl)));

    connect(d->progressBar, SIGNAL(signalProgressCanceled()),
            this, SLOT(slotCancelThread()));

    // -- Slots/Signals ------------------------------------------------------

    connect(d->useButtonGroup, SIGNAL(buttonReleased(int)),
            this, SLOT(slotSrcTimestampChanged()));

    connect(d->useFileDateTypeChooser, SIGNAL(currentIndexChanged(int)),
            this, SLOT(slotSrcTimestampChanged()));

    connect(d->useMetaDateTypeChooser, SIGNAL(currentIndexChanged(int)),
            this, SLOT(slotSrcTimestampChanged()));

    connect(d->adjTypeChooser, SIGNAL(currentIndexChanged(int)),
            this, SLOT(slotAdjustmentTypeChanged()));

    connect(d->useCustomDateTodayBtn, SIGNAL(clicked()),
            this, SLOT(slotResetDateToCurrent()));

    connect(d->updFileNameCheck, SIGNAL(clicked()),
            this, SLOT(slotUpdateListView()));

    connect(d->adjDetByClockPhotoBtn, SIGNAL(clicked()),
            this, SLOT(slotDetAdjustmentByClockPhoto()));

    connect(this, SIGNAL(applyClicked()),
            this, SLOT(slotApplyClicked()));

    connect(this, SIGNAL(signalMyCloseClicked()),
            this, SLOT(slotCloseClicked()));

    connect(d->useCustDateInput, SIGNAL(dateChanged(QDate)),
            this, SLOT(slotUpdateListView()));

    connect(d->useCustTimeInput, SIGNAL(timeChanged(QTime)),
            this, SLOT(slotUpdateListView()));

    connect(d->adjDaysInput, SIGNAL(valueChanged(int)),
            this, SLOT(slotUpdateListView()));

    connect(d->adjTimeInput, SIGNAL(timeChanged(QTime)),
            this, SLOT(slotUpdateListView()));

    // -----------------------------------------------------------------------

    setBusy(false);
    readSettings();
    slotSrcTimestampChanged();
    slotAdjustmentTypeChanged();
}

TimeAdjustDialog::~TimeAdjustDialog()
{
    delete d;
}

void TimeAdjustDialog::closeEvent(QCloseEvent* e)
{
    if (!e) return;
    saveSettings();
    e->accept();
}

void TimeAdjustDialog::readSettings()
{
    KConfig config("kipirc");
    KConfigGroup group = config.group(QString("Time Adjust Settings"));

    int useTimestampType = group.readEntry("Use Timestamp Type", 0);
    if (useTimestampType == 0)      d->useApplDateBtn->setChecked(true);
    else if (useTimestampType == 1) d->useFileDateBtn->setChecked(true);
    else if (useTimestampType == 2) d->useMetaDateBtn->setChecked(true);
    else if (useTimestampType == 3) d->useCustomDateBtn->setChecked(true);

    d->useFileDateTypeChooser->setCurrentIndex(group.readEntry("File Timestamp Type",   0));
    d->useMetaDateTypeChooser->setCurrentIndex(group.readEntry("Meta Timestamp Type",   0));
    d->useCustDateInput->setDateTime(group.readEntry("Custom Date",                     QDateTime::currentDateTime()));
    d->useCustTimeInput->setDateTime(group.readEntry("Custom Time",                     QDateTime::currentDateTime()));

    d->adjTypeChooser->setCurrentIndex(group.readEntry("Adjustment Type",               0));
    d->adjDaysInput->setValue(group.readEntry("Adjustment Days",                        0));
    d->adjTimeInput->setDateTime(group.readEntry("Adjustment Time",                     QDateTime()));

    d->updAppDateCheck->setChecked(group.readEntry("Update Application Time",           false));
    d->updFileModDateCheck->setChecked(group.readEntry("Update File Modification Time", false));
    d->updEXIFModDateCheck->setChecked(group.readEntry("Update EXIF Modification Time", false));
    d->updEXIFOriDateCheck->setChecked(group.readEntry("Update EXIF Original Time",     false));
    d->updEXIFDigDateCheck->setChecked(group.readEntry("Update EXIF Digitization Time", false));
    d->updIPTCDateCheck->setChecked(group.readEntry("Update IPTC Time",                 false));
    d->updXMPDateCheck->setChecked(group.readEntry("Update XMP Creation Time",          false));
    d->updFileNameCheck->setChecked(group.readEntry("Update File Name",                 false));

    KConfigGroup group2 = config.group(QString("Time Adjust Dialog"));
    restoreDialogSize(group2);
}

void TimeAdjustDialog::saveSettings()
{
    KConfig config("kipirc");
    KConfigGroup group = config.group(QString("Time Adjust Settings"));

    int useTimestampType = 0; // default if d->useApplDateBtn->isChecked()
    if (d->useFileDateBtn->isChecked())   useTimestampType = 1;
    if (d->useMetaDateBtn->isChecked())   useTimestampType = 2;
    if (d->useCustomDateBtn->isChecked()) useTimestampType = 3;

    group.writeEntry("Use Timestamp Type",            useTimestampType);

    group.writeEntry("File Timestamp Type",           d->useFileDateTypeChooser->currentIndex());
    group.writeEntry("Meta Timestamp Type",           d->useMetaDateTypeChooser->currentIndex());
    group.writeEntry("Custom Date",                   d->useCustDateInput->dateTime());
    group.writeEntry("Custom Time",                   d->useCustTimeInput->dateTime());

    group.writeEntry("Adjustment Type",               d->adjTypeChooser->currentIndex());
    group.writeEntry("Adjustment Days",               d->adjDaysInput->value());
    group.writeEntry("Adjustment Time",               d->adjTimeInput->dateTime());

    group.writeEntry("Update Application Time",       d->updAppDateCheck->isChecked());
    group.writeEntry("Update File Modification Time", d->updFileModDateCheck->isChecked());
    group.writeEntry("Update EXIF Modification Time", d->updEXIFModDateCheck->isChecked());
    group.writeEntry("Update EXIF Original Time",     d->updEXIFOriDateCheck->isChecked());
    group.writeEntry("Update EXIF Digitization Time", d->updEXIFDigDateCheck->isChecked());
    group.writeEntry("Update IPTC Time",              d->updIPTCDateCheck->isChecked());
    group.writeEntry("Update XMP Creation Time",      d->updXMPDateCheck->isChecked());
    group.writeEntry("Update File Name",              d->updFileNameCheck->isChecked());

    KConfigGroup group2 = config.group(QString("Time Adjust Dialog"));
    saveDialogSize(group2);
    config.sync();
}

void TimeAdjustDialog::addItems(const KUrl::List& imageUrls)
{
    d->itemsUsedMap.clear();

    foreach (const KUrl& url, imageUrls)
    {
        d->itemsUsedMap.insert(url, QDateTime());
    }

    d->listView->listView()->clear();
    d->listView->slotAddImages(imageUrls);
    readTimestamps();
}

void TimeAdjustDialog::readTimestamps()
{
    foreach (const KUrl& url, d->itemsUsedMap.keys())
    {
        d->itemsUsedMap.insert(url, QDateTime());
    }

    if (d->useApplDateBtn->isChecked())
    {
        readApplicationTimestamps();
    }
    else if (d->useFileDateBtn->isChecked())
    {
        readFileTimestamps();
    }
    else if (d->useMetaDateBtn->isChecked())
    {
        readMetadataTimestamps();
    }
    else if (d->useCustomDateBtn->isChecked())
    {
        foreach (const KUrl& url, d->itemsUsedMap.keys())
        {
            d->itemsUsedMap.insert(url, QDateTime(d->useCustDateInput->date(), d->useCustTimeInput->time()));
        }
    }

    slotUpdateListView();
}

void TimeAdjustDialog::readApplicationTimestamps()
{
    KUrl::List floatingDateItems;

    foreach (const KUrl& url, d->itemsUsedMap.keys())
    {
        KPImageInfo info(url);
        if (info.isExactDate())
        {
            d->itemsUsedMap.insert(url, info.date());
        }
        else
        {
            floatingDateItems.append(url);
            d->itemsUsedMap.insert(url, QDateTime());
        }
    }

    // TODO (blackie): handle all items in listview with inexact timestamp through floatingDateItems.
}

void TimeAdjustDialog::readFileTimestamps()
{
    foreach (const KUrl& url, d->itemsUsedMap.keys())
    {
        QFileInfo fileInfo(url.toLocalFile());
        d->itemsUsedMap.insert(url, fileInfo.lastModified());
    }
}

void TimeAdjustDialog::readMetadataTimestamps()
{
    foreach (const KUrl& url, d->itemsUsedMap.keys())
    {
        KPMetadata meta;
        if (!meta.load(url.toLocalFile()))
        {
            d->itemsUsedMap.insert(url, QDateTime());
            continue;
        }

        QDateTime curImageDateTime;

        switch (d->useMetaDateTypeChooser->currentIndex())
        {
            case 0:
                curImageDateTime = meta.getImageDateTime();
                break;
            case 1:
                curImageDateTime = QDateTime::fromString(meta.getExifTagString("Exif.Image.DateTime"),
                                                         "yyyy:MM:dd hh:mm:ss");
                break;
            case 2:
                curImageDateTime = QDateTime::fromString(meta.getExifTagString("Exif.Photo.DateTimeOriginal"),
                                                         "yyyy:MM:dd hh:mm:ss");
                break;
            case 3:
                curImageDateTime = QDateTime::fromString(meta.getExifTagString("Exif.Photo.DateTimeDigitized"),
                                                         "yyyy:MM:dd hh:mm:ss");
                break;
            case 4:
                // we have to truncate the timezone from the time, otherwise it cannot be converted to a QTime
                curImageDateTime = QDateTime(QDate::fromString(meta.getIptcTagString("Iptc.Application2.DateCreated"), 
                                                               Qt::ISODate),
                                             QTime::fromString(meta.getIptcTagString("Iptc.Application2.TimeCreated").left(8),
                                                               Qt::ISODate));
                break;
            case 5:
                curImageDateTime = QDateTime::fromString(meta.getXmpTagString("Xmp.xmp.CreateDate"),
                                                         "yyyy:MM:dd hh:mm:ss");
                break;
            default:
                // curImageDateTime stays invalid
                break;
        };

        d->itemsUsedMap.insert(url, curImageDateTime);
    }
}

void TimeAdjustDialog::slotSrcTimestampChanged()
{
    d->useFileDateTypeChooser->setEnabled(false);
    d->useMetaDateTypeChooser->setEnabled(false);
    d->useCustDateInput->setEnabled(false);
    d->useCustTimeInput->setEnabled(false);
    d->useCustomDateTodayBtn->setEnabled(false);

    if (d->useFileDateBtn->isChecked())
    {
        d->useFileDateTypeChooser->setEnabled(true);
    }
    else if (d->useMetaDateBtn->isChecked())
    {
        d->useMetaDateTypeChooser->setEnabled(true);
    }
    else if (d->useCustomDateBtn->isChecked())
    {
        d->useCustDateInput->setEnabled(true);
        d->useCustTimeInput->setEnabled(true);
        d->useCustomDateTodayBtn->setEnabled(true);
    }

    readTimestamps();
}

void TimeAdjustDialog::slotResetDateToCurrent()
{
    QDateTime currentDateTime(QDateTime::currentDateTime());
    d->useCustDateInput->setDateTime(currentDateTime);
    d->useCustTimeInput->setDateTime(currentDateTime);
    readTimestamps();
}

void TimeAdjustDialog::slotAdjustmentTypeChanged()
{
    // If the addition or subtraction has been selected, enable the edit boxes to enter the adjustment length
    bool isAdjustment = (d->adjTypeChooser->currentIndex() > 0);
    d->adjDaysInput->setEnabled(isAdjustment);
    d->adjDaysLabel->setEnabled(isAdjustment);
    d->adjTimeInput->setEnabled(isAdjustment);

    // update listview info (with adjustment enabled also the adjusted time is shown)
    slotUpdateListView();
}

void TimeAdjustDialog::slotDetAdjustmentByClockPhoto()
{
    /* When user press the clock photo button, a dialog is displayed and set the
     * results to the proper widgets.
     */
    QPointer<ClockPhotoDialog> dlg = new ClockPhotoDialog(this);
    int result                     = dlg->exec();

    if (result == QDialog::Accepted)
    {
        if (dlg->deltaDays == 0 && dlg->deltaHours == 0 && dlg->deltaMinutes == 0 && dlg->deltaSeconds == 0)
        {
            d->adjTypeChooser->setCurrentIndex(0);
        }
        else if (dlg->deltaNegative)
        {
            d->adjTypeChooser->setCurrentIndex(2);
        }
        else
        {
            d->adjTypeChooser->setCurrentIndex(1);
        }

        d->adjDaysInput->setValue(dlg->deltaDays);
        QTime deltaTime;
        deltaTime.setHMS(dlg->deltaHours, dlg->deltaMinutes, dlg->deltaSeconds);
        d->adjTimeInput->setTime(deltaTime);
    }

    delete dlg;
}

void TimeAdjustDialog::slotApplyClicked()
{
    d->progressBar->show();
    d->progressBar->progressScheduled(i18n("Adjust Time and Date"), true, true);
    d->progressBar->progressThumbnailChanged(KIcon("kipi").pixmap(22, 22));
    d->progressBar->setMaximum(d->itemsUsedMap.keys().size());

    // NOTE: this loop is not yet re-entrant due to use KPImageInfo. It must still in main thread.
    foreach (const KUrl& url, d->itemsUpdatedMap.keys())
    {
        if (d->updAppDateCheck->isChecked())
        {
            KPImageInfo info(url);
            QDateTime dt = d->itemsUpdatedMap.value(url);
            if (dt.isValid()) info.setDate(dt);
            kapp->processEvents();
        }
    }

    d->thread->setAppDateCheck(d->updAppDateCheck->isChecked());
    d->thread->setFileNameCheck(d->updFileNameCheck->isChecked());
    d->thread->setEXIFDataCheck(d->updEXIFModDateCheck->isChecked(),
                                d->updEXIFOriDateCheck->isChecked(),
                                d->updEXIFDigDateCheck->isChecked());
    d->thread->setIPTCDateCheck(d->updIPTCDateCheck->isChecked());
    d->thread->setXMPDateCheck(d->updXMPDateCheck->isChecked());
    d->thread->setFileModDateCheck(d->updFileModDateCheck->isChecked());
    d->thread->setUpdatedDates(d->itemsUpdatedMap);

    if (!d->thread->isRunning())
    {
        d->thread->start();
    }

    enableButton(Apply, false);
    setBusy(true);
}

void TimeAdjustDialog::slotButtonClicked(int button)
{
    emit buttonClicked(static_cast<KDialog::ButtonCode>(button));

    switch (button)
    {
        case Apply:
            emit applyClicked();
            break;
        case Close:
            emit signalMyCloseClicked();
            break;
        default:
            break;
    }
}

void TimeAdjustDialog::setBusy(bool busy)
{
    if (busy)
    {
        disconnect(this, SIGNAL(signalMyCloseClicked()),
                   this, SLOT(slotCloseClicked()));

        setButtonGuiItem(Close, KStandardGuiItem::cancel());
        enableButton(Apply, false);

        connect(this, SIGNAL(signalMyCloseClicked()),
                this, SLOT(slotCancelThread()));
    }
    else
    {
        disconnect(this, SIGNAL(signalMyCloseClicked()),
                   this, SLOT(slotCancelThread()));

        setButtonGuiItem(Close, KStandardGuiItem::close());
        enableButton(Apply, true);

        connect(this, SIGNAL(signalMyCloseClicked()),
                this, SLOT(slotCloseClicked()));
     }
}

void TimeAdjustDialog::slotCloseClicked()
{
    saveSettings();
    done(Close);
}

QDateTime TimeAdjustDialog::calculateAdjustedTime(const QDateTime& originalTime) const
{
    int sign = 0;

    switch (d->adjTypeChooser->currentIndex())
    {
        case 1:
            sign = 1;
            break;
        case 2:
            sign = -1;
            break;
        default:
            return originalTime;
    };

    const QTime& adjTime = d->adjTimeInput->time();
    int seconds          = adjTime.second();
    seconds             += 60*adjTime.minute();
    seconds             += 60*60*adjTime.hour();
    seconds             += 24*60*60*d->adjDaysInput->value();

    return originalTime.addSecs(sign * seconds);
}

void TimeAdjustDialog::slotProgressChanged(int progress)
{
    d->progressBar->setValue(progress);
}

void TimeAdjustDialog::slotErrorFilesUpdate(const QString& fileTimeErrorFile, const QString& metaTimeErrorFile)
{
    if (!fileTimeErrorFile.isEmpty())
    {
        d->fileTimeErrorFiles.append(fileTimeErrorFile);
    }

    if (!metaTimeErrorFile.isEmpty())
    {
        d->metaTimeErrorFiles.append(metaTimeErrorFile);
    }
}

void TimeAdjustDialog::slotCancelThread()
{
    if (d->thread->isRunning())
    {
        d->thread->cancel();
    }
}

void TimeAdjustDialog::slotProcessStarted(const KUrl& url)
{
    d->listView->processing(url);
}

void TimeAdjustDialog::slotProcessEnded(const KUrl& url)
{
    d->listView->processed(url, true);
}

void TimeAdjustDialog::slotThreadFinished()
{
    if (!d->metaTimeErrorFiles.isEmpty())
        {
            KMessageBox::informationList(
                         kapp->activeWindow(),
                         i18n("Unable to update metadata in:"),
                         d->metaTimeErrorFiles,
                         i18n("Adjust Time & Date"));
        }

        if (!d->fileTimeErrorFiles.isEmpty())
        {
            KMessageBox::informationList(
                         kapp->activeWindow(),
                         i18n("Unable to update file modification time in:"),
                         d->fileTimeErrorFiles,
                         i18n("Adjust Time & Date"));
        }

    setBusy(false);
    d->progressBar->hide();
    d->progressBar->progressCompleted();
    enableButton(Apply, true);
    saveSettings();
}

void TimeAdjustDialog::slotUpdateListView()
{
    kapp->setOverrideCursor(Qt::WaitCursor);

    d->listView->setItemDates(d->itemsUsedMap, MyImageList::TIMESTAMP_USED);

    foreach (const KUrl& url, d->itemsUsedMap.keys())
    {
        d->itemsUpdatedMap.insert(url, calculateAdjustedTime(d->itemsUsedMap.value(url)));
    }

    d->listView->setItemDates(d->itemsUpdatedMap, MyImageList::TIMESTAMP_UPDATED, d->updFileNameCheck->isChecked());

    kapp->restoreOverrideCursor();
}

/*
void TimeAdjustDialog::slotUpdateExample()
{
    QString exampleTimeFormat("hh:mm:ss");

    // When the custom timestamp is to be used, do not show any file original timestamps
    if (d->useCustomDateBtn->isChecked())
    {
        QDateTime customTime(d->useCustDateInput->date(), d->useCustTimeInput->time());
        if (d->adjTypeChooser->currentIndex() > 0)
            customTime = calculateAdjustedTime(customTime);

        QDate customDate            = customTime.date();
        QString formattedCustomDate = KGlobal::locale()->formatDate(customDate, KLocale::ShortDate);

        QString customTimeStr = customTime.toString(exampleTimeFormat);
        d->exampleTimeChangeLabel->setText(i18n("Custom: <b>%1 %2</b>", formattedCustomDate, customTimeStr));
        return;
    }

    // If the file timestamp structures are not ready yet, do not show any information
    // (this may happen during initialization)
    if (d->imageOriginalDates.size() == 0 || d->exampleFileChooser->currentIndex() >= d->imageOriginalDates.size()) 
    {
        d->exampleTimeChangeLabel->setText("");
        return;
    }

    // Get the file original timestamp according to the selection, if it is not available inform the user
    QDateTime originalTime = d->imageOriginalDates.at(d->exampleFileChooser->currentIndex());
    if (!originalTime.isValid())
    {
        d->exampleTimeChangeLabel->setText(i18n("Original: <b>N/A</b><br/>"
                                                "Image will be skipped"));
        return;
    }

    // Show the file original timestamp (and adjusted one if needed)

    QDate   originalDate          = originalTime.date();
    QString formattedOriginalDate = KGlobal::locale()->formatDate(originalDate, KLocale::ShortDate);
    QString originalTimeStr       = originalTime.toString(exampleTimeFormat);

    if (d->adjTypeChooser->currentIndex() == 0)
    {
        d->exampleTimeChangeLabel->setText(i18n("Original: <b>%1 %2</b>", formattedOriginalDate, originalTimeStr));
        return;
    }
    else
    {
        QDateTime adjustedTime        = calculateAdjustedTime(originalTime);
        QDate adjustedDate            = adjustedTime.date();
        QString formattedAdjustedDate = KGlobal::locale()->formatDate(adjustedDate, KLocale::ShortDate);

        QString adjustedTimeStr = adjustedTime.toString(exampleTimeFormat);
        d->exampleTimeChangeLabel->setText(i18n("Original: <b>%1 %2</b><br/>"
                                                "Adjusted: <b>%3 %4</b>",
                                           formattedOriginalDate, originalTimeStr, formattedAdjustedDate, adjustedTimeStr));
    }
}
*/

}  // namespace KIPITimeAdjustPlugin
