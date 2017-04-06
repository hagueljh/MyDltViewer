/**
 * @licence app begin@
 * Copyright (C) 2011-2012  BMW AG
 *
 * This file is part of GENIVI Project Dlt Viewer.
 *
 * Contributions are licensed to the GENIVI Alliance under one or more
 * Contribution License Agreements.
 *
 * \copyright
 * This Source Code Form is subject to the terms of the
 * Mozilla Public License, v. 2.0. If a  copy of the MPL was not distributed with
 * this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * \file mainwindow.h
 * For further information see http://www.genivi.org/.
 * @licence end@
 */

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QFile>
#include <QTimer>
#include <QDir>
#include <QShortcut>
#include <QMessageBox>
#include <QColor>
#include <QProgressBar>

#include "tablemodel.h"
#include "project.h"
#include "settingsdialog.h"
#include "searchdialog.h"
#include "optmanager.h"
#include "qdlt.h"
#include "dltsettingsmanager.h"
#include "filterdialog.h"
#include "dltfileindexer.h"
#include "workingdirectory.h"
#include "exporterdialog.h"
#include <QCompleter>
#include "searchtablemodel.h"
#include "ui_mainwindow.h"
#include <QStringListModel>

/**
 * When ecu items buffer size exceeds this while using
 * serial connection, it will be considered corrupted.
 **/

#define DLT_BUFFER_CORRUPT_TRESHOLD 4* 1024

/**
 * @brief Namespace to contain the toolbar positions.
 * You should always remember to update these enums if you
 * add, remove or re-arrange actions in the toolbars!
 * If you add separators, please add "Separator" lines similar
 * to those below, to make it obvious why the numbering scheme is
 * leaping over some numbers.
 */
namespace ToolbarPosition {
    /**
     * @brief Main Toolbar button positions.
     * These are static thorough the program run time.
     */
    enum MainToolbarPosition {
        NewDltFile      = 0,
        OpenDltFile     = 1,
        Clear           = 2,
        SaveDltFile     = 3,
        SaveProjectFile = 4,
        // Separator    = 5,
        ConnectAll      = 6,
        DisconnectAll   = 7,
        // Separator    = 8,
        Settings        = 9,
        // Separator    = 10,
        AutoScroll      = 11
    };

    /**
     * @brief Search Toolbar button positions
     * Please note! Find Previos/Next will be shifted
     * right by one position after constructor, because
     * the textbox is added! So, if you want to use this
     * enum to identify these actions later, you need to use
     * SearchToolbarPosition::FindXXX + 1.
     */
    enum SearchToolbarPosition {
        Search          = 0,
        Regexp          = 1,
        SearchList      = 2,
        // Separator    = 3,
        FindPrevious    = 4,
        FindNext        = 5
    };
}

extern "C"
{
#include "dlt_common.h"
}

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    QCompleter *newCompleter;

