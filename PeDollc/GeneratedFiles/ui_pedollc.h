/********************************************************************************
** Form generated from reading UI file 'pedollc.ui'
**
** Created by: Qt User Interface Compiler version 4.8.6
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_PEDOLLC_H
#define UI_PEDOLLC_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QGroupBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QListWidget>
#include <QtGui/QMainWindow>
#include <QtGui/QMenu>
#include <QtGui/QMenuBar>
#include <QtGui/QProgressBar>
#include <QtGui/QPushButton>
#include <QtGui/QRadioButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QSpinBox>
#include <QtGui/QStatusBar>
#include <QtGui/QTabWidget>
#include <QtGui/QTableWidget>
#include <QtGui/QTextEdit>
#include <QtGui/QToolBar>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_PeDollcClass
{
public:
    QAction *action_Menu_MonitorConnector;
    QAction *action_Menu_Exit;
    QAction *action_Menu_ClearHook;
    QAction *action_Menu_ClearFilter;
    QAction *action_4;
    QAction *action_API;
    QAction *action_API_2;
    QAction *action_5;
    QAction *action_6;
    QAction *action_PeDoll;
    QAction *action_menu_regist;
    QAction *action_Tool_ControllerConnect;
    QAction *action_Tool_DollConnect;
    QAction *action_Tool_Run;
    QAction *action_Tool_Stop;
    QAction *action_Tool_Hook;
    QAction *actionLoad_menu_hookregulars;
    QAction *action_tool_Clear;
    QAction *action_tool_Tab;
    QAction *actionControllerClose;
    QAction *actionKill_Virtual_Machine_Detector;
    QAction *action_Menu_VMWare_vmx_Anti_virtualization;
    QAction *actionHexToDex;
    QWidget *centralWidget;
    QVBoxLayout *verticalLayout;
    QTabWidget *tabWidget;
    QWidget *tab;
    QVBoxLayout *verticalLayout_3;
    QTextEdit *textEdit_Monitor_Messages;
    QWidget *tab_9;
    QVBoxLayout *verticalLayout_6;
    QTextEdit *textEdit_Query;
    QHBoxLayout *horizontalLayout_2;
    QPushButton *pushButton_QueryPass;
    QPushButton *pushButton_QueryReject;
    QPushButton *pushButton_QueryTerminate;
    QWidget *tab_2;
    QVBoxLayout *verticalLayout_4;
    QTextEdit *textEdit_Hook_Regular;
    QWidget *tab_3;
    QVBoxLayout *verticalLayout_5;
    QTextEdit *textEdit_Functions_Filter;
    QWidget *tab_5;
    QVBoxLayout *verticalLayout_9;
    QTextEdit *textEdit_Files;
    QWidget *tab_4;
    QVBoxLayout *verticalLayout_2;
    QTextEdit *textEdit_Windows_Operator;
    QWidget *tab_7;
    QVBoxLayout *verticalLayout_8;
    QTextEdit *textEdit_Processes;
    QWidget *tab_6;
    QVBoxLayout *verticalLayout_7;
    QTextEdit *textEdit_Register;
    QWidget *Network;
    QVBoxLayout *verticalLayout_11;
    QTextEdit *textEdit_Network;
    QWidget *tab_11;
    QVBoxLayout *verticalLayout_17;
    QHBoxLayout *horizontalLayout_15;
    QSpacerItem *horizontalSpacer_2;
    QLabel *label_3;
    QLineEdit *lineEdit_Mem_Addr;
    QLabel *label_4;
    QSpinBox *spinBox_Mem_Size;
    QPushButton *pushButton_Mem_Dump;
    QProgressBar *progressBar_MemPro;
    QHBoxLayout *horizontalLayout_16;
    QTextEdit *textEdit_MemHex;
    QTextEdit *textEdit_MemASC;
    QGroupBox *groupBox_3;
    QVBoxLayout *verticalLayout_16;
    QHBoxLayout *horizontalLayout_17;
    QRadioButton *radioButton_Mem_GBK;
    QRadioButton *radioButton_Mem_UTF16;
    QRadioButton *radioButton_Mem_UTF8;
    QHBoxLayout *horizontalLayout_18;
    QRadioButton *radioButton_Mem_ASM64;
    QRadioButton *radioButton_Mem_ASM86;
    QRadioButton *radioButton_Mem_ASM16;
    QPushButton *pushButton_MemorySaveToFile;
    QWidget *tab_8;
    QHBoxLayout *horizontalLayout_7;
    QWidget *widget_DumpRegOpen;
    QHBoxLayout *horizontalLayout_5;
    QVBoxLayout *verticalLayout_10;
    QListWidget *listWidget_Dump;
    QGroupBox *groupBox;
    QVBoxLayout *verticalLayout_15;
    QHBoxLayout *horizontalLayout_9;
    QRadioButton *radioButton_GBK;
    QRadioButton *radioButton_UTF16;
    QRadioButton *radioButton_UTF8;
    QHBoxLayout *horizontalLayout_4;
    QRadioButton *radioButton_ASM64;
    QRadioButton *radioButton_ASM86;
    QRadioButton *radioButton_ASM16;
    QHBoxLayout *horizontalLayout_3;
    QLabel *label;
    QSpinBox *spinBox_DumpMaxSize;
    QLabel *label_2;
    QSpinBox *spinBox_DumpMaxCount;
    QHBoxLayout *horizontalLayout;
    QPushButton *pushButton_DumpClear;
    QPushButton *pushButton_DumpDelete;
    QPushButton *pushButton_DumpSave;
    QPushButton *pushButton_DumpSaveAll;
    QTextEdit *textEdit_NetworkHex;
    QVBoxLayout *verticalLayout_12;
    QTextEdit *textEdit_NetworkASC;
    QLabel *label_RegNotice;
    QWidget *tab_10;
    QHBoxLayout *horizontalLayout_13;
    QWidget *widget_StackPanel;
    QHBoxLayout *horizontalLayout_14;
    QVBoxLayout *verticalLayout_14;
    QHBoxLayout *horizontalLayout_12;
    QLabel *label_ESP;
    QSpacerItem *horizontalSpacer_3;
    QLineEdit *lineEdit_ESP_Addr;
    QPushButton *pushButton_StackSearch;
    QTableWidget *tableWidget_Stack;
    QGroupBox *groupBox_2;
    QHBoxLayout *horizontalLayout_8;
    QRadioButton *radioButton_Stack_GBK;
    QRadioButton *radioButton_Stack_UTF16;
    QRadioButton *radioButton_Stack_UTF8;
    QHBoxLayout *horizontalLayout_11;
    QSpacerItem *horizontalSpacer;
    QPushButton *pushButton_StackClear;
    QPushButton *pushButton_StackUpdate;
    QVBoxLayout *verticalLayout_13;
    QHBoxLayout *horizontalLayout_10;
    QTextEdit *textEdit_StackHex;
    QTextEdit *textEdit_StackASC;
    QLabel *label_StackRegNotice;
    QHBoxLayout *horizontalLayout_6;
    QLineEdit *lineEdit_CommandLine;
    QPushButton *pushButton_Enter;
    QPushButton *pushButton_Switch;
    QMenuBar *menuBar;
    QMenu *menu;
    QMenu *menu_2;
    QMenu *menu_4;
    QMenu *menuTools;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *PeDollcClass)
    {
        if (PeDollcClass->objectName().isEmpty())
            PeDollcClass->setObjectName(QString::fromUtf8("PeDollcClass"));
        PeDollcClass->resize(1136, 617);
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/PeDollc/Resources/binary.png"), QSize(), QIcon::Normal, QIcon::Off);
        PeDollcClass->setWindowIcon(icon);
        action_Menu_MonitorConnector = new QAction(PeDollcClass);
        action_Menu_MonitorConnector->setObjectName(QString::fromUtf8("action_Menu_MonitorConnector"));
        action_Menu_Exit = new QAction(PeDollcClass);
        action_Menu_Exit->setObjectName(QString::fromUtf8("action_Menu_Exit"));
        action_Menu_ClearHook = new QAction(PeDollcClass);
        action_Menu_ClearHook->setObjectName(QString::fromUtf8("action_Menu_ClearHook"));
        action_Menu_ClearFilter = new QAction(PeDollcClass);
        action_Menu_ClearFilter->setObjectName(QString::fromUtf8("action_Menu_ClearFilter"));
        action_4 = new QAction(PeDollcClass);
        action_4->setObjectName(QString::fromUtf8("action_4"));
        action_API = new QAction(PeDollcClass);
        action_API->setObjectName(QString::fromUtf8("action_API"));
        action_API_2 = new QAction(PeDollcClass);
        action_API_2->setObjectName(QString::fromUtf8("action_API_2"));
        action_5 = new QAction(PeDollcClass);
        action_5->setObjectName(QString::fromUtf8("action_5"));
        action_6 = new QAction(PeDollcClass);
        action_6->setObjectName(QString::fromUtf8("action_6"));
        action_PeDoll = new QAction(PeDollcClass);
        action_PeDoll->setObjectName(QString::fromUtf8("action_PeDoll"));
        action_menu_regist = new QAction(PeDollcClass);
        action_menu_regist->setObjectName(QString::fromUtf8("action_menu_regist"));
        action_Tool_ControllerConnect = new QAction(PeDollcClass);
        action_Tool_ControllerConnect->setObjectName(QString::fromUtf8("action_Tool_ControllerConnect"));
        QIcon icon1;
        icon1.addFile(QString::fromUtf8(":/PeDollc/Resources/connect.png"), QSize(), QIcon::Normal, QIcon::Off);
        action_Tool_ControllerConnect->setIcon(icon1);
        action_Tool_DollConnect = new QAction(PeDollcClass);
        action_Tool_DollConnect->setObjectName(QString::fromUtf8("action_Tool_DollConnect"));
        QIcon icon2;
        icon2.addFile(QString::fromUtf8(":/PeDollc/Resources/linker.png"), QSize(), QIcon::Normal, QIcon::Off);
        action_Tool_DollConnect->setIcon(icon2);
        action_Tool_Run = new QAction(PeDollcClass);
        action_Tool_Run->setObjectName(QString::fromUtf8("action_Tool_Run"));
        QIcon icon3;
        icon3.addFile(QString::fromUtf8(":/PeDollc/Resources/run.png"), QSize(), QIcon::Normal, QIcon::Off);
        action_Tool_Run->setIcon(icon3);
        action_Tool_Stop = new QAction(PeDollcClass);
        action_Tool_Stop->setObjectName(QString::fromUtf8("action_Tool_Stop"));
        QIcon icon4;
        icon4.addFile(QString::fromUtf8(":/PeDollc/Resources/stop.png"), QSize(), QIcon::Normal, QIcon::Off);
        action_Tool_Stop->setIcon(icon4);
        action_Tool_Hook = new QAction(PeDollcClass);
        action_Tool_Hook->setObjectName(QString::fromUtf8("action_Tool_Hook"));
        QIcon icon5;
        icon5.addFile(QString::fromUtf8(":/PeDollc/Resources/Hook.png"), QSize(), QIcon::Normal, QIcon::Off);
        action_Tool_Hook->setIcon(icon5);
        actionLoad_menu_hookregulars = new QAction(PeDollcClass);
        actionLoad_menu_hookregulars->setObjectName(QString::fromUtf8("actionLoad_menu_hookregulars"));
        action_tool_Clear = new QAction(PeDollcClass);
        action_tool_Clear->setObjectName(QString::fromUtf8("action_tool_Clear"));
        QIcon icon6;
        icon6.addFile(QString::fromUtf8(":/PeDollc/Resources/Clear.png"), QSize(), QIcon::Normal, QIcon::Off);
        action_tool_Clear->setIcon(icon6);
        action_tool_Tab = new QAction(PeDollcClass);
        action_tool_Tab->setObjectName(QString::fromUtf8("action_tool_Tab"));
        QIcon icon7;
        icon7.addFile(QString::fromUtf8(":/PeDollc/Resources/Tab.png"), QSize(), QIcon::Normal, QIcon::Off);
        action_tool_Tab->setIcon(icon7);
        actionControllerClose = new QAction(PeDollcClass);
        actionControllerClose->setObjectName(QString::fromUtf8("actionControllerClose"));
        QIcon icon8;
        icon8.addFile(QString::fromUtf8(":/PeDollc/Resources/Close.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionControllerClose->setIcon(icon8);
        actionKill_Virtual_Machine_Detector = new QAction(PeDollcClass);
        actionKill_Virtual_Machine_Detector->setObjectName(QString::fromUtf8("actionKill_Virtual_Machine_Detector"));
        actionKill_Virtual_Machine_Detector->setEnabled(false);
        action_Menu_VMWare_vmx_Anti_virtualization = new QAction(PeDollcClass);
        action_Menu_VMWare_vmx_Anti_virtualization->setObjectName(QString::fromUtf8("action_Menu_VMWare_vmx_Anti_virtualization"));
        actionHexToDex = new QAction(PeDollcClass);
        actionHexToDex->setObjectName(QString::fromUtf8("actionHexToDex"));
        centralWidget = new QWidget(PeDollcClass);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
        verticalLayout = new QVBoxLayout(centralWidget);
        verticalLayout->setSpacing(0);
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        tabWidget = new QTabWidget(centralWidget);
        tabWidget->setObjectName(QString::fromUtf8("tabWidget"));
        tabWidget->setTabPosition(QTabWidget::South);
        tabWidget->setDocumentMode(false);
        tab = new QWidget();
        tab->setObjectName(QString::fromUtf8("tab"));
        verticalLayout_3 = new QVBoxLayout(tab);
        verticalLayout_3->setSpacing(0);
        verticalLayout_3->setContentsMargins(0, 0, 0, 0);
        verticalLayout_3->setObjectName(QString::fromUtf8("verticalLayout_3"));
        textEdit_Monitor_Messages = new QTextEdit(tab);
        textEdit_Monitor_Messages->setObjectName(QString::fromUtf8("textEdit_Monitor_Messages"));
        textEdit_Monitor_Messages->setStyleSheet(QString::fromUtf8("QTextEdit{background-color: rgb(0, 0, 0);}"));
        textEdit_Monitor_Messages->setTabChangesFocus(false);
        textEdit_Monitor_Messages->setUndoRedoEnabled(false);
        textEdit_Monitor_Messages->setReadOnly(true);

        verticalLayout_3->addWidget(textEdit_Monitor_Messages);

        tabWidget->addTab(tab, QString());
        tab_9 = new QWidget();
        tab_9->setObjectName(QString::fromUtf8("tab_9"));
        verticalLayout_6 = new QVBoxLayout(tab_9);
        verticalLayout_6->setSpacing(6);
        verticalLayout_6->setContentsMargins(11, 11, 11, 11);
        verticalLayout_6->setObjectName(QString::fromUtf8("verticalLayout_6"));
        textEdit_Query = new QTextEdit(tab_9);
        textEdit_Query->setObjectName(QString::fromUtf8("textEdit_Query"));
        textEdit_Query->setStyleSheet(QString::fromUtf8("QTextEdit{background-color: rgb(0, 0, 0);}"));
        textEdit_Query->setTabChangesFocus(false);
        textEdit_Query->setUndoRedoEnabled(false);
        textEdit_Query->setReadOnly(true);

        verticalLayout_6->addWidget(textEdit_Query);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setSpacing(6);
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        pushButton_QueryPass = new QPushButton(tab_9);
        pushButton_QueryPass->setObjectName(QString::fromUtf8("pushButton_QueryPass"));

        horizontalLayout_2->addWidget(pushButton_QueryPass);

        pushButton_QueryReject = new QPushButton(tab_9);
        pushButton_QueryReject->setObjectName(QString::fromUtf8("pushButton_QueryReject"));

        horizontalLayout_2->addWidget(pushButton_QueryReject);

        pushButton_QueryTerminate = new QPushButton(tab_9);
        pushButton_QueryTerminate->setObjectName(QString::fromUtf8("pushButton_QueryTerminate"));

        horizontalLayout_2->addWidget(pushButton_QueryTerminate);


        verticalLayout_6->addLayout(horizontalLayout_2);

        tabWidget->addTab(tab_9, QString());
        tab_2 = new QWidget();
        tab_2->setObjectName(QString::fromUtf8("tab_2"));
        verticalLayout_4 = new QVBoxLayout(tab_2);
        verticalLayout_4->setSpacing(0);
        verticalLayout_4->setContentsMargins(0, 0, 0, 0);
        verticalLayout_4->setObjectName(QString::fromUtf8("verticalLayout_4"));
        textEdit_Hook_Regular = new QTextEdit(tab_2);
        textEdit_Hook_Regular->setObjectName(QString::fromUtf8("textEdit_Hook_Regular"));
        textEdit_Hook_Regular->setStyleSheet(QString::fromUtf8("QTextEdit{background-color: rgb(0, 0, 0);}"));
        textEdit_Hook_Regular->setTabChangesFocus(false);
        textEdit_Hook_Regular->setUndoRedoEnabled(false);
        textEdit_Hook_Regular->setReadOnly(true);

        verticalLayout_4->addWidget(textEdit_Hook_Regular);

        tabWidget->addTab(tab_2, QString());
        tab_3 = new QWidget();
        tab_3->setObjectName(QString::fromUtf8("tab_3"));
        verticalLayout_5 = new QVBoxLayout(tab_3);
        verticalLayout_5->setSpacing(0);
        verticalLayout_5->setContentsMargins(0, 0, 0, 0);
        verticalLayout_5->setObjectName(QString::fromUtf8("verticalLayout_5"));
        textEdit_Functions_Filter = new QTextEdit(tab_3);
        textEdit_Functions_Filter->setObjectName(QString::fromUtf8("textEdit_Functions_Filter"));
        textEdit_Functions_Filter->setStyleSheet(QString::fromUtf8("QTextEdit{background-color: rgb(0, 0, 0);}"));
        textEdit_Functions_Filter->setTabChangesFocus(false);
        textEdit_Functions_Filter->setUndoRedoEnabled(false);
        textEdit_Functions_Filter->setReadOnly(true);

        verticalLayout_5->addWidget(textEdit_Functions_Filter);

        tabWidget->addTab(tab_3, QString());
        tab_5 = new QWidget();
        tab_5->setObjectName(QString::fromUtf8("tab_5"));
        verticalLayout_9 = new QVBoxLayout(tab_5);
        verticalLayout_9->setSpacing(0);
        verticalLayout_9->setContentsMargins(0, 0, 0, 0);
        verticalLayout_9->setObjectName(QString::fromUtf8("verticalLayout_9"));
        textEdit_Files = new QTextEdit(tab_5);
        textEdit_Files->setObjectName(QString::fromUtf8("textEdit_Files"));
        textEdit_Files->setStyleSheet(QString::fromUtf8("QTextEdit{background-color: rgb(0, 0, 0);}"));
        textEdit_Files->setTabChangesFocus(false);
        textEdit_Files->setUndoRedoEnabled(false);
        textEdit_Files->setReadOnly(true);

        verticalLayout_9->addWidget(textEdit_Files);

        tabWidget->addTab(tab_5, QString());
        tab_4 = new QWidget();
        tab_4->setObjectName(QString::fromUtf8("tab_4"));
        verticalLayout_2 = new QVBoxLayout(tab_4);
        verticalLayout_2->setSpacing(0);
        verticalLayout_2->setContentsMargins(0, 0, 0, 0);
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        textEdit_Windows_Operator = new QTextEdit(tab_4);
        textEdit_Windows_Operator->setObjectName(QString::fromUtf8("textEdit_Windows_Operator"));
        textEdit_Windows_Operator->setStyleSheet(QString::fromUtf8("QTextEdit{background-color: rgb(0, 0, 0);}"));
        textEdit_Windows_Operator->setTabChangesFocus(false);
        textEdit_Windows_Operator->setUndoRedoEnabled(false);
        textEdit_Windows_Operator->setReadOnly(true);

        verticalLayout_2->addWidget(textEdit_Windows_Operator);

        tabWidget->addTab(tab_4, QString());
        tab_7 = new QWidget();
        tab_7->setObjectName(QString::fromUtf8("tab_7"));
        verticalLayout_8 = new QVBoxLayout(tab_7);
        verticalLayout_8->setSpacing(0);
        verticalLayout_8->setContentsMargins(0, 0, 0, 0);
        verticalLayout_8->setObjectName(QString::fromUtf8("verticalLayout_8"));
        textEdit_Processes = new QTextEdit(tab_7);
        textEdit_Processes->setObjectName(QString::fromUtf8("textEdit_Processes"));
        textEdit_Processes->setStyleSheet(QString::fromUtf8("QTextEdit{background-color: rgb(0, 0, 0);}"));
        textEdit_Processes->setTabChangesFocus(false);
        textEdit_Processes->setUndoRedoEnabled(false);
        textEdit_Processes->setReadOnly(true);

        verticalLayout_8->addWidget(textEdit_Processes);

        tabWidget->addTab(tab_7, QString());
        tab_6 = new QWidget();
        tab_6->setObjectName(QString::fromUtf8("tab_6"));
        verticalLayout_7 = new QVBoxLayout(tab_6);
        verticalLayout_7->setSpacing(0);
        verticalLayout_7->setContentsMargins(0, 0, 0, 0);
        verticalLayout_7->setObjectName(QString::fromUtf8("verticalLayout_7"));
        textEdit_Register = new QTextEdit(tab_6);
        textEdit_Register->setObjectName(QString::fromUtf8("textEdit_Register"));
        textEdit_Register->setStyleSheet(QString::fromUtf8("QTextEdit{background-color: rgb(0, 0, 0);}"));
        textEdit_Register->setTabChangesFocus(false);
        textEdit_Register->setUndoRedoEnabled(false);
        textEdit_Register->setReadOnly(true);

        verticalLayout_7->addWidget(textEdit_Register);

        tabWidget->addTab(tab_6, QString());
        Network = new QWidget();
        Network->setObjectName(QString::fromUtf8("Network"));
        verticalLayout_11 = new QVBoxLayout(Network);
        verticalLayout_11->setSpacing(0);
        verticalLayout_11->setContentsMargins(0, 0, 0, 0);
        verticalLayout_11->setObjectName(QString::fromUtf8("verticalLayout_11"));
        textEdit_Network = new QTextEdit(Network);
        textEdit_Network->setObjectName(QString::fromUtf8("textEdit_Network"));
        textEdit_Network->setStyleSheet(QString::fromUtf8("QTextEdit{background-color: rgb(0, 0, 0);}"));
        textEdit_Network->setTabChangesFocus(false);
        textEdit_Network->setUndoRedoEnabled(false);
        textEdit_Network->setReadOnly(true);

        verticalLayout_11->addWidget(textEdit_Network);

        tabWidget->addTab(Network, QString());
        tab_11 = new QWidget();
        tab_11->setObjectName(QString::fromUtf8("tab_11"));
        verticalLayout_17 = new QVBoxLayout(tab_11);
        verticalLayout_17->setSpacing(6);
        verticalLayout_17->setContentsMargins(11, 11, 11, 11);
        verticalLayout_17->setObjectName(QString::fromUtf8("verticalLayout_17"));
        horizontalLayout_15 = new QHBoxLayout();
        horizontalLayout_15->setSpacing(6);
        horizontalLayout_15->setObjectName(QString::fromUtf8("horizontalLayout_15"));
        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_15->addItem(horizontalSpacer_2);

        label_3 = new QLabel(tab_11);
        label_3->setObjectName(QString::fromUtf8("label_3"));

        horizontalLayout_15->addWidget(label_3);

        lineEdit_Mem_Addr = new QLineEdit(tab_11);
        lineEdit_Mem_Addr->setObjectName(QString::fromUtf8("lineEdit_Mem_Addr"));

        horizontalLayout_15->addWidget(lineEdit_Mem_Addr);

        label_4 = new QLabel(tab_11);
        label_4->setObjectName(QString::fromUtf8("label_4"));

        horizontalLayout_15->addWidget(label_4);

        spinBox_Mem_Size = new QSpinBox(tab_11);
        spinBox_Mem_Size->setObjectName(QString::fromUtf8("spinBox_Mem_Size"));
        spinBox_Mem_Size->setMinimumSize(QSize(128, 0));
        spinBox_Mem_Size->setMinimum(1);
        spinBox_Mem_Size->setMaximum(33554432);

        horizontalLayout_15->addWidget(spinBox_Mem_Size);

        pushButton_Mem_Dump = new QPushButton(tab_11);
        pushButton_Mem_Dump->setObjectName(QString::fromUtf8("pushButton_Mem_Dump"));

        horizontalLayout_15->addWidget(pushButton_Mem_Dump);

        progressBar_MemPro = new QProgressBar(tab_11);
        progressBar_MemPro->setObjectName(QString::fromUtf8("progressBar_MemPro"));
        progressBar_MemPro->setValue(0);

        horizontalLayout_15->addWidget(progressBar_MemPro);


        verticalLayout_17->addLayout(horizontalLayout_15);

        horizontalLayout_16 = new QHBoxLayout();
        horizontalLayout_16->setSpacing(6);
        horizontalLayout_16->setObjectName(QString::fromUtf8("horizontalLayout_16"));
        textEdit_MemHex = new QTextEdit(tab_11);
        textEdit_MemHex->setObjectName(QString::fromUtf8("textEdit_MemHex"));
        textEdit_MemHex->setStyleSheet(QString::fromUtf8("QTextEdit{background-color: rgb(0, 0, 0);}"));
        textEdit_MemHex->setTabChangesFocus(false);
        textEdit_MemHex->setUndoRedoEnabled(false);
        textEdit_MemHex->setReadOnly(true);

        horizontalLayout_16->addWidget(textEdit_MemHex);

        textEdit_MemASC = new QTextEdit(tab_11);
        textEdit_MemASC->setObjectName(QString::fromUtf8("textEdit_MemASC"));
        textEdit_MemASC->setStyleSheet(QString::fromUtf8("QTextEdit{background-color: rgb(0, 0, 0);}"));
        textEdit_MemASC->setTabChangesFocus(false);
        textEdit_MemASC->setUndoRedoEnabled(false);
        textEdit_MemASC->setReadOnly(true);

        horizontalLayout_16->addWidget(textEdit_MemASC);


        verticalLayout_17->addLayout(horizontalLayout_16);

        groupBox_3 = new QGroupBox(tab_11);
        groupBox_3->setObjectName(QString::fromUtf8("groupBox_3"));
        verticalLayout_16 = new QVBoxLayout(groupBox_3);
        verticalLayout_16->setSpacing(6);
        verticalLayout_16->setContentsMargins(11, 11, 11, 11);
        verticalLayout_16->setObjectName(QString::fromUtf8("verticalLayout_16"));
        horizontalLayout_17 = new QHBoxLayout();
        horizontalLayout_17->setSpacing(6);
        horizontalLayout_17->setObjectName(QString::fromUtf8("horizontalLayout_17"));
        radioButton_Mem_GBK = new QRadioButton(groupBox_3);
        radioButton_Mem_GBK->setObjectName(QString::fromUtf8("radioButton_Mem_GBK"));
        radioButton_Mem_GBK->setChecked(true);

        horizontalLayout_17->addWidget(radioButton_Mem_GBK);

        radioButton_Mem_UTF16 = new QRadioButton(groupBox_3);
        radioButton_Mem_UTF16->setObjectName(QString::fromUtf8("radioButton_Mem_UTF16"));

        horizontalLayout_17->addWidget(radioButton_Mem_UTF16);

        radioButton_Mem_UTF8 = new QRadioButton(groupBox_3);
        radioButton_Mem_UTF8->setObjectName(QString::fromUtf8("radioButton_Mem_UTF8"));

        horizontalLayout_17->addWidget(radioButton_Mem_UTF8);


        verticalLayout_16->addLayout(horizontalLayout_17);

        horizontalLayout_18 = new QHBoxLayout();
        horizontalLayout_18->setSpacing(6);
        horizontalLayout_18->setObjectName(QString::fromUtf8("horizontalLayout_18"));
        radioButton_Mem_ASM64 = new QRadioButton(groupBox_3);
        radioButton_Mem_ASM64->setObjectName(QString::fromUtf8("radioButton_Mem_ASM64"));
        radioButton_Mem_ASM64->setEnabled(false);

        horizontalLayout_18->addWidget(radioButton_Mem_ASM64);

        radioButton_Mem_ASM86 = new QRadioButton(groupBox_3);
        radioButton_Mem_ASM86->setObjectName(QString::fromUtf8("radioButton_Mem_ASM86"));
        radioButton_Mem_ASM86->setEnabled(false);

        horizontalLayout_18->addWidget(radioButton_Mem_ASM86);

        radioButton_Mem_ASM16 = new QRadioButton(groupBox_3);
        radioButton_Mem_ASM16->setObjectName(QString::fromUtf8("radioButton_Mem_ASM16"));
        radioButton_Mem_ASM16->setEnabled(false);

        horizontalLayout_18->addWidget(radioButton_Mem_ASM16);


        verticalLayout_16->addLayout(horizontalLayout_18);

        pushButton_MemorySaveToFile = new QPushButton(groupBox_3);
        pushButton_MemorySaveToFile->setObjectName(QString::fromUtf8("pushButton_MemorySaveToFile"));

        verticalLayout_16->addWidget(pushButton_MemorySaveToFile);


        verticalLayout_17->addWidget(groupBox_3);

        tabWidget->addTab(tab_11, QString());
        tab_8 = new QWidget();
        tab_8->setObjectName(QString::fromUtf8("tab_8"));
        horizontalLayout_7 = new QHBoxLayout(tab_8);
        horizontalLayout_7->setSpacing(0);
        horizontalLayout_7->setContentsMargins(0, 0, 0, 0);
        horizontalLayout_7->setObjectName(QString::fromUtf8("horizontalLayout_7"));
        widget_DumpRegOpen = new QWidget(tab_8);
        widget_DumpRegOpen->setObjectName(QString::fromUtf8("widget_DumpRegOpen"));
        horizontalLayout_5 = new QHBoxLayout(widget_DumpRegOpen);
        horizontalLayout_5->setSpacing(9);
        horizontalLayout_5->setContentsMargins(0, 0, 0, 0);
        horizontalLayout_5->setObjectName(QString::fromUtf8("horizontalLayout_5"));
        verticalLayout_10 = new QVBoxLayout();
        verticalLayout_10->setSpacing(6);
        verticalLayout_10->setObjectName(QString::fromUtf8("verticalLayout_10"));
        listWidget_Dump = new QListWidget(widget_DumpRegOpen);
        listWidget_Dump->setObjectName(QString::fromUtf8("listWidget_Dump"));

        verticalLayout_10->addWidget(listWidget_Dump);

        groupBox = new QGroupBox(widget_DumpRegOpen);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        verticalLayout_15 = new QVBoxLayout(groupBox);
        verticalLayout_15->setSpacing(6);
        verticalLayout_15->setContentsMargins(11, 11, 11, 11);
        verticalLayout_15->setObjectName(QString::fromUtf8("verticalLayout_15"));
        horizontalLayout_9 = new QHBoxLayout();
        horizontalLayout_9->setSpacing(6);
        horizontalLayout_9->setObjectName(QString::fromUtf8("horizontalLayout_9"));
        radioButton_GBK = new QRadioButton(groupBox);
        radioButton_GBK->setObjectName(QString::fromUtf8("radioButton_GBK"));
        radioButton_GBK->setChecked(true);

        horizontalLayout_9->addWidget(radioButton_GBK);

        radioButton_UTF16 = new QRadioButton(groupBox);
        radioButton_UTF16->setObjectName(QString::fromUtf8("radioButton_UTF16"));

        horizontalLayout_9->addWidget(radioButton_UTF16);

        radioButton_UTF8 = new QRadioButton(groupBox);
        radioButton_UTF8->setObjectName(QString::fromUtf8("radioButton_UTF8"));

        horizontalLayout_9->addWidget(radioButton_UTF8);


        verticalLayout_15->addLayout(horizontalLayout_9);

        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setSpacing(6);
        horizontalLayout_4->setObjectName(QString::fromUtf8("horizontalLayout_4"));
        radioButton_ASM64 = new QRadioButton(groupBox);
        radioButton_ASM64->setObjectName(QString::fromUtf8("radioButton_ASM64"));

        horizontalLayout_4->addWidget(radioButton_ASM64);

        radioButton_ASM86 = new QRadioButton(groupBox);
        radioButton_ASM86->setObjectName(QString::fromUtf8("radioButton_ASM86"));

        horizontalLayout_4->addWidget(radioButton_ASM86);

        radioButton_ASM16 = new QRadioButton(groupBox);
        radioButton_ASM16->setObjectName(QString::fromUtf8("radioButton_ASM16"));

        horizontalLayout_4->addWidget(radioButton_ASM16);


        verticalLayout_15->addLayout(horizontalLayout_4);


        verticalLayout_10->addWidget(groupBox);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setSpacing(6);
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        label = new QLabel(widget_DumpRegOpen);
        label->setObjectName(QString::fromUtf8("label"));

        horizontalLayout_3->addWidget(label);

        spinBox_DumpMaxSize = new QSpinBox(widget_DumpRegOpen);
        spinBox_DumpMaxSize->setObjectName(QString::fromUtf8("spinBox_DumpMaxSize"));
        spinBox_DumpMaxSize->setMaximum(8192);

        horizontalLayout_3->addWidget(spinBox_DumpMaxSize);

        label_2 = new QLabel(widget_DumpRegOpen);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        horizontalLayout_3->addWidget(label_2);

        spinBox_DumpMaxCount = new QSpinBox(widget_DumpRegOpen);
        spinBox_DumpMaxCount->setObjectName(QString::fromUtf8("spinBox_DumpMaxCount"));
        spinBox_DumpMaxCount->setMaximum(9999999);

        horizontalLayout_3->addWidget(spinBox_DumpMaxCount);


        verticalLayout_10->addLayout(horizontalLayout_3);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setSpacing(0);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        pushButton_DumpClear = new QPushButton(widget_DumpRegOpen);
        pushButton_DumpClear->setObjectName(QString::fromUtf8("pushButton_DumpClear"));
        pushButton_DumpClear->setMaximumSize(QSize(16777215, 16777215));

        horizontalLayout->addWidget(pushButton_DumpClear);

        pushButton_DumpDelete = new QPushButton(widget_DumpRegOpen);
        pushButton_DumpDelete->setObjectName(QString::fromUtf8("pushButton_DumpDelete"));
        pushButton_DumpDelete->setMaximumSize(QSize(16777215, 16777215));

        horizontalLayout->addWidget(pushButton_DumpDelete);

        pushButton_DumpSave = new QPushButton(widget_DumpRegOpen);
        pushButton_DumpSave->setObjectName(QString::fromUtf8("pushButton_DumpSave"));
        pushButton_DumpSave->setMaximumSize(QSize(16777215, 16777215));

        horizontalLayout->addWidget(pushButton_DumpSave);

        pushButton_DumpSaveAll = new QPushButton(widget_DumpRegOpen);
        pushButton_DumpSaveAll->setObjectName(QString::fromUtf8("pushButton_DumpSaveAll"));
        pushButton_DumpSaveAll->setMaximumSize(QSize(16777215, 16777215));

        horizontalLayout->addWidget(pushButton_DumpSaveAll);


        verticalLayout_10->addLayout(horizontalLayout);


        horizontalLayout_5->addLayout(verticalLayout_10);

        textEdit_NetworkHex = new QTextEdit(widget_DumpRegOpen);
        textEdit_NetworkHex->setObjectName(QString::fromUtf8("textEdit_NetworkHex"));
        textEdit_NetworkHex->setStyleSheet(QString::fromUtf8("QTextEdit{background-color: rgb(0, 0, 0);}"));
        textEdit_NetworkHex->setTabChangesFocus(false);
        textEdit_NetworkHex->setUndoRedoEnabled(false);
        textEdit_NetworkHex->setReadOnly(true);

        horizontalLayout_5->addWidget(textEdit_NetworkHex);

        verticalLayout_12 = new QVBoxLayout();
        verticalLayout_12->setSpacing(2);
        verticalLayout_12->setObjectName(QString::fromUtf8("verticalLayout_12"));
        textEdit_NetworkASC = new QTextEdit(widget_DumpRegOpen);
        textEdit_NetworkASC->setObjectName(QString::fromUtf8("textEdit_NetworkASC"));
        textEdit_NetworkASC->setStyleSheet(QString::fromUtf8("QTextEdit{background-color: rgb(0, 0, 0);}"));
        textEdit_NetworkASC->setTabChangesFocus(false);
        textEdit_NetworkASC->setUndoRedoEnabled(false);
        textEdit_NetworkASC->setReadOnly(true);

        verticalLayout_12->addWidget(textEdit_NetworkASC);


        horizontalLayout_5->addLayout(verticalLayout_12);

        textEdit_NetworkHex->raise();

        horizontalLayout_7->addWidget(widget_DumpRegOpen);

        label_RegNotice = new QLabel(tab_8);
        label_RegNotice->setObjectName(QString::fromUtf8("label_RegNotice"));
        label_RegNotice->setAlignment(Qt::AlignCenter);

        horizontalLayout_7->addWidget(label_RegNotice);

        tabWidget->addTab(tab_8, QString());
        tab_10 = new QWidget();
        tab_10->setObjectName(QString::fromUtf8("tab_10"));
        horizontalLayout_13 = new QHBoxLayout(tab_10);
        horizontalLayout_13->setSpacing(0);
        horizontalLayout_13->setContentsMargins(0, 0, 0, 0);
        horizontalLayout_13->setObjectName(QString::fromUtf8("horizontalLayout_13"));
        widget_StackPanel = new QWidget(tab_10);
        widget_StackPanel->setObjectName(QString::fromUtf8("widget_StackPanel"));
        horizontalLayout_14 = new QHBoxLayout(widget_StackPanel);
        horizontalLayout_14->setSpacing(6);
        horizontalLayout_14->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_14->setObjectName(QString::fromUtf8("horizontalLayout_14"));
        verticalLayout_14 = new QVBoxLayout();
        verticalLayout_14->setSpacing(6);
        verticalLayout_14->setObjectName(QString::fromUtf8("verticalLayout_14"));
        horizontalLayout_12 = new QHBoxLayout();
        horizontalLayout_12->setSpacing(6);
        horizontalLayout_12->setObjectName(QString::fromUtf8("horizontalLayout_12"));
        label_ESP = new QLabel(widget_StackPanel);
        label_ESP->setObjectName(QString::fromUtf8("label_ESP"));

        horizontalLayout_12->addWidget(label_ESP);

        horizontalSpacer_3 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_12->addItem(horizontalSpacer_3);

        lineEdit_ESP_Addr = new QLineEdit(widget_StackPanel);
        lineEdit_ESP_Addr->setObjectName(QString::fromUtf8("lineEdit_ESP_Addr"));

        horizontalLayout_12->addWidget(lineEdit_ESP_Addr);

        pushButton_StackSearch = new QPushButton(widget_StackPanel);
        pushButton_StackSearch->setObjectName(QString::fromUtf8("pushButton_StackSearch"));

        horizontalLayout_12->addWidget(pushButton_StackSearch);


        verticalLayout_14->addLayout(horizontalLayout_12);

        tableWidget_Stack = new QTableWidget(widget_StackPanel);
        if (tableWidget_Stack->columnCount() < 4)
            tableWidget_Stack->setColumnCount(4);
        QTableWidgetItem *__qtablewidgetitem = new QTableWidgetItem();
        tableWidget_Stack->setHorizontalHeaderItem(0, __qtablewidgetitem);
        QTableWidgetItem *__qtablewidgetitem1 = new QTableWidgetItem();
        tableWidget_Stack->setHorizontalHeaderItem(1, __qtablewidgetitem1);
        QTableWidgetItem *__qtablewidgetitem2 = new QTableWidgetItem();
        tableWidget_Stack->setHorizontalHeaderItem(2, __qtablewidgetitem2);
        QTableWidgetItem *__qtablewidgetitem3 = new QTableWidgetItem();
        tableWidget_Stack->setHorizontalHeaderItem(3, __qtablewidgetitem3);
        tableWidget_Stack->setObjectName(QString::fromUtf8("tableWidget_Stack"));
        tableWidget_Stack->setMaximumSize(QSize(16777215, 16777215));

        verticalLayout_14->addWidget(tableWidget_Stack);

        groupBox_2 = new QGroupBox(widget_StackPanel);
        groupBox_2->setObjectName(QString::fromUtf8("groupBox_2"));
        horizontalLayout_8 = new QHBoxLayout(groupBox_2);
        horizontalLayout_8->setSpacing(10);
        horizontalLayout_8->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_8->setObjectName(QString::fromUtf8("horizontalLayout_8"));
        radioButton_Stack_GBK = new QRadioButton(groupBox_2);
        radioButton_Stack_GBK->setObjectName(QString::fromUtf8("radioButton_Stack_GBK"));
        radioButton_Stack_GBK->setChecked(true);

        horizontalLayout_8->addWidget(radioButton_Stack_GBK);

        radioButton_Stack_UTF16 = new QRadioButton(groupBox_2);
        radioButton_Stack_UTF16->setObjectName(QString::fromUtf8("radioButton_Stack_UTF16"));

        horizontalLayout_8->addWidget(radioButton_Stack_UTF16);

        radioButton_Stack_UTF8 = new QRadioButton(groupBox_2);
        radioButton_Stack_UTF8->setObjectName(QString::fromUtf8("radioButton_Stack_UTF8"));

        horizontalLayout_8->addWidget(radioButton_Stack_UTF8);


        verticalLayout_14->addWidget(groupBox_2);

        horizontalLayout_11 = new QHBoxLayout();
        horizontalLayout_11->setSpacing(2);
        horizontalLayout_11->setObjectName(QString::fromUtf8("horizontalLayout_11"));
        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_11->addItem(horizontalSpacer);

        pushButton_StackClear = new QPushButton(widget_StackPanel);
        pushButton_StackClear->setObjectName(QString::fromUtf8("pushButton_StackClear"));

        horizontalLayout_11->addWidget(pushButton_StackClear);

        pushButton_StackUpdate = new QPushButton(widget_StackPanel);
        pushButton_StackUpdate->setObjectName(QString::fromUtf8("pushButton_StackUpdate"));

        horizontalLayout_11->addWidget(pushButton_StackUpdate);


        verticalLayout_14->addLayout(horizontalLayout_11);


        horizontalLayout_14->addLayout(verticalLayout_14);

        verticalLayout_13 = new QVBoxLayout();
        verticalLayout_13->setSpacing(6);
        verticalLayout_13->setObjectName(QString::fromUtf8("verticalLayout_13"));
        horizontalLayout_10 = new QHBoxLayout();
        horizontalLayout_10->setSpacing(6);
        horizontalLayout_10->setObjectName(QString::fromUtf8("horizontalLayout_10"));
        textEdit_StackHex = new QTextEdit(widget_StackPanel);
        textEdit_StackHex->setObjectName(QString::fromUtf8("textEdit_StackHex"));
        textEdit_StackHex->setStyleSheet(QString::fromUtf8("QTextEdit{background-color: rgb(0, 0, 0);}"));
        textEdit_StackHex->setTabChangesFocus(false);
        textEdit_StackHex->setUndoRedoEnabled(false);
        textEdit_StackHex->setReadOnly(true);

        horizontalLayout_10->addWidget(textEdit_StackHex);

        textEdit_StackASC = new QTextEdit(widget_StackPanel);
        textEdit_StackASC->setObjectName(QString::fromUtf8("textEdit_StackASC"));
        textEdit_StackASC->setStyleSheet(QString::fromUtf8("QTextEdit{background-color: rgb(0, 0, 0);}"));
        textEdit_StackASC->setTabChangesFocus(false);
        textEdit_StackASC->setUndoRedoEnabled(false);
        textEdit_StackASC->setReadOnly(true);

        horizontalLayout_10->addWidget(textEdit_StackASC);


        verticalLayout_13->addLayout(horizontalLayout_10);


        horizontalLayout_14->addLayout(verticalLayout_13);


        horizontalLayout_13->addWidget(widget_StackPanel);

        label_StackRegNotice = new QLabel(tab_10);
        label_StackRegNotice->setObjectName(QString::fromUtf8("label_StackRegNotice"));
        label_StackRegNotice->setAlignment(Qt::AlignCenter);

        horizontalLayout_13->addWidget(label_StackRegNotice);

        tabWidget->addTab(tab_10, QString());

        verticalLayout->addWidget(tabWidget);

        horizontalLayout_6 = new QHBoxLayout();
        horizontalLayout_6->setSpacing(0);
        horizontalLayout_6->setObjectName(QString::fromUtf8("horizontalLayout_6"));
        lineEdit_CommandLine = new QLineEdit(centralWidget);
        lineEdit_CommandLine->setObjectName(QString::fromUtf8("lineEdit_CommandLine"));

        horizontalLayout_6->addWidget(lineEdit_CommandLine);

        pushButton_Enter = new QPushButton(centralWidget);
        pushButton_Enter->setObjectName(QString::fromUtf8("pushButton_Enter"));

        horizontalLayout_6->addWidget(pushButton_Enter);

        pushButton_Switch = new QPushButton(centralWidget);
        pushButton_Switch->setObjectName(QString::fromUtf8("pushButton_Switch"));

        horizontalLayout_6->addWidget(pushButton_Switch);


        verticalLayout->addLayout(horizontalLayout_6);

        PeDollcClass->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(PeDollcClass);
        menuBar->setObjectName(QString::fromUtf8("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 1136, 23));
        menu = new QMenu(menuBar);
        menu->setObjectName(QString::fromUtf8("menu"));
        menu_2 = new QMenu(menuBar);
        menu_2->setObjectName(QString::fromUtf8("menu_2"));
        menu_4 = new QMenu(menuBar);
        menu_4->setObjectName(QString::fromUtf8("menu_4"));
        menuTools = new QMenu(menuBar);
        menuTools->setObjectName(QString::fromUtf8("menuTools"));
        PeDollcClass->setMenuBar(menuBar);
        mainToolBar = new QToolBar(PeDollcClass);
        mainToolBar->setObjectName(QString::fromUtf8("mainToolBar"));
        PeDollcClass->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(PeDollcClass);
        statusBar->setObjectName(QString::fromUtf8("statusBar"));
        PeDollcClass->setStatusBar(statusBar);

        menuBar->addAction(menu->menuAction());
        menuBar->addAction(menu_2->menuAction());
        menuBar->addAction(menuTools->menuAction());
        menuBar->addAction(menu_4->menuAction());
        menu->addAction(action_Menu_MonitorConnector);
        menu->addAction(action_Menu_Exit);
        menu_2->addAction(actionLoad_menu_hookregulars);
        menu_2->addAction(action_Menu_ClearHook);
        menu_2->addAction(action_Menu_ClearFilter);
        menu_4->addAction(action_PeDoll);
        menuTools->addAction(actionHexToDex);
        menuTools->addAction(action_Menu_VMWare_vmx_Anti_virtualization);
        mainToolBar->addAction(action_Tool_ControllerConnect);
        mainToolBar->addAction(actionControllerClose);
        mainToolBar->addSeparator();
        mainToolBar->addAction(action_Tool_DollConnect);
        mainToolBar->addAction(action_Tool_Hook);
        mainToolBar->addAction(action_Tool_Run);
        mainToolBar->addAction(action_Tool_Stop);
        mainToolBar->addSeparator();
        mainToolBar->addAction(action_tool_Tab);
        mainToolBar->addAction(action_tool_Clear);

        retranslateUi(PeDollcClass);

        tabWidget->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(PeDollcClass);
    } // setupUi

    void retranslateUi(QMainWindow *PeDollcClass)
    {
        PeDollcClass->setWindowTitle(QApplication::translate("PeDollcClass", "PeDoll--PE32 Monitor trial", 0, QApplication::UnicodeUTF8));
        action_Menu_MonitorConnector->setText(QApplication::translate("PeDollcClass", "Monitor Connector", 0, QApplication::UnicodeUTF8));
        action_Menu_Exit->setText(QApplication::translate("PeDollcClass", "Exit", 0, QApplication::UnicodeUTF8));
        action_Menu_ClearHook->setText(QApplication::translate("PeDollcClass", "Clear API Hook Regulars", 0, QApplication::UnicodeUTF8));
        action_Menu_ClearFilter->setText(QApplication::translate("PeDollcClass", "Clear Functions Filter Regulars", 0, QApplication::UnicodeUTF8));
        action_4->setText(QApplication::translate("PeDollcClass", "Terminate", 0, QApplication::UnicodeUTF8));
        action_API->setText(QApplication::translate("PeDollcClass", "Unhook all apis", 0, QApplication::UnicodeUTF8));
        action_API_2->setText(QApplication::translate("PeDollcClass", "Hook all apis", 0, QApplication::UnicodeUTF8));
        action_5->setText(QApplication::translate("PeDollcClass", "Restart", 0, QApplication::UnicodeUTF8));
        action_6->setText(QApplication::translate("PeDollcClass", "Run", 0, QApplication::UnicodeUTF8));
        action_PeDoll->setText(QApplication::translate("PeDollcClass", "About PeDoll", 0, QApplication::UnicodeUTF8));
        action_menu_regist->setText(QApplication::translate("PeDollcClass", "Register", 0, QApplication::UnicodeUTF8));
        action_Tool_ControllerConnect->setText(QApplication::translate("PeDollcClass", "connect", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        action_Tool_ControllerConnect->setToolTip(QApplication::translate("PeDollcClass", "Connect to Controller", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        action_Tool_DollConnect->setText(QApplication::translate("PeDollcClass", "Linktodoll", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        action_Tool_DollConnect->setToolTip(QApplication::translate("PeDollcClass", "Link to doll", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        action_Tool_Run->setText(QApplication::translate("PeDollcClass", "run", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        action_Tool_Run->setToolTip(QApplication::translate("PeDollcClass", "run", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        action_Tool_Stop->setText(QApplication::translate("PeDollcClass", "stop", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        action_Tool_Stop->setToolTip(QApplication::translate("PeDollcClass", "stop", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        action_Tool_Hook->setText(QApplication::translate("PeDollcClass", "Hook", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        action_Tool_Hook->setToolTip(QApplication::translate("PeDollcClass", "APIs Hook", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        actionLoad_menu_hookregulars->setText(QApplication::translate("PeDollcClass", "Load hook regulars", 0, QApplication::UnicodeUTF8));
        action_tool_Clear->setText(QApplication::translate("PeDollcClass", "Clear", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        action_tool_Clear->setToolTip(QApplication::translate("PeDollcClass", "Clear", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        action_tool_Tab->setText(QApplication::translate("PeDollcClass", "tab", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        action_tool_Tab->setToolTip(QApplication::translate("PeDollcClass", "Tab", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        action_tool_Tab->setShortcut(QApplication::translate("PeDollcClass", "Up", 0, QApplication::UnicodeUTF8));
        actionControllerClose->setText(QApplication::translate("PeDollcClass", "ControllerClose", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        actionControllerClose->setToolTip(QApplication::translate("PeDollcClass", "Close controller", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        actionKill_Virtual_Machine_Detector->setText(QApplication::translate("PeDollcClass", "Kill Virtual Machine Detector", 0, QApplication::UnicodeUTF8));
        action_Menu_VMWare_vmx_Anti_virtualization->setText(QApplication::translate("PeDollcClass", "VMWare vmx Dis-virtualization", 0, QApplication::UnicodeUTF8));
        actionHexToDex->setText(QApplication::translate("PeDollcClass", "Transfer to Dex", 0, QApplication::UnicodeUTF8));
        textEdit_Monitor_Messages->setHtml(QApplication::translate("PeDollcClass", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'SimSun'; font-size:9pt; font-weight:400; font-style:normal;\">\n"
"<p style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><br /></p></body></html>", 0, QApplication::UnicodeUTF8));
        tabWidget->setTabText(tabWidget->indexOf(tab), QApplication::translate("PeDollcClass", "Monitor Messages", 0, QApplication::UnicodeUTF8));
        textEdit_Query->setHtml(QApplication::translate("PeDollcClass", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'SimSun'; font-size:9pt; font-weight:400; font-style:normal;\">\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:16pt; font-weight:600; color:#ff0000;\">API:</span></p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:20pt; color:#ffff7f;\">unknow</span></p>\n"
"<p style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px; font-size:12pt;\"><br /></p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;"
                        "\"><span style=\" font-size:16pt; color:#55ffff;\">MSDN Notes:</span></p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:12pt; color:#00ff00;\">unknow</span></p>\n"
"<p style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px; font-size:12pt; color:#00ff00;\"><br /></p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:16pt; font-weight:600; color:#ffaa00;\">Behavior:</span></p>\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:12pt; color:#00ff00;\">unknow</span></p></body></html>", 0, QApplication::UnicodeUTF8));
        pushButton_QueryPass->setText(QApplication::translate("PeDollcClass", "Pass", 0, QApplication::UnicodeUTF8));
        pushButton_QueryReject->setText(QApplication::translate("PeDollcClass", "Reject", 0, QApplication::UnicodeUTF8));
        pushButton_QueryTerminate->setText(QApplication::translate("PeDollcClass", "Terminate", 0, QApplication::UnicodeUTF8));
        tabWidget->setTabText(tabWidget->indexOf(tab_9), QApplication::translate("PeDollcClass", "Query", 0, QApplication::UnicodeUTF8));
        textEdit_Hook_Regular->setHtml(QApplication::translate("PeDollcClass", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'SimSun'; font-size:9pt; font-weight:400; font-style:normal;\">\n"
"<p style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><br /></p></body></html>", 0, QApplication::UnicodeUTF8));
        tabWidget->setTabText(tabWidget->indexOf(tab_2), QApplication::translate("PeDollcClass", "API Hook regular", 0, QApplication::UnicodeUTF8));
        textEdit_Functions_Filter->setHtml(QApplication::translate("PeDollcClass", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'SimSun'; font-size:9pt; font-weight:400; font-style:normal;\">\n"
"<p style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><br /></p></body></html>", 0, QApplication::UnicodeUTF8));
        tabWidget->setTabText(tabWidget->indexOf(tab_3), QApplication::translate("PeDollcClass", "Functions filter", 0, QApplication::UnicodeUTF8));
        textEdit_Files->setHtml(QApplication::translate("PeDollcClass", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'SimSun'; font-size:9pt; font-weight:400; font-style:normal;\">\n"
"<p style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><br /></p></body></html>", 0, QApplication::UnicodeUTF8));
        tabWidget->setTabText(tabWidget->indexOf(tab_5), QApplication::translate("PeDollcClass", "Files", 0, QApplication::UnicodeUTF8));
        textEdit_Windows_Operator->setHtml(QApplication::translate("PeDollcClass", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'SimSun'; font-size:9pt; font-weight:400; font-style:normal;\">\n"
"<p style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><br /></p></body></html>", 0, QApplication::UnicodeUTF8));
        tabWidget->setTabText(tabWidget->indexOf(tab_4), QApplication::translate("PeDollcClass", "Windows Operater", 0, QApplication::UnicodeUTF8));
        textEdit_Processes->setHtml(QApplication::translate("PeDollcClass", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'SimSun'; font-size:9pt; font-weight:400; font-style:normal;\">\n"
"<p style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><br /></p></body></html>", 0, QApplication::UnicodeUTF8));
        tabWidget->setTabText(tabWidget->indexOf(tab_7), QApplication::translate("PeDollcClass", "Processes", 0, QApplication::UnicodeUTF8));
        textEdit_Register->setHtml(QApplication::translate("PeDollcClass", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'SimSun'; font-size:9pt; font-weight:400; font-style:normal;\">\n"
"<p style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><br /></p></body></html>", 0, QApplication::UnicodeUTF8));
        tabWidget->setTabText(tabWidget->indexOf(tab_6), QApplication::translate("PeDollcClass", "Registrys", 0, QApplication::UnicodeUTF8));
        textEdit_Network->setHtml(QApplication::translate("PeDollcClass", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'SimSun'; font-size:9pt; font-weight:400; font-style:normal;\">\n"
"<p style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><br /></p></body></html>", 0, QApplication::UnicodeUTF8));
        tabWidget->setTabText(tabWidget->indexOf(Network), QApplication::translate("PeDollcClass", "Network", 0, QApplication::UnicodeUTF8));
        label_3->setText(QApplication::translate("PeDollcClass", "Address:", 0, QApplication::UnicodeUTF8));
        label_4->setText(QApplication::translate("PeDollcClass", "size:", 0, QApplication::UnicodeUTF8));
        spinBox_Mem_Size->setSuffix(QApplication::translate("PeDollcClass", " bytes", 0, QApplication::UnicodeUTF8));
        pushButton_Mem_Dump->setText(QApplication::translate("PeDollcClass", "dump", 0, QApplication::UnicodeUTF8));
        textEdit_MemHex->setHtml(QApplication::translate("PeDollcClass", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'SimSun'; font-size:9pt; font-weight:400; font-style:normal;\">\n"
"<p style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><br /></p></body></html>", 0, QApplication::UnicodeUTF8));
        textEdit_MemASC->setHtml(QApplication::translate("PeDollcClass", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'SimSun'; font-size:9pt; font-weight:400; font-style:normal;\">\n"
"<p style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><br /></p></body></html>", 0, QApplication::UnicodeUTF8));
        groupBox_3->setTitle(QApplication::translate("PeDollcClass", "Encodes", 0, QApplication::UnicodeUTF8));
        radioButton_Mem_GBK->setText(QApplication::translate("PeDollcClass", "GBK(Windows default)", 0, QApplication::UnicodeUTF8));
        radioButton_Mem_UTF16->setText(QApplication::translate("PeDollcClass", "UTF16", 0, QApplication::UnicodeUTF8));
        radioButton_Mem_UTF8->setText(QApplication::translate("PeDollcClass", "UTF8", 0, QApplication::UnicodeUTF8));
        radioButton_Mem_ASM64->setText(QApplication::translate("PeDollcClass", "disassembly x64", 0, QApplication::UnicodeUTF8));
        radioButton_Mem_ASM86->setText(QApplication::translate("PeDollcClass", "disassembly x86", 0, QApplication::UnicodeUTF8));
        radioButton_Mem_ASM16->setText(QApplication::translate("PeDollcClass", "disassembly 16", 0, QApplication::UnicodeUTF8));
        pushButton_MemorySaveToFile->setText(QApplication::translate("PeDollcClass", "MemorySaveToFile", 0, QApplication::UnicodeUTF8));
        tabWidget->setTabText(tabWidget->indexOf(tab_11), QApplication::translate("PeDollcClass", "Memory", 0, QApplication::UnicodeUTF8));
        groupBox->setTitle(QApplication::translate("PeDollcClass", "Encodes", 0, QApplication::UnicodeUTF8));
        radioButton_GBK->setText(QApplication::translate("PeDollcClass", "GBK(Windows default)", 0, QApplication::UnicodeUTF8));
        radioButton_UTF16->setText(QApplication::translate("PeDollcClass", "UTF16", 0, QApplication::UnicodeUTF8));
        radioButton_UTF8->setText(QApplication::translate("PeDollcClass", "UTF8", 0, QApplication::UnicodeUTF8));
        radioButton_ASM64->setText(QApplication::translate("PeDollcClass", "disassembly x64", 0, QApplication::UnicodeUTF8));
        radioButton_ASM86->setText(QApplication::translate("PeDollcClass", "disassembly x86", 0, QApplication::UnicodeUTF8));
        radioButton_ASM16->setText(QApplication::translate("PeDollcClass", "disassembly 16", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("PeDollcClass", "Max Size", 0, QApplication::UnicodeUTF8));
        spinBox_DumpMaxSize->setSuffix(QApplication::translate("PeDollcClass", " bytes", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("PeDollcClass", "Max count", 0, QApplication::UnicodeUTF8));
        pushButton_DumpClear->setText(QApplication::translate("PeDollcClass", "Clear", 0, QApplication::UnicodeUTF8));
        pushButton_DumpDelete->setText(QApplication::translate("PeDollcClass", "Delete", 0, QApplication::UnicodeUTF8));
        pushButton_DumpSave->setText(QApplication::translate("PeDollcClass", "Save", 0, QApplication::UnicodeUTF8));
        pushButton_DumpSaveAll->setText(QApplication::translate("PeDollcClass", "Save all", 0, QApplication::UnicodeUTF8));
        textEdit_NetworkHex->setHtml(QApplication::translate("PeDollcClass", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'SimSun'; font-size:9pt; font-weight:400; font-style:normal;\">\n"
"<p style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><br /></p></body></html>", 0, QApplication::UnicodeUTF8));
        textEdit_NetworkASC->setHtml(QApplication::translate("PeDollcClass", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'SimSun'; font-size:9pt; font-weight:400; font-style:normal;\">\n"
"<p style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><br /></p></body></html>", 0, QApplication::UnicodeUTF8));
        label_RegNotice->setText(QApplication::translate("PeDollcClass", "Only for registered", 0, QApplication::UnicodeUTF8));
        tabWidget->setTabText(tabWidget->indexOf(tab_8), QApplication::translate("PeDollcClass", "Dump", 0, QApplication::UnicodeUTF8));
        label_ESP->setText(QApplication::translate("PeDollcClass", "ESP:00000000", 0, QApplication::UnicodeUTF8));
        pushButton_StackSearch->setText(QApplication::translate("PeDollcClass", "Go", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem = tableWidget_Stack->horizontalHeaderItem(0);
        ___qtablewidgetitem->setText(QApplication::translate("PeDollcClass", "Address", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem1 = tableWidget_Stack->horizontalHeaderItem(1);
        ___qtablewidgetitem1->setText(QApplication::translate("PeDollcClass", "DWORD", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem2 = tableWidget_Stack->horizontalHeaderItem(2);
        ___qtablewidgetitem2->setText(QApplication::translate("PeDollcClass", "DEC", 0, QApplication::UnicodeUTF8));
        QTableWidgetItem *___qtablewidgetitem3 = tableWidget_Stack->horizontalHeaderItem(3);
        ___qtablewidgetitem3->setText(QApplication::translate("PeDollcClass", "String Pointer", 0, QApplication::UnicodeUTF8));
        groupBox_2->setTitle(QApplication::translate("PeDollcClass", "Encodes", 0, QApplication::UnicodeUTF8));
        radioButton_Stack_GBK->setText(QApplication::translate("PeDollcClass", "GBK(Windows default)", 0, QApplication::UnicodeUTF8));
        radioButton_Stack_UTF16->setText(QApplication::translate("PeDollcClass", "UTF16", 0, QApplication::UnicodeUTF8));
        radioButton_Stack_UTF8->setText(QApplication::translate("PeDollcClass", "UTF8", 0, QApplication::UnicodeUTF8));
        pushButton_StackClear->setText(QApplication::translate("PeDollcClass", "Clear", 0, QApplication::UnicodeUTF8));
        pushButton_StackUpdate->setText(QApplication::translate("PeDollcClass", "Update", 0, QApplication::UnicodeUTF8));
        textEdit_StackHex->setHtml(QApplication::translate("PeDollcClass", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'SimSun'; font-size:9pt; font-weight:400; font-style:normal;\">\n"
"<p style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><br /></p></body></html>", 0, QApplication::UnicodeUTF8));
        textEdit_StackASC->setHtml(QApplication::translate("PeDollcClass", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'SimSun'; font-size:9pt; font-weight:400; font-style:normal;\">\n"
"<p style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><br /></p></body></html>", 0, QApplication::UnicodeUTF8));
        label_StackRegNotice->setText(QApplication::translate("PeDollcClass", "Only for registered", 0, QApplication::UnicodeUTF8));
        tabWidget->setTabText(tabWidget->indexOf(tab_10), QApplication::translate("PeDollcClass", "Stack", 0, QApplication::UnicodeUTF8));
        pushButton_Enter->setText(QApplication::translate("PeDollcClass", "Enter", 0, QApplication::UnicodeUTF8));
        pushButton_Switch->setText(QApplication::translate("PeDollcClass", "Switch", 0, QApplication::UnicodeUTF8));
        menu->setTitle(QApplication::translate("PeDollcClass", "Monitor", 0, QApplication::UnicodeUTF8));
        menu_2->setTitle(QApplication::translate("PeDollcClass", "Regular", 0, QApplication::UnicodeUTF8));
        menu_4->setTitle(QApplication::translate("PeDollcClass", "About", 0, QApplication::UnicodeUTF8));
        menuTools->setTitle(QApplication::translate("PeDollcClass", "Tools", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class PeDollcClass: public Ui_PeDollcClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PEDOLLC_H
