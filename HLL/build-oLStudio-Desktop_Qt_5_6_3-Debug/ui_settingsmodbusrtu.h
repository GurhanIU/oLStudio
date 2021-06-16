/********************************************************************************
** Form generated from reading UI file 'settingsmodbusrtu.ui'
**
** Created by: Qt User Interface Compiler version 5.6.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SETTINGSMODBUSRTU_H
#define UI_SETTINGSMODBUSRTU_H

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

class Ui_SettingsModbusRTU
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

    void setupUi(QDialog *SettingsModbusRTU)
    {
        if (SettingsModbusRTU->objectName().isEmpty())
            SettingsModbusRTU->setObjectName(QStringLiteral("SettingsModbusRTU"));
        SettingsModbusRTU->resize(220, 256);
        SettingsModbusRTU->setMinimumSize(QSize(220, 256));
        SettingsModbusRTU->setMaximumSize(QSize(220, 300));
        QIcon icon;
        icon.addFile(QStringLiteral(":/icons/options-16.png"), QSize(), QIcon::Normal, QIcon::Off);
        SettingsModbusRTU->setWindowIcon(icon);
        SettingsModbusRTU->setModal(true);
        verticalLayout = new QVBoxLayout(SettingsModbusRTU);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        gridLayout = new QGridLayout();
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        cmbDev = new QComboBox(SettingsModbusRTU);
        cmbDev->setObjectName(QStringLiteral("cmbDev"));
        cmbDev->setEditable(true);

        gridLayout->addWidget(cmbDev, 0, 1, 1, 1);

        lblDev = new QLabel(SettingsModbusRTU);
        lblDev->setObjectName(QStringLiteral("lblDev"));

        gridLayout->addWidget(lblDev, 0, 0, 1, 1);

        lblBaud = new QLabel(SettingsModbusRTU);
        lblBaud->setObjectName(QStringLiteral("lblBaud"));
        lblBaud->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);

        gridLayout->addWidget(lblBaud, 2, 0, 1, 1);

        sbPort = new QSpinBox(SettingsModbusRTU);
        sbPort->setObjectName(QStringLiteral("sbPort"));
        sbPort->setMinimum(1);
        sbPort->setMaximum(128);

        gridLayout->addWidget(sbPort, 1, 1, 1, 1);

        cmbBaud = new QComboBox(SettingsModbusRTU);
        cmbBaud->insertItems(0, QStringList()
         << QStringLiteral("1200")
         << QStringLiteral("2400")
         << QStringLiteral("4800")
         << QStringLiteral("9600")
         << QStringLiteral("19200")
         << QStringLiteral("38400")
        );
        cmbBaud->setObjectName(QStringLiteral("cmbBaud"));

        gridLayout->addWidget(cmbBaud, 2, 1, 1, 1);

        cmbDataBits = new QComboBox(SettingsModbusRTU);
        cmbDataBits->insertItems(0, QStringList()
         << QStringLiteral("7")
         << QStringLiteral("8")
        );
        cmbDataBits->setObjectName(QStringLiteral("cmbDataBits"));

        gridLayout->addWidget(cmbDataBits, 3, 1, 1, 1);

        lblDataBits = new QLabel(SettingsModbusRTU);
        lblDataBits->setObjectName(QStringLiteral("lblDataBits"));
        lblDataBits->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);

        gridLayout->addWidget(lblDataBits, 3, 0, 1, 1);

        lblRTS = new QLabel(SettingsModbusRTU);
        lblRTS->setObjectName(QStringLiteral("lblRTS"));
        lblRTS->setText(QStringLiteral("RTS"));
        lblRTS->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);

        gridLayout->addWidget(lblRTS, 6, 0, 1, 1);

        lblStopBits = new QLabel(SettingsModbusRTU);
        lblStopBits->setObjectName(QStringLiteral("lblStopBits"));
        lblStopBits->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);

        gridLayout->addWidget(lblStopBits, 4, 0, 1, 1);

        cmbRTS = new QComboBox(SettingsModbusRTU);
        cmbRTS->setObjectName(QStringLiteral("cmbRTS"));

        gridLayout->addWidget(cmbRTS, 6, 1, 1, 1);

        cmbParity = new QComboBox(SettingsModbusRTU);
        cmbParity->setObjectName(QStringLiteral("cmbParity"));

        gridLayout->addWidget(cmbParity, 5, 1, 1, 1);

        cmbStopBits = new QComboBox(SettingsModbusRTU);
        cmbStopBits->insertItems(0, QStringList()
         << QStringLiteral("1")
         << QStringLiteral("1.5")
         << QStringLiteral("2")
        );
        cmbStopBits->setObjectName(QStringLiteral("cmbStopBits"));

        gridLayout->addWidget(cmbStopBits, 4, 1, 1, 1);

        lblParity = new QLabel(SettingsModbusRTU);
        lblParity->setObjectName(QStringLiteral("lblParity"));
        lblParity->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);

        gridLayout->addWidget(lblParity, 5, 0, 1, 1);

        lblPort = new QLabel(SettingsModbusRTU);
        lblPort->setObjectName(QStringLiteral("lblPort"));
        lblPort->setMinimumSize(QSize(0, 0));
        lblPort->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);

        gridLayout->addWidget(lblPort, 1, 0, 1, 1);


        verticalLayout->addLayout(gridLayout);

        buttonBox = new QDialogButtonBox(SettingsModbusRTU);
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

        retranslateUi(SettingsModbusRTU);
        QObject::connect(buttonBox, SIGNAL(accepted()), SettingsModbusRTU, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), SettingsModbusRTU, SLOT(reject()));

        cmbBaud->setCurrentIndex(0);
        cmbDataBits->setCurrentIndex(-1);
        cmbParity->setCurrentIndex(-1);
        cmbStopBits->setCurrentIndex(-1);


        QMetaObject::connectSlotsByName(SettingsModbusRTU);
    } // setupUi

    void retranslateUi(QDialog *SettingsModbusRTU)
    {
        SettingsModbusRTU->setWindowTitle(QApplication::translate("SettingsModbusRTU", "Modbus RTU Settings", Q_NULLPTR));
        cmbDev->clear();
        cmbDev->insertItems(0, QStringList()
         << QApplication::translate("SettingsModbusRTU", "/dev/ttyS", Q_NULLPTR)
         << QApplication::translate("SettingsModbusRTU", "/dev/ttyUSB", Q_NULLPTR)
        );
        lblDev->setText(QApplication::translate("SettingsModbusRTU", "Serial device", Q_NULLPTR));
        lblBaud->setText(QApplication::translate("SettingsModbusRTU", "Baud", Q_NULLPTR));
        lblDataBits->setText(QApplication::translate("SettingsModbusRTU", "Data Bits", Q_NULLPTR));
        lblStopBits->setText(QApplication::translate("SettingsModbusRTU", "Stop Bits", Q_NULLPTR));
        cmbParity->clear();
        cmbParity->insertItems(0, QStringList()
         << QApplication::translate("SettingsModbusRTU", "None", Q_NULLPTR)
         << QApplication::translate("SettingsModbusRTU", "Odd", Q_NULLPTR)
         << QApplication::translate("SettingsModbusRTU", "Even", Q_NULLPTR)
        );
        lblParity->setText(QApplication::translate("SettingsModbusRTU", "Parity", Q_NULLPTR));
        lblPort->setText(QApplication::translate("SettingsModbusRTU", "Serial port", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class SettingsModbusRTU: public Ui_SettingsModbusRTU {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SETTINGSMODBUSRTU_H