private:
    Ui::MainWindow *ui;

    /* Timer for connecting to ECUs */
    QTimer timer;

    /* Timer for draw Event */
    QTimer draw_timer;
    int draw_interval;

    QDltControl qcontrol;
    QFile outputfile;
    bool outputfileIsTemporary;
    bool outputfileIsFromCLI;
    TableModel *tableModel;
    SearchTableModel *m_searchtableModel;
    WorkingDirectory workingDirectory;

    /* Status line items */
    QLabel *statusFilename;
    QLabel *statusFileVersion;
    QLabel *statusBytesReceived;
    QLabel *statusByteErrorsReceived;
    QLabel *statusSyncFoundReceived;
    QProgressBar *statusProgressBar;

    unsigned long totalBytesRcvd;
    unsigned long totalByteErrorsRcvd;
    unsigned long totalSyncFoundRcvd;

    /* Search */
    SearchDialog *searchDlg;
    QShortcut *m_shortcut_searchnext;
    QShortcut *m_shortcut_searchprev;

    /* Export */
    ExporterDialog exporterDialog;

    /* Settings dialog containing also the settings parameter itself */
    SettingsDialog *settings;
    QLineEdit *searchTextbox;

    /* injections */
    QString injectionAplicationId;
    QString injectionContextId;
    QString injectionServiceId;
    QString injectionData;
    bool injectionDataBinary;

    /* Toggle buttons */
    QAction *scrollButton;

    QList<QAction *> m_searchActions;

    QTableView *m_searchresultsTable;

    /* Autoload plugins configuration */
    QStringList autoloadPluginsVersionEcus;
    QStringList autoloadPluginsVersionStrings;

    /* String List Containing Search History */
    enum { MaxSearchHistory = 20 };
    QAction *searchHistoryActs[MaxSearchHistory];
    QStringList searchHistory;


    /* Recent files */
    enum { MaxRecentFiles = 5 };
    QAction *recentFileActs[MaxRecentFiles];
    QStringList recentFiles;

    /* Recent projects */
    enum { MaxRecentProjects = 5 };
    QAction *recentProjectActs[MaxRecentProjects];
    QStringList recentProjects;

    /* Recent filters */
    enum { MaxRecentFilters = 5 };
    QAction *recentFiltersActs[MaxRecentFilters];
    QStringList recentFilters;

    /* Recent hostnames and ports */
    enum { MaxRecentHostnames = 10 };
    QStringList recentHostnames;
    enum { MaxRecentPorts = 10 };
    QStringList recentPorts;

    /* dlt-file Indexer with cancel cabability */
    DltFileIndexer *dltIndexer;

    /* Color for blinking 'Apply changes'-button */
    QColor pulseButtonColor;

    bool isSearchOngoing;

    /* DLT File opened only Read only */
    bool isDltFileReadOnly;

    /* functions called in constructor */
    void initState();
    void initView();
    void initSignalConnections();
    void initFileHandling();
    void initSearchTable();

    /* general functions */

    void getSelectedItems(EcuItem **ecuitem,ApplicationItem** appitem,ContextItem** conitem);

    /**
        * @brief Reload the complete log file
        * @param update if this parameter is false, the file is loaded the first time, if true the reload is performed because of a changed configuration
        *
        */
    void reloadLogFileStop();
    void reloadLogFile(bool update=false, bool multithreaded = true);

    void reloadLogFileDefaultFilter();

    void exportSelection(bool ascii,bool file);
    void exportSelection_searchTable();

    void ControlServiceRequest(EcuItem* ecuitem, int service_id );
    void SendInjection(EcuItem* ecuitem);

    void controlMessage_SendControlMessage(EcuItem* ecuitem,DltMessage &msg, QString appid, QString contid);
    void controlMessage_WriteControlMessage(DltMessage &msg, QString appid, QString contid);
    void controlMessage_SetLogLevel(EcuItem* ecuitem, QString app, QString con,int log_level);
    void controlMessage_SetDefaultLogLevel(EcuItem* ecuitem, int status);
    void controlMessage_SetTraceStatus(EcuItem* ecuitem,QString app, QString con,int status);
    void controlMessage_SetDefaultTraceStatus(EcuItem* ecuitem, int status);
    void controlMessage_SetVerboseMode(EcuItem* ecuitem, int mode);
    void controlMessage_SetTimingPackets(EcuItem* ecuitem, bool enable);
    void controlMessage_GetSoftwareVersion(EcuItem* ecuitem);
    void controlMessage_GetLogInfo(EcuItem* ecuitem);
    void controlMessage_ReceiveControlMessage(EcuItem *ecuitem,QDltMsg &msg);
    void controlMessage_SetContext(EcuItem *ecuitem, QString apid, QString ctid,QString ctdescription,int log_level,int trace_status);
    void controlMessage_SetApplication(EcuItem *ecuitem, QString apid, QString appdescription);
    void controlMessage_Marker();

    void filterDialogRead(FilterDialog &dlg,FilterItem* item);
    void filterDialogWrite(FilterDialog &dlg,FilterItem* item);
    void filterUpdate();

    void loadPlugins();
    void updatePluginsECUList();
    void updatePlugins();
    void updatePlugin(PluginItem *item);
    void contextLoadingFile(QDltMsg &msg);
    void versionString(QDltMsg &msg);
    void pluginsAutoload(QString version);

    void connectECU(EcuItem *ecuitem,bool force = false);
    void disconnectECU(EcuItem *ecuitem);
    void checkConnectionState();
    void read(EcuItem *ecuitem);
    void updateIndex();
    void drawUpdatedView();

    void updateRecentFileActions();
    void setCurrentFile(const QString &fileName);
    void removeCurrentFile(const QString &fileName);

    void updateRecentProjectActions();
    void setCurrentProject(const QString &projectName);
    void removeCurrentProject(const QString &projectName);

    void updateRecentFiltersActions();
    void setCurrentFilters(const QString &filtersName);
    void removeCurrentFilters(const QString &filtersName);

    void setCurrentHostname(const QString &hostName);
    void setCurrentPort(const QString &portName);

    void sendUpdates(EcuItem* ecuitem);

    bool anyPluginsEnabled();
    bool anyFiltersEnabled();

    bool openDltFile(QStringList fileName);
    bool openDlpFile(QString filename);
    bool openDlfFile(QString filename, bool replace);

    void commandLineConvertToASCII();
    void commandLineConvertToUTF8();

    void commandLineExecutePlugin(QString name, QString cmd, QStringList params);

    void iterateDecodersForMsg(QDltMsg &, int triggeredByUser);

    QStringList getSerialPortsWithQextEnumerator();

    void deleteactualFile();

    int nearest_line(int line);
    bool jump_to_line(int line);

 /**
     * @brief ErrorMessage
     * @param String which is shown as MessageBox, if we are not in silent mode.
     *  In Silent Mode, it is printed on the console, which only works in Windows DEbug or under Linux Shells
     *
     */
    void ErrorMessage(QMessageBox::Icon level, QString title,  QString message);

    void clearSelection();
    void saveSelection();
    void restoreSelection();
    QList<int> previousSelection;

    /* Disconnect and Reconnect serial connections */
    QList<int> m_previouslyConnectedSerialECUs;
    void saveAndDisconnectCurrentlyConnectedSerialECUs();
    void connectPreviouslyConnectedECUs();

    /* default filters */
    void resetDefaultFilter();

    QStringListModel m_CompleterModel;

