/***************************************************************************
 *   This file is part of the CuteReport project                           *
 *   Copyright (C) 2012-2015 by Alexander Mikhalov                         *
 *   alexander.mikhalov@gmail.com                                          *
 *                                                                         *
 **                   GNU General Public License Usage                    **
 *                                                                         *
 *   This library is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation, either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>. *
 *                                                                         *
 *                                                                         *
 *   This library is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 ***************************************************************************/

#include "reporteditor.h"
#include "reportcontainer.h"
#include "reportproperties.h"
#include "reportcore.h"
#include "reportinterface.h"
#include "propertyeditor.h"
#include "rendererinterface.h"
#include "printerinterface.h"
#include "storageinterface.h"
#include "mainwindow.h"
#include "stdstoragedialog.h"
#include "renamedialog.h"
#include "mainwindow.h"

#include <QMenu>
#include <QWidget>
#include <QMessageBox>
#include <QInputDialog>

using namespace CuteReport;

const int documentId = 98765;
const QString MODULENAME = "ReportEditor";

inline void initMyResource() { Q_INIT_RESOURCE(reporteditor); }

ReportEditor::ReportEditor(QObject *parent) :
    ModuleInterface(parent)
  ,m_saveReportAction(0)
  ,m_closeReportAction(0)
{
}


ReportEditor::~ReportEditor()
{
    foreach (ReportStruct st, m_reports) {
        delete st.report;
    }
    delete ui;
}


void ReportEditor::init(CuteDesigner::Core *core)
{
    initMyResource();
    ModuleInterface::init(core);

    ui = new ReportContainer(this);

    if (core->getSettingValue("ReportProperties/tabMode").isNull())
        core->setSettingValue("ReportProperties/tabMode", 2);

    connect(core, SIGNAL(currentReportChanged(CuteReport::ReportInterface*)),
            this, SLOT(slotCurrentReportChangedByCore(CuteReport::ReportInterface*)));
    connect(core, SIGNAL(requestForReport(QString)), this, SLOT(loadReport(QString)));
    connect(core, SIGNAL(appIsAboutToClose(bool*)), this, SLOT(slotAppIsAboutToClose(bool*)));
    connect(core, SIGNAL(initDone()), this, SLOT(slotDesignerInitDone()));

    // queued because need to have lowest priority for preventing emmitting currentReportChanged before reportCreated
    // cancelled since after mass undoredo delete/create pointer is not valid
    connect(core, SIGNAL(newReport_after(CuteReport::ReportInterface*)), this, SLOT(slotCoreReportCreated(CuteReport::ReportInterface*)));
    connect(core, SIGNAL(loadReport_after(CuteReport::ReportInterface*)), this, SLOT(slotCoreReportCreated(CuteReport::ReportInterface*)));

    connect(ui.data(), SIGNAL(requestForNewReport()), this, SLOT(slotRequestForNewReport()));
    connect(ui.data(), SIGNAL(requestForTemplate()), this, SLOT(slotRequestForTemplate()));
    connect(ui.data(), SIGNAL(requestForOpenReport()), this, SLOT(slotRequestForOpenReport()));
    connect(ui.data(), SIGNAL(requestForCloseReport()), this, SLOT(slotRequestForCloseReport()));
    connect(ui.data(), SIGNAL(requestForSaveReport()), this, SLOT(slotRequestForSaveReport()));
    connect(ui.data(), SIGNAL(currentTabChanged(int)), this, SLOT(slotCurrentTabChanged(int)));
    //    connect(ui.data(), SIGNAL(requestForRenameReport(int)), this, SLOT(slotRequestForRenameReport(int)));
}


void ReportEditor::reloadSettings()
{
    if (ui)
        ui->reloadSettings();
}


void ReportEditor::saveSettings()
{
    if (ui)
        ui->saveSettings();
}


void ReportEditor::sync()
{
    if (core()->currentReport()) {
        ReportIterator i;
        for (i = m_reports.begin(); i != m_reports.end(); ++i) {
            if (i->report == core()->currentReport()) {
                i->reportProperties->saveAll();
                break;
            }
        }
    }
}


QWidget * ReportEditor::view()
{
    return ui;
}


void ReportEditor::activate()
{
}


void ReportEditor::deactivate()
{
    if (core()->currentReport()) {
        ReportIterator i;
        for (i = m_reports.begin(); i != m_reports.end(); ++i) {
            if (i->report == core()->currentReport()) {
                i->reportProperties->saveAll();
                break;
            }
        }
    }
}


