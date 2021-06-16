/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.6.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QSplitter>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QAction *actionExit;
    QAction *actionSave_Session;
    QAction *actionLoad_Session;
    QAction *actionAbout;
    QAction *actionSerial_RTU;
    QAction *actionScan;
    QAction *actionOpenLogFile;
    QAction *actionConnect;
    QAction *actionNew_RegAddress;
    QAction *actionNew_Parameter;
    QAction *actionNew_Address_Type;
    QAction *actionNew_DeviceType;
    QAction *actionNew_Page;
    QAction *actionNew_Menu;
    QAction *actionPair_Menu_Page;
    QAction *actionWriteToDevice;
    QAction *actionReadFromDevice;
    QWidget *centralwidget;
    QVBoxLayout *verticalLayout_2;
    QSplitter *splitter;
    QMenuBar *menubar;
    QMenu *menuYard_m;
    QMenu *menuStudio;
    QMenu *menuAyarlar;
    QStatusBar *statusBar;
    QToolBar *topToolBar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QStringLiteral("MainWindow"));
        MainWindow->resize(1280, 920);
        actionExit = new QAction(MainWindow);
        actionExit->setObjectName(QStringLiteral("actionExit"));
        QIcon icon;
        icon.addFile(QStringLiteral(":/icons/exit-16.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionExit->setIcon(icon);
        actionExit->setIconVisibleInMenu(true);
        actionSave_Session = new QAction(MainWindow);
        actionSave_Session->setObjectName(QStringLiteral("actionSave_Session"));
        QIcon icon1;
        icon1.addFile(QStringLiteral(":/icons/document-export-16.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionSave_Session->setIcon(icon1);
        actionLoad_Session = new QAction(MainWindow);
        actionLoad_Session->setObjectName(QStringLiteral("actionLoad_Session"));
        QIcon icon2;
        icon2.addFile(QStringLiteral(":/icons/document-import-16.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionLoad_Session->setIcon(icon2);
        actionAbout = new QAction(MainWindow);
        actionAbout->setObjectName(QStringLiteral("actionAbout"));
        QIcon icon3;
        icon3.addFile(QStringLiteral(":/icons/info-sign-16.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionAbout->setIcon(icon3);
        actionAbout->setIconVisibleInMenu(true);
        actionSerial_RTU = new QAction(MainWindow);
        actionSerial_RTU->setObjectName(QStringLiteral("actionSerial_RTU"));
        QIcon icon4;
        icon4.addFile(QStringLiteral(":/icons/serial-pot-16.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionSerial_RTU->setIcon(icon4);
        actionSerial_RTU->setText(QStringLiteral("COM Port"));
        actionSerial_RTU->setIconVisibleInMenu(true);
        actionScan = new QAction(MainWindow);
        actionScan->setObjectName(QStringLiteral("actionScan"));
        actionScan->setCheckable(true);
        QIcon icon5;
        icon5.addFile(QStringLiteral(":/icons/cyclic-process-16.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionScan->setIcon(icon5);
        actionOpenLogFile = new QAction(MainWindow);
        actionOpenLogFile->setObjectName(QStringLiteral("actionOpenLogFile"));
        QIcon icon6;
        icon6.addFile(QStringLiteral(":/icons/text-x-log-16.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionOpenLogFile->setIcon(icon6);
        actionConnect = new QAction(MainWindow);
        actionConnect->setObjectName(QStringLiteral("actionConnect"));
        actionConnect->setCheckable(true);
        QIcon icon7;
        icon7.addFile(QStringLiteral(":/icons/plug-disconnect-16.png"), QSize(), QIcon::Normal, QIcon::Off);
        icon7.addFile(QStringLiteral(":/icons/plug-connect-16.png"), QSize(), QIcon::Normal, QIcon::On);
        actionConnect->setIcon(icon7);
        actionNew_RegAddress = new QAction(MainWindow);
        actionNew_RegAddress->setObjectName(QStringLiteral("actionNew_RegAddress"));
        QIcon icon8;
        icon8.addFile(QStringLiteral(":/icons/Header16.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionNew_RegAddress->setIcon(icon8);
        actionNew_Parameter = new QAction(MainWindow);
        actionNew_Parameter->setObjectName(QStringLiteral("actionNew_Parameter"));
        actionNew_Parameter->setIcon(icon8);
        actionNew_Address_Type = new QAction(MainWindow);
        actionNew_Address_Type->setObjectName(QStringLiteral("actionNew_Address_Type"));
        actionNew_Address_Type->setIcon(icon8);
        actionNew_DeviceType = new QAction(MainWindow);
        actionNew_DeviceType->setObjectName(QStringLiteral("actionNew_DeviceType"));
        QIcon icon9;
        icon9.addFile(QStringLiteral(":/icons/connect-24.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionNew_DeviceType->setIcon(icon9);
        actionNew_Page = new QAction(MainWindow);
        actionNew_Page->setObjectName(QStringLiteral("actionNew_Page"));
        actionNew_Page->setIcon(icon6);
        actionNew_Menu = new QAction(MainWindow);
        actionNew_Menu->setObjectName(QStringLiteral("actionNew_Menu"));
        QIcon icon10;
        icon10.addFile(QStringLiteral(":/icons/edit-16.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionNew_Menu->setIcon(icon10);
        actionPair_Menu_Page = new QAction(MainWindow);
        actionPair_Menu_Page->setObjectName(QStringLiteral("actionPair_Menu_Page"));
        QIcon icon11;
        icon11.addFile(QStringLiteral(":/icons/data-sort-16.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionPair_Menu_Page->setIcon(icon11);
        actionWriteToDevice = new QAction(MainWindow);
        actionWriteToDevice->setObjectName(QStringLiteral("actionWriteToDevice"));
        QIcon icon12;
        icon12.addFile(QStringLiteral(":/icons/upload.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionWriteToDevice->setIcon(icon12);
        actionReadFromDevice = new QAction(MainWindow);
        actionReadFromDevice->setObjectName(QStringLiteral("actionReadFromDevice"));
        QIcon icon13;
        icon13.addFile(QStringLiteral(":/icons/download.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionReadFromDevice->setIcon(icon13);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName(QStringLiteral("centralwidget"));
        verticalLayout_2 = new QVBoxLayout(centralwidget);
        verticalLayout_2->setSpacing(3);
        verticalLayout_2->setObjectName(QStringLiteral("verticalLayout_2"));
        verticalLayout_2->setContentsMargins(3, 3, 3, 3);
        splitter = new QSplitter(centralwidget);
        splitter->setObjectName(QStringLiteral("splitter"));
        splitter->setMinimumSize(QSize(3, 7));
        splitter->setAutoFillBackground(false);
        splitter->setStyleSheet(QLatin1String("/*QSplitter[orientation=\"1\"]::handle {\n"
"	border: 1px solid #567dbc;\n"
"	min-width: 2px;	\n"
"	max-width: 2px;\n"
"    min-height: 100px;	\n"
"	max-height: 100px;\n"
"	\n"
"}*/\n"
"/*QSplitte::handle::horizontal {\n"
"	border: 3px dashed blue;\n"
"	margin: 50px 1px;\n"
"	min-width: 10px;	\n"
"	max-width: 10px;\n"
"}*/"));
        splitter->setFrameShape(QFrame::NoFrame);
        splitter->setFrameShadow(QFrame::Plain);
        splitter->setLineWidth(0);
        splitter->setMidLineWidth(0);
        splitter->setOrientation(Qt::Horizontal);
        splitter->setOpaqueResize(true);
        splitter->setHandleWidth(3);
        splitter->setChildrenCollapsible(false);

        verticalLayout_2->addWidget(splitter);

        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName(QStringLiteral("menubar"));
        menubar->setGeometry(QRect(0, 0, 1280, 20));
        menuYard_m = new QMenu(menubar);
        menuYard_m->setObjectName(QStringLiteral("menuYard_m"));
        menuStudio = new QMenu(menubar);
        menuStudio->setObjectName(QStringLiteral("menuStudio"));
        menuStudio->setEnabled(true);
        menuAyarlar = new QMenu(menubar);
        menuAyarlar->setObjectName(QStringLiteral("menuAyarlar"));
        MainWindow->setMenuBar(menubar);
        statusBar = new QStatusBar(MainWindow);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        MainWindow->setStatusBar(statusBar);
        topToolBar = new QToolBar(MainWindow);
        topToolBar->setObjectName(QStringLiteral("topToolBar"));
        topToolBar->setMinimumSize(QSize(0, 0));
        QFont font;
        font.setFamily(QStringLiteral("MS Shell Dlg 2"));
        font.setPointSize(8);
        topToolBar->setFont(font);
        topToolBar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
        topToolBar->setFloatable(true);
        MainWindow->addToolBar(Qt::TopToolBarArea, topToolBar);

        menubar->addAction(menuAyarlar->menuAction());
        menubar->addAction(menuStudio->menuAction());
        menubar->addAction(menuYard_m->menuAction());
        menuYard_m->addAction(actionAbout);
        menuStudio->addAction(actionNew_RegAddress);
        menuStudio->addAction(actionNew_Parameter);
        menuStudio->addSeparator();
        menuStudio->addAction(actionNew_Address_Type);
        menuStudio->addAction(actionNew_DeviceType);
        menuStudio->addSeparator();
        menuStudio->addAction(actionNew_Menu);
        menuStudio->addAction(actionNew_Page);
        menuStudio->addAction(actionPair_Menu_Page);
        menuAyarlar->addAction(actionSerial_RTU);
        topToolBar->addAction(actionSerial_RTU);
        topToolBar->addAction(actionConnect);
        topToolBar->addSeparator();
        topToolBar->addAction(actionScan);
        topToolBar->addAction(actionOpenLogFile);
        topToolBar->addSeparator();
        topToolBar->addAction(actionAbout);
        topToolBar->addSeparator();

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "OnLab Studio", Q_NULLPTR));
        actionExit->setText(QApplication::translate("MainWindow", "\303\207\304\261k\304\261\305\237", Q_NULLPTR));
        actionSave_Session->setText(QApplication::translate("MainWindow", "D\304\261\305\237a Aktar", Q_NULLPTR));
        actionLoad_Session->setText(QApplication::translate("MainWindow", "\304\260\303\247e Aktar", Q_NULLPTR));
        actionAbout->setText(QApplication::translate("MainWindow", "Hakk\304\261nda", Q_NULLPTR));
        actionScan->setText(QApplication::translate("MainWindow", "Tara", Q_NULLPTR));
        actionOpenLogFile->setText(QApplication::translate("MainWindow", "Kay\304\261t", Q_NULLPTR));
        actionConnect->setText(QApplication::translate("MainWindow", "Ba\304\237lan", Q_NULLPTR));
        actionNew_RegAddress->setText(QApplication::translate("MainWindow", "Registers", Q_NULLPTR));
        actionNew_Parameter->setText(QApplication::translate("MainWindow", "Parameters", Q_NULLPTR));
        actionNew_Address_Type->setText(QApplication::translate("MainWindow", "Address Types", Q_NULLPTR));
        actionNew_DeviceType->setText(QApplication::translate("MainWindow", "Device Types", Q_NULLPTR));
        actionNew_Page->setText(QApplication::translate("MainWindow", "Pages", Q_NULLPTR));
        actionNew_Menu->setText(QApplication::translate("MainWindow", "Menus", Q_NULLPTR));
        actionPair_Menu_Page->setText(QApplication::translate("MainWindow", "Pair Menu - Page", Q_NULLPTR));
        actionWriteToDevice->setText(QApplication::translate("MainWindow", "Cihaza Yaz", Q_NULLPTR));
#ifndef QT_NO_TOOLTIP
        actionWriteToDevice->setToolTip(QApplication::translate("MainWindow", "Cihaza Yaz", Q_NULLPTR));
#endif // QT_NO_TOOLTIP
        actionReadFromDevice->setText(QApplication::translate("MainWindow", "Cihazdan Oku", Q_NULLPTR));
#ifndef QT_NO_TOOLTIP
        actionReadFromDevice->setToolTip(QApplication::translate("MainWindow", "Cihazdan Oku", Q_NULLPTR));
#endif // QT_NO_TOOLTIP
        menuYard_m->setTitle(QApplication::translate("MainWindow", "Yard\304\261m", Q_NULLPTR));
        menuStudio->setTitle(QApplication::translate("MainWindow", "OnLab", Q_NULLPTR));
        menuAyarlar->setTitle(QApplication::translate("MainWindow", "Ayarlar", Q_NULLPTR));
        topToolBar->setWindowTitle(QApplication::translate("MainWindow", "TopBar", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