protected:
    void keyPressEvent ( QKeyEvent * event );
    void dragEnterEvent(QDragEnterEvent *event);
    void dropEvent(QDropEvent *event);
    void closeEvent(QCloseEvent *event);

private slots:
    void reloadLogFileProgressMax(quint64 num);
    void reloadLogFileProgress(quint64 num);
    void reloadLogFileProgressText(QString text);
    void reloadLogFileVersionString(QString ecuId, QString version);
    void reloadLogFileFinishIndex();
    void reloadLogFileFinishFilter();
    void reloadLogFileFinishDefaultFilter();

    void on_tableView_selectionChanged(const QItemSelection & selected, const QItemSelection & deselected);

    void on_tableView_customContextMenuRequested(QPoint pos);
    void on_tableView_SearchIndex_customContextMenuRequested(QPoint pos);
    void on_pluginWidget_customContextMenuRequested(QPoint pos);
    void on_filterWidget_customContextMenuRequested(QPoint pos);
    void on_configWidget_customContextMenuRequested(QPoint pos);

    void on_configWidget_itemSelectionChanged();
    void on_pluginWidget_itemSelectionChanged();
    void on_filterWidget_itemSelectionChanged();

    void on_filterWidget_itemClicked(QTreeWidgetItem *item, int column);

    void on_pluginWidget_itemExpanded(QTreeWidgetItem* item);

// File methods

private slots:

    void on_action_menuFile_New_triggered();
    void on_action_menuFile_SaveAs_triggered();
    void on_action_menuFile_Import_DLT_Stream_with_Serial_Header_triggered();
    void on_action_menuFile_Append_DLT_File_triggered();
    void on_action_menuFile_Import_DLT_Stream_triggered();
    void on_action_menuFile_Settings_triggered();
    void on_action_menuFile_Open_triggered();
    void on_actionExport_triggered();

public slots:

    void on_New_triggered(QString fileName);
    void on_Open_triggered(QStringList filenames);
    void on_SaveAs_triggered(QString fileName);
    void on_action_menuFile_Clear_triggered();
    void on_action_menuFile_Quit_triggered();
    void on_action_FindNext();