QIcon ReportEditor::icon()
{
    return QIcon(":images/report.png");
}


QString ReportEditor::name()
{
    return QString("Reports");
}


void ReportEditor::slotCurrentReportChangedByCore(CuteReport::ReportInterface* report)
{
    if (m_saveReportAction)
        m_saveReportAction->setEnabled(report);
    if (m_saveReportAsAction)
        m_saveReportAsAction->setEnabled(report);
    if (m_closeReportAction)
        m_closeReportAction->setEnabled(report);

    if (!report) {
        core()->setDocumentTitles(documentId, QString());
        return;
    }

    ui->setCurrentTab(report->objectName());
    QString reportName = (report->name().isEmpty() ? "NoName" : report->name()) +  (report->isValid() ? "" : "(invalid)") + (report->isDirty() ? "*" : "");
    core()->setDocumentTitles(documentId, reportName);

}


void ReportEditor::slotCurrentTabChanged(int index)
{
    if (index >= m_reports.size())
        return;

    core()->sync();
    core()->setCurrentReport(m_reports[index].report);

}


//void ReportEditor::slotPrinterChanged(CuteReport::PrinterInterface* printer)
//{
//    CuteReport::ReportInterface * report = dynamic_cast<CuteReport::ReportInterface *>(sender());
//    if (!report)
//        return;

//    ReportIterator i;
//    for (i = m_reports.begin(); i != m_reports.end(); ++i) {
//        if (i->report == report) {
//            i->printerPropertyEditor->setObject(printer);
//            break;
//        }
//    }
//}


//void ReportEditor::slotRendererChanged(CuteReport::RendererInterface* renderer)
//{
//    CuteReport::ReportInterface * report = dynamic_cast<CuteReport::ReportInterface*>(sender());
//    if (!report)
//        return;

//    ReportIterator i;
//    for (i = m_reports.begin(); i != m_reports.end(); ++i) {
//        if (i->report == report) {
//            i->rendererPropertyEditor->setObject(renderer);
//            break;
//        }
//    }
//}


void ReportEditor::slotRequestForNewRenderer(const CuteReport::RendererInterface *renderer)
{
    if (!core()->currentReport())
        return;
    core()->currentReport()->addRenderer( core()->reportCore()->createRendererObject(renderer->moduleFullName(), core()->currentReport()) );
}


void ReportEditor::slotRequestForDeleteRenderer(const QString &name)
{
    if (!core()->currentReport())
        return;
    core()->currentReport()->deleteRenderer(name);
}


void ReportEditor::slotRequestForChangeDefaultRenderer(const QString &name)
{
    if (!core()->currentReport())
        return;
    core()->currentReport()->setDefaultRendererName(name);
}


void ReportEditor::slotRequestForNewRendererName(const QString &name)
{
    bool ok;
    QString text = QInputDialog::getText(0, tr("Renderer renaming"),
                                         tr("Renderer name:"), QLineEdit::Normal,
                                         name, &ok);
    if (ok && !text.isEmpty()) {
        RendererInterface * renderer = core()->currentReport()->renderer(name);
        if (renderer)
            renderer->setObjectName(text);
    }
}


void ReportEditor::slotRequestForNewPrinter(const CuteReport::PrinterInterface* printer)
{
    if (!core()->currentReport())
        return;
    core()->currentReport()->addPrinter( core()->reportCore()->createPrinterObject(printer->moduleFullName(), core()->currentReport()) );
}


void ReportEditor::slotRequestForDeletePrinter(const QString &name)
{
    if (!core()->currentReport())
        return;
    core()->currentReport()->deletePrinter(name);
}


void ReportEditor::slotRequestForChangeDefaultPrinter(const QString &name)
{
    if (!core()->currentReport())
        return;
    core()->currentReport()->setDefaultPrinterName(name);
}


void ReportEditor::slotRequestForNewPrinterName(const QString &name)
{
    bool ok;
    QString text = QInputDialog::getText(0, tr("Printer renaming"),
                                         tr("Printer name:"), QLineEdit::Normal,
                                         name, &ok);
    if (ok && !text.isEmpty()) {
        PrinterInterface * printer = core()->currentReport()->printer(name);
        if (printer)
            printer->setObjectName(text);
    }
}


