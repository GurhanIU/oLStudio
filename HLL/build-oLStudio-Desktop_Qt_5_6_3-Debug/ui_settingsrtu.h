/********************************************************************************
** Form generated from reading UI file 'settingsrtu.ui'
**
** Created by: Qt User Interface Compiler version 5.6.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SETTINGSRTU_H
#define UI_SETTINGSRTU_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_SettingsRTU
{
public:
    QVBoxLayout *verticalLayout;
    QGridLayout *gridLayout;
    QComboBox *cmbDev;
    QLabel *lblDev;
    QLabel *lblBaud;
    QSpinBox *sbPort;
    QComboBox *cmbBaud;
    QComboBox *cmbDataBits;
    QLabel *lblDataBits;
    QLabel *lblRTS;
    QLabel *lblStopBits;
    QComboBox *cmbRTS;
    QComboBox *cmbParity;
    QComboBox *cmbStopBits;
    QLabel *lblParity;
    QLabel *lblPort;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *SettingsRTU)
    {
        if (SettingsRTU->objectName().isEmpty())
            SettingsRTU->setObjectName(QStringLiteral("SettingsRTU"));
        SettingsRTU->resize(220, 256);
        SettingsRTU->setMinimumSize(QSize(220, 256));
        SettingsRTU->setMaximumSize(QSize(220, 300));
        QIcon icon;
        icon.addFile(QStringLiteral(":/icons/options-16.png"), QSize(), QIcon::Normal, QIcon::Off);
        SettingsRTU->setWindowIcon(icon);
        SettingsRTU->setModal(true);
        verticalLayout = new QVBoxLayout(SettingsRTU);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        gridLayout = new QGridLayout();
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        cmbDev = new QComboBox(SettingsRTU);
        cmbDev->setObjectName(QStringLiteral("cmbDev"));
        cmbDev->setEditable(true);

        gridLayout->addWidget(cmbDev, 0, 1, 1, 1);

        lblDev = new QLabel(SettingsRTU);
        lblDev->setObjectName(QStringLiteral("lblDev"));

        gridLayout->addWidget(lblDev, 0, 0, 1, 1);

        lblBaud = new QLabel(SettingsRTU);
        lblBaud->setObjectName(QStringLiteral("lblBaud"));
        lblBaud->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);

        gridLayout->addWidget(lblBaud, 2, 0, 1, 1);

        sbPort = new QSpinBox(SettingsRTU);
        sbPort->setObjectName(QStringLiteral("sbPort"));
        sbPort->setMinimum(1);
        sbPort->setMaximum(128);

        gridLayout->addWidget(sbPort, 1, 1, 1, 1);

        cmbBaud = new QComboBox(SettingsRTU);
        cmbBaud->addItem(QStringLiteral("1200"));
        cmbBaud->addItem(QStringLiteral("2400"));
        cmbBaud->addItem(QStringLiteral("4800"));
        cmbBaud->addItem(QStringLiteral("9600"));
        cmbBaud->addItem(QStringLiteral("19200"));
        cmbBaud->addItem(QStringLiteral("38400"));
        cmbBaud->addItem(QString());
        cmbBaud->addItem(QString());
        cmbBaud->setObjectName(QStringLiteral("cmbBaud"));

        gridLayout->addWidget(cmbBaud, 2, 1, 1, 1);

        cmbDataBits = new QComboBox(SettingsRTU);
        cmbDataBits->insertItems(0, QStringList()
         << QStringLiteral("7")
         << QStringLiteral("8")
        );
        cmbDataBits->setObjectName(QStringLiteral("cmbDataBits"));

        gridLayout->addWidget(cmbDataBits, 3, 1, 1, 1);

        lblDataBits = new QLabel(SettingsRTU);
        lblDataBits->setObjectName(QStringLiteral("lblDataBits"));
        lblDataBits->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);

        gridLayout->addWidget(lblDataBits, 3, 0, 1, 1);

        lblRTS = new QLabel(SettingsRTU);
        lblRTS->setObjectName(QStringLiteral("lblRTS"));
        lblRTS->setText(QStringLiteral("RTS"));
        lblRTS->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);

        gridLayout->addWidget(lblRTS, 6, 0, 1, 1);

        lblStopBits = new QLabel(SettingsRTU);
        lblStopBits->setObjectName(QStringLiteral("lblStopBits"));
        lblStopBits->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);

        gridLayout->addWidget(lblStopBits, 4, 0, 1, 1);

        cmbRTS = new QComboBox(SettingsRTU);
        cmbRTS->setObjectName(QStringLiteral("cmbRTS"));

        gridLayout->addWidget(cmbRTS, 6, 1, 1, 1);

        cmbParity = new QComboBox(SettingsRTU);
        cmbParity->setObjectName(QStringLiteral("cmbParity"));

        gridLayout->addWidget(cmbParity, 5, 1, 1, 1);

        cmbStopBits = new QComboBox(SettingsRTU);
        cmbStopBits->insertItems(0, QStringList()
         << QStringLiteral("1")
         << QStringLiteral("1.5")
         << QStringLiteral("2")
        );
        cmbStopBits->setObjectName(QStringLiteral("cmbStopBits"));

        gridLayout->addWidget(cmbStopBits, 4, 1, 1, 1);

        lblParity = new QLabel(SettingsRTU);
        lblParity->setObjectName(QStringLiteral("lblParity"));
        lblParity->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);

        gridLayout->addWidget(lblParity, 5, 0, 1, 1);

        lblPort = new QLabel(SettingsRTU);
        lblPort->setObjectName(QStringLiteral("lblPort"));
        lblPort->setMinimumSize(QSize(0, 0));
        lblPort->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);

        gridLayout->addWidget(lblPort, 1, 0, 1, 1);


        verticalLayout->addLayout(gridLayout);

        buttonBox = new QDialogButtonBox(SettingsRTU);
        buttonBox->setObjectName(QStringLiteral("buttonBox"));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        verticalLayout->addWidget(buttonBox);

#ifndef QT_NO_SHORTCUT
        lblBaud->setBuddy(cmbBaud);
        lblDataBits->setBuddy(cmbDataBits);
        lblStopBits->setBuddy(cmbStopBits);
        lblParity->setBuddy(cmbParity);
#endif // QT_NO_SHORTCUT

        retranslateUi(SettingsRTU);
        QObject::connect(buttonBox, SIGNAL(accepted()), SettingsRTU, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), SettingsRTU, SLOT(reject()));

        cmbBaud->setCurrentIndex(0);
        cmbDataBits->setCurrentIndex(-1);
        cmbParity->setCurrentIndex(-1);
        cmbStopBits->setCurrentIndex(-1);


        QMetaObject::connectSlotsByName(SettingsRTU);
    } // setupUi

    void retranslateUi(QDialog *SettingsRTU)
    {
        SettingsRTU->setWindowTitle(QApplication::translate("SettingsRTU", "COM Port Ayarlar\304\261", Q_NULLPTR));
        cmbDev->clear();
        cmbDev->insertItems(0, QStringList()
         << QApplication::translate("SettingsRTU", "/dev/ttyS", Q_NULLPTR)
         << QApplication::translate("SettingsRTU", "/dev/ttyUSB", Q_NULLPTR)
        );
        lblDev->setText(QApplication::translate("SettingsRTU", "Serial device", Q_NULLPTR));
        lblBaud->setText(QApplication::translate("SettingsRTU", "Baud", Q_NULLPTR));
        cmbBaud->setItemText(6, QApplication::translate("SettingsRTU", "57600", Q_NULLPTR));
        cmbBaud->setItemText(7, QApplication::translate("SettingsRTU", "115200", Q_NULLPTR));

        lblDataBits->setText(QApplication::translate("SettingsRTU", "Data Bits", Q_NULLPTR));
        lblStopBits->setText(QApplication::translate("SettingsRTU", "Stop Bits", Q_NULLPTR));
        cmbParity->clear();
        cmbParity->insertItems(0, QStringList()
         << QApplication::translate("SettingsRTU", "None", Q_NULLPTR)
         << QApplication::translate("SettingsRTU", "Odd", Q_NULLPTR)
         << QApplication::translate("SettingsRTU", "Even", Q_NULLPTR)
        );
        lblParity->setText(QApplication::translate("SettingsRTU", "Parity", Q_NULLPTR));
        lblPort->setText(QApplication::translate("SettingsRTU", "Serial port", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class SettingsRTU: public Ui_SettingsRTU {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SETTINGSRTU_H