private slots:

    // Search methods
    void on_action_menuSearch_Find_triggered();

    void searchTableRenewed();
    void searchtable_cellSelected(QModelIndex index);

    // Project methods
    void on_action_menuProject_Save_triggered();
    void on_action_menuProject_Open_triggered();
    void on_action_menuProject_New_triggered();

    // Help methods
    void on_action_menuHelp_Support_triggered();
    void on_action_menuHelp_Info_triggered();
    void on_action_menuHelp_Command_Line_triggered();

    // Config methods
    void on_action_menuConfig_Context_Delete_triggered();
    void on_action_menuConfig_Context_Edit_triggered();
    void on_action_menuConfig_Context_Add_triggered();
    void on_action_menuConfig_Application_Delete_triggered();
    void on_action_menuConfig_Application_Edit_triggered();
    void on_action_menuConfig_Application_Add_triggered();
    void on_action_menuConfig_ECU_Edit_triggered();
    void on_action_menuConfig_ECU_Delete_triggered();
    void on_action_menuConfig_ECU_Add_triggered();
    void on_action_menuConfig_Collapse_All_ECUs_triggered();
    void on_action_menuConfig_Expand_All_ECUs_triggered();
    void on_action_menuConfig_Disconnect_triggered();
    void on_action_menuConfig_Connect_triggered();
    void on_action_menuConfig_Delete_All_Contexts_triggered();
    void on_action_menuConfig_Copy_to_clipboard_triggered();
    void on_action_menuConfig_SearchTable_Copy_to_clipboard_triggered();

    // DLT methods
    void on_action_menuDLT_Send_Injection_triggered();
    void on_action_menuDLT_Get_Local_Time_2_triggered();
    void on_action_menuDLT_Get_Software_Version_triggered();
    void on_action_menuDLT_Reset_to_Factory_Default_triggered();
    void on_action_menuDLT_Store_Config_triggered();
    void on_action_menuDLT_Set_All_Log_Levels_triggered();
    void on_action_menuDLT_Set_Log_Level_triggered();
    void on_action_menuDLT_Set_Default_Log_Level_triggered();
    void on_action_menuDLT_Get_Default_Log_Level_triggered();
    void on_action_menuDLT_Get_Log_Info_triggered();
    void on_action_menuDLT_Edit_All_Log_Levels_triggered();

    // Filter methods
    void on_action_menuFilter_Load_triggered();
    void on_action_menuFilter_Save_As_triggered();
    void on_action_menuFilter_Delete_triggered();
    void on_action_menuFilter_Edit_triggered();
    void on_action_menuFilter_Add_triggered();
    void on_action_menuFilter_Clear_all_triggered();
    void on_action_menuFilter_Duplicate_triggered();
    void on_action_menuFilter_Append_Filters_triggered();

    // Plugin methods
    void on_action_menuPlugin_Hide_triggered();
    void on_action_menuPlugin_Show_triggered();
    void on_action_menuPlugin_Edit_triggered();
    void action_menuPlugin_Enable_triggered();
    void on_action_menuPlugin_Disable_triggered();

    //Rename
    void filterAdd();
    void filterAddTable();
    void connected();
    void disconnected();
    void error(QAbstractSocket::SocketError);
    void readyRead();
    void timeout();
    void draw_timeout();
    void connectAll();
    void disconnectAll();
    void applySettings();
    void updateScrollButton();
    void openRecentFile();
    void openRecentProject();
    void openRecentFilters();
    void applyConfigEnabled(bool enabled);
    void tableViewValueChanged(int value);
    void stateChangedTCP(QAbstractSocket::SocketState socketState);
    void stateChangedSerial(bool dsrChanged);
    void sectionInTableDoubleClicked(int logicalIndex);
    void on_actionJump_To_triggered();
    void on_actionAutoScroll_triggered(bool checked);
    void on_actionConnectAll_triggered();
    void on_actionDisconnectAll_triggered();

    // Config Items
    void on_pluginsEnabled_toggled(bool checked);
    void on_filtersEnabled_toggled(bool checked);
    void on_applyConfig_clicked();
    void on_tabWidget_currentChanged(int index);

    void on_comboBoxFilterSelection_activated(const QString &arg1);

    void on_actionDefault_Filter_Reload_triggered();

    void on_actionDefault_Filter_Create_Index_triggered();

    void on_pushButtonDefaultFilterUpdateCache_clicked();

    void on_checkBoxSortByTime_toggled(bool checked);

    void on_actionMarker_triggered();

    void on_actionApply_Configuration_triggered(bool checked);
    void on_actionToggle_PluginsEnabled_triggered(bool checked);
    void on_actionToggle_FiltersEnabled_triggered(bool checked);
    void on_actionToggle_SortByTimeEnabled_triggered(bool checked);


public slots:

    void sendInjection(int index,QString applicationId,QString contextId,int serviceId,QByteArray data);
    void filterOrderChanged();
    void jumpToMsgSignal(int index);
    void markerSignal();
    void connectEcuSignal(int index);
    void disconnectEcuSignal(int index);
    void reopenFileSignal();

    void controlMessage_Timezone(int timezone, unsigned char dst);
    void controlMessage_UnregisterContext(QString ecuId,QString appId,QString ctId);

    //History Slots
    void onAddActionToHistory();
    void onSearchProgressChanged(bool isInProgress);

public:

    /* Project configuration containing ECU/APP/Context/Filter/Plugin configuration */
    Project project;

    /* DLT file handling */
    QDltFile qfile;

    /* Loading and handling all plugins */
    QDltPluginManager pluginManager;

    QDltDefaultFilter defaultFilter;

    QStringList openFileNames;
    QStringList list;

    /* store startLoggingDateTime when logging first data */
    QDateTime startLoggingDateTime;


};

#endif // MAINWINDOW_H