void ReportEditor::slotRequestForNewStorage(const CuteReport::StorageInterface* storage)
{
    if (!core()->currentReport())
        return;
    core()->currentReport()->addStorage( core()->reportCore()->createStorageObject( storage->moduleFullName(), core()->currentReport()) );
}


void ReportEditor::slotRequestForDeleteStorage(const QString & storageName)
{
    if (!core()->currentReport())
        return;
    core()->currentReport()->deleteStorage(storageName);
}


void ReportEditor::slotRequestForChangeDefaultStorage(const QString & storageName)
{
    if (!core()->currentReport())
        return;
    core()->currentReport()->setDefaultStorageName(storageName);
}


void ReportEditor::slotRequestForNewStorageName(const QString &storageName)
{
    bool ok;
    QString text = QInputDialog::getText(0, tr("Storage renaming"),
                                         tr("Storage name:"), QLineEdit::Normal,
                                         storageName, &ok);
    if (ok && !text.isEmpty()) {
        StorageInterface * storage = core()->currentReport()->storage(storageName);
        if (storage)
            storage->setObjectName(text);
    }
}


void ReportEditor::slotRequestForTemplate()
{
    CuteReport::StdStorageDialog d(core()->reportCore(), core()->mainWindow(), "Load Report");
//    d.setUrlHint("templates");
    if (!d.exec())
        return;

    QString selectedObjectUrl = d.currentObjectUrl();

    CuteReport::ReportInterface * new_report = core()->reportCore()->loadReport(selectedObjectUrl);

    if (new_report) {
        newReportPreprocess(new_report);
        ui->setCurrentTab(new_report->objectName());
        core()->setCurrentReport(new_report);
    }
}


void ReportEditor::slotRequestForNewReport()
{
    core()->emitNewReportBefore();

    ReportIterator i;
    for (i = m_reports.begin(); i != m_reports.end(); ++i) {
        if (i->report->name().isEmpty()) {
            QMessageBox::warning(core()->reportCore()->rootWidget(), tr("CuteReport"),
                                 tr("There is unnamed report open.\nName this report before opening new one."),
                                 QMessageBox::Ok);
            return;
        }
    }

    CuteReport::ReportInterface * new_report = core()->reportCore()->createReport();
    new_report->setAuthor("Author");

    core()->emitNewReportAfter(new_report);
}


void ReportEditor::loadReport(const QString & objectUrl)
{
    core()->emitLoadReportBefore(objectUrl);

    QString selectedObjectUrl = objectUrl;

    if (selectedObjectUrl.isEmpty()) {
        CuteReport::StdStorageDialog d(core()->reportCore(), core()->mainWindow(), "Load Report");
        QString lastURL = core()->getSettingValue("ReportProperties/lastURL").toString();
        d.setUrl(lastURL);
        //d.setUrlHint(lastURL.isEmpty() ? "reports" : lastURL);
        if (!d.exec())
            return;

        selectedObjectUrl = d.currentObjectUrl();
        core()->setSettingValue("ReportProperties/lastURL", selectedObjectUrl);
    }

    CuteReport::ReportInterface * new_report = core()->reportCore()->loadReport(selectedObjectUrl);

    if (CuteReport::ReportInterface * existsReport = sameReportExists(new_report)) {
        delete new_report;
        m_reportToSwitch = existsReport;
        QTimer::singleShot(0, this, SLOT(slotSetCurrentReport()));
    } else {
        core()->emitLoadReportAfter(new_report);
    }

}


ReportInterface *ReportEditor::sameReportExists(CuteReport::ReportInterface* newReport)
{
    CuteReport::ReportInterface* existsReport = 0;

    ReportIterator i;
    for (i = m_reports.begin(); i != m_reports.end(); ++i) {
        if (i->report->name() == newReport->name()) {
            existsReport = i->report;
            break;
        }
    }

    if (existsReport) {
        if (newReport->name().isEmpty())
            QMessageBox::warning(core()->reportCore()->rootWidget(), tr("CuteReport"),
                                 tr("Report with an empty name is already open.\nName your current report before opening new one."),
                                 QMessageBox::Ok);
        else
            QMessageBox::warning(core()->reportCore()->rootWidget(), tr("CuteReport"),
                                 tr("Report with name \'%1\' is already open.").arg(newReport->name()),
                                 QMessageBox::Ok);
        return existsReport;
    }

    return 0;
}

void ReportEditor::slotRequestForOpenReport()
{
    loadReport("");
}


void ReportEditor::slotRequestForSaveReport()
{
    saveReport(core()->currentReport(), false);
}


void ReportEditor::slotRequestForSaveReportAs()
{
    saveReport(core()->currentReport(), true);
}

bool ReportEditor::saveReport(CuteReport::ReportInterface * report, bool askFileName)
{
    if (!report)
        return false;

    core()->sync();

    //qDebug() << report->description();

    if (!report->isValid()) {
        QMessageBox::warning(core()->mainWindow(), tr("Cute Report"),
                             tr("The document is invalid and can not be saved."),
                             QMessageBox::Ok, QMessageBox::Ok);
        return false;
    }

    QString objectUrl;

    if (askFileName || report->fileUrl().isEmpty()) {
        CuteReport::StdStorageDialog d(core()->reportCore(), core()->mainWindow(), tr("Save Report"));
//        d.setUrlHint("reports");
//        d.setObjectHint(report->fileUrl());
        if (!d.exec())
            return false;
        objectUrl = d.currentObjectUrl();
    } else
        objectUrl = report->fileUrl();


    QFileInfo file(objectUrl);
    if(file.suffix().isEmpty())
        objectUrl += ".qtrp";
    bool result = core()->reportCore()->saveReport(objectUrl, report);

    return result;
}


void ReportEditor::slotRequestForCloseReport()
{
    CuteReport::ReportInterface * report = core()->currentReport();
    if (!report)
        return;

    if (report->isDirty() && report->isValid()) {
        int ret = QMessageBox::warning(core()->mainWindow(), tr("Cute Report"),
                                       tr("The document has been modified.\n"
                                          "Do you want to save your changes?"),
                                       QMessageBox::Save | QMessageBox::Discard
                                       | QMessageBox::Cancel,
                                       QMessageBox::Save);
        switch (ret) {
            case QMessageBox::Save:
                if (!saveReport(core()->currentReport()))
                    return;
                break;
            case QMessageBox::Discard:    break;
            case QMessageBox::Cancel:     return;
            default:                      return;
        }
    }

    core()->emitDeleteReportBefore(report);
    delete report;
    core()->emitDeleteReportAfter(report);
}


void ReportEditor::slotReportObjectDestroyed(QObject *report)
{
    int index = -1;
    for (int i = 0; i < m_reports.size(); ++i ) {
        if (m_reports.at(i).report == report) {
            index = i;
            break;
        }
    }

    if (index == -1)
        return;

    ReportStruct s = m_reports.takeAt(index);
    ui->removeTab(index);
//    delete s.printerPropertyEditor;
//    delete s.rendererPropertyEditor;
    delete s.reportProperties;

    index = qMin(index, m_reports.size()-1);

    core()->setCurrentReport( index<0 ? 0 : m_reports.at(index).report );
}


//void ReportEditor::slotRequestForRenameReport(int index)
//{
//    if (index >= m_reports.size())
//        return;

//    CuteReport::ReportInterface* report = m_reports.at(index).report;

//    RenameDialog d(report, report, core()->mainWindow());
//    d.setWindowTitle("Report renaming");
//    if (d.exec() == QDialog::Accepted) {
//        report->setObjectName(d.newName());

//        ui.data()->changeTabText(name, report->objectName());

//        QList>::iterator it = m_reports.find(report);
//        for (i = list.begin(); i != list.end(); ++i)
//            it.value().tabText = report->objectName();
//    }
//}


void ReportEditor::newReportPreprocess(CuteReport::ReportInterface * report)
{
    report->setFlag(ReportInterface::VariablesAutoUpdate, true);
    report->setFlag(ReportInterface::DirtynessAutoUpdate, true);

    report->setObjectName(makeCorrectObjectName(report));

    ReportStruct reportProp;

    reportProp.report = report;
    reportProp.tabText = report->objectName();
//    reportProp.rendererPropertyEditor = core()->createPropertyEditor(ui);
//    reportProp.printerPropertyEditor = core()->createPropertyEditor(ui);
    reportProp.reportProperties = new PropertyEditor::ReportProperties(core(), ui);

//    reportProp.reportProperties->addRendererPropertyEditor(reportProp.rendererPropertyEditor);
//    reportProp.reportProperties->addPrinterPropertyEditor(reportProp.printerPropertyEditor);

    reportProp.reportProperties->connectReport(report);
//    reportProp.rendererPropertyEditor->setObject(report->renderer());
//    reportProp.printerPropertyEditor->setObject(report->printer());
    m_reports.append(reportProp);

    ui->addTab(reportProp.reportProperties, QIcon(":images/report.png"), report->objectName());

//    connect(report, SIGNAL(printerChanged(CuteReport::PrinterInterface*)),
//            this, SLOT(slotPrinterChanged(CuteReport::PrinterInterface*)));
//    connect(report, SIGNAL(rendererChanged(CuteReport::RendererInterface*)),
//            this, SLOT(slotRendererChanged(CuteReport::RendererInterface*)));
    connect(report, SIGNAL(nameChanged(QString)), this, SLOT(slotReportNameChangedOutside(QString)));
    connect(report, SIGNAL(dirtynessChanged(bool)), this, SLOT(slotDirtynessChanged(bool)));
    connect(report, SIGNAL(destroyed(QObject*)), this, SLOT(slotReportObjectDestroyed(QObject*)));

    connect(reportProp.reportProperties, SIGNAL(requestForNewStorage(const CuteReport::StorageInterface*)), this, SLOT(slotRequestForNewStorage(const CuteReport::StorageInterface*)));
    connect(reportProp.reportProperties, SIGNAL(requestForDeleteStorage(QString)), this, SLOT(slotRequestForDeleteStorage(QString)));
    connect(reportProp.reportProperties, SIGNAL(requestForDefaultStorage(QString)), this, SLOT(slotRequestForChangeDefaultStorage(QString)));
    connect(reportProp.reportProperties, SIGNAL(requestForNewStorageName(QString)), this, SLOT(slotRequestForNewStorageName(QString)) );

    connect(reportProp.reportProperties, SIGNAL(requestForNewRenderer(const CuteReport::RendererInterface*)), this, SLOT(slotRequestForNewRenderer(const CuteReport::RendererInterface*)));
    connect(reportProp.reportProperties, SIGNAL(requestForDeleteRenderer(QString)), this, SLOT(slotRequestForDeleteRenderer(QString)));
    connect(reportProp.reportProperties, SIGNAL(requestForDefaultRenderer(QString)), this, SLOT(slotRequestForChangeDefaultRenderer(QString)));
    connect(reportProp.reportProperties, SIGNAL(requestForNewRendererName(QString)), this, SLOT(slotRequestForNewRendererName(QString)) );

    connect(reportProp.reportProperties, SIGNAL(requestForNewPrinter(const CuteReport::PrinterInterface*)), this, SLOT(slotRequestForNewPrinter(const CuteReport::PrinterInterface*)));
    connect(reportProp.reportProperties, SIGNAL(requestForDeletePrinter(QString)), this, SLOT(slotRequestForDeletePrinter(QString)));
    connect(reportProp.reportProperties, SIGNAL(requestForDefaultPrinter(QString)), this, SLOT(slotRequestForChangeDefaultPrinter(QString)));
    connect(reportProp.reportProperties, SIGNAL(requestForNewPrinterName(QString)), this, SLOT(slotRequestForNewPrinterName(QString)) );

    QTimer::singleShot(10, report, SLOT(updateVariables()));
}


QList<CuteDesigner::DesignerMenu*> ReportEditor::mainMenu()
{
    QWidget * parent = core()->mainWindow();

    QList<CuteDesigner::DesignerMenu*> menus;

    CuteDesigner::DesignerMenu * reportMenu = new CuteDesigner::DesignerMenu(parent, "Report", 1000, 1000); // very high priority;  very high priority;
    menus.append(reportMenu);

    reportMenu->menu->addAction(createAction("actionNewReport", "New Report", ":/images/document-new.png", "Ctrl+N", SLOT(slotRequestForNewReport())));
    reportMenu->menu->addAction(createAction("actionOpenReport", "Open Report", ":/images/document-open.png", "Ctrl+O", SLOT(slotRequestForOpenReport())));
    m_saveReportAction = createAction("actionSaveReport", "Save Report", ":/images/document-save.png", "Ctrl+S", SLOT(slotRequestForSaveReport()));
    reportMenu->menu->addAction(m_saveReportAction);
    m_saveReportAsAction = createAction("actionSaveAsReport", "Save Report As...", ":/images/document-save-as.png", "", SLOT(slotRequestForSaveReportAs()));
    reportMenu->menu->addAction(m_saveReportAsAction);
    m_closeReportAction = createAction("actionCloseReport", "Close Report", ":/images/document-close.png", "Ctrl+W", SLOT(slotRequestForCloseReport()));
    reportMenu->menu->addAction(m_closeReportAction);

    m_saveReportAction->setEnabled(false);
    m_closeReportAction->setEnabled(false);
    m_saveReportAsAction->setEnabled(false);

    return menus;
}


void ReportEditor::slotReportNameChangedOutside(QString name)
{
    Q_UNUSED(name);
    CuteReport::ReportInterface* report = dynamic_cast<CuteReport::ReportInterface*>(sender());

    int index = -1;
    for (int i = 0; i < m_reports.size(); ++i ) {
        if (m_reports.at(i).report == report) {
            index = i;
            break;
        }
    }

    if (index == -1)
        return;

    report->setObjectName(makeCorrectObjectName(report));
    ui.data()->changeTabText(index, report->objectName());

    m_reports.value(index).tabText = report->objectName();

    QString reportName = (report->name().isEmpty() ? "NoName" : report->name()) +  (report->isValid() ? "" : "(invalid)") + (report->isDirty() ? "*" : "");
    core()->setDocumentTitles(documentId, reportName);
}


void ReportEditor::slotDirtynessChanged(bool isDirty)
{
    Q_UNUSED(isDirty);
    CuteReport::ReportInterface* report = dynamic_cast<CuteReport::ReportInterface*>(sender());
    if (report != core()->currentReport())
        return;

    QString reportName = (report->name().isEmpty() ? "NoName" : report->name()) +  (report->isValid() ? "" : "(invalid)") + (report->isDirty() ? "*" : "");
    core()->setDocumentTitles(documentId, reportName);
}


void ReportEditor::slotCoreReportCreated(ReportInterface *report)
{
    if (!report)
        return;

    foreach (ReportStruct st, m_reports) {
        if (st.report == report)
            return;
    }

    if (CuteReport::ReportInterface* existsReport = sameReportExists(report)) {
        delete report;
        m_reportToSwitch = existsReport;
        QTimer::singleShot(0, this, SLOT(slotSetCurrentReport()));
    } else {
        newReportPreprocess(report);
        m_reportToSwitch = report;
        QTimer::singleShot(0, this, SLOT(slotSetCurrentReport()));
    }
}


void ReportEditor::slotSetCurrentReport()
{
    core()->setCurrentReport(m_reportToSwitch.data());
    /// do not set m_reportToSwitch to 0 because of issues of some sequential reports
}


void ReportEditor::slotDesignerInitDone()
{
    QStringList reportURLs = core()->getSettingValue("Designer/LastReportURLs").toString().split(";");
    foreach (const QString & url, reportURLs) {
    if (!url.isEmpty())
        loadReport(url);
    }
}


void ReportEditor::slotAppIsAboutToClose(bool* cancel)
{
    QStringList list;
    foreach (ReportStruct st, m_reports) {
        CuteReport::ReportInterface * report = st.report;
        if (report->isDirty() && report->isValid()) {
            int ret = QMessageBox::warning(core()->mainWindow(), tr("Cute Report"),
                                           tr("The document <b>\"%1\"</b> has been modified.<br>"
                                              "Do you want to save your changes?").arg(report->name().isEmpty() ? tr("NoName") : report->name()),
                                           QMessageBox::Yes | QMessageBox::No| QMessageBox::Cancel,
                                           QMessageBox::Yes);
            if (ret == QMessageBox::Cancel) {
                if (cancel) {
                    (*cancel) = true;
                    return;
                }
            } else if (ret == QMessageBox::Yes) {
                bool res = saveReport(report);
                if (!res && cancel) {
                    (*cancel) = true;
                    return;
                }
            }
        }
        list << report->fileUrl();
    }
    core()->setSettingValue("Designer/LastReportURLs", list.join(";"));
}


QString ReportEditor::makeCorrectObjectName(CuteReport::ReportInterface * report)
{
    if (report->name().isEmpty())
        return report->objectName();

    QString newObjectName = report->name();
    newObjectName.replace(QRegExp("\\W"), "_");
    newObjectName.replace(QRegExp("_{2,}"),"_");
    return newObjectName;
}


//suit_end_namespace

#if QT_VERSION < 0x050000
Q_EXPORT_PLUGIN2(ReportEditor, ReportEditor)
#endif
