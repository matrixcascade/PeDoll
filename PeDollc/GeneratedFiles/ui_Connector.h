/********************************************************************************
** Form generated from reading UI file 'Connector.ui'
**
** Created by: Qt User Interface Compiler version 4.8.6
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CONNECTOR_H
#define UI_CONNECTOR_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_Form_Connector
{
public:
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout_2;
    QLabel *label;
    QLineEdit *lineEdit;
    QHBoxLayout *horizontalLayout;
    QSpacerItem *horizontalSpacer;
    QPushButton *pushButton_ok;
    QPushButton *pushButton_Cancel;

    void setupUi(QWidget *Form_Connector)
    {
        if (Form_Connector->objectName().isEmpty())
            Form_Connector->setObjectName(QString::fromUtf8("Form_Connector"));
        Form_Connector->resize(257, 71);
        verticalLayout = new QVBoxLayout(Form_Connector);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        label = new QLabel(Form_Connector);
        label->setObjectName(QString::fromUtf8("label"));
        label->setMinimumSize(QSize(32, 0));

        horizontalLayout_2->addWidget(label);

        lineEdit = new QLineEdit(Form_Connector);
        lineEdit->setObjectName(QString::fromUtf8("lineEdit"));
        lineEdit->setAlignment(Qt::AlignCenter);

        horizontalLayout_2->addWidget(lineEdit);


        verticalLayout->addLayout(horizontalLayout_2);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        pushButton_ok = new QPushButton(Form_Connector);
        pushButton_ok->setObjectName(QString::fromUtf8("pushButton_ok"));

        horizontalLayout->addWidget(pushButton_ok);

        pushButton_Cancel = new QPushButton(Form_Connector);
        pushButton_Cancel->setObjectName(QString::fromUtf8("pushButton_Cancel"));

        horizontalLayout->addWidget(pushButton_Cancel);


        verticalLayout->addLayout(horizontalLayout);


        retranslateUi(Form_Connector);
        QObject::connect(pushButton_Cancel, SIGNAL(clicked()), Form_Connector, SLOT(hide()));

        QMetaObject::connectSlotsByName(Form_Connector);
    } // setupUi

    void retranslateUi(QWidget *Form_Connector)
    {
        Form_Connector->setWindowTitle(QApplication::translate("Form_Connector", "Connection", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("Form_Connector", "IPv4", 0, QApplication::UnicodeUTF8));
        lineEdit->setText(QApplication::translate("Form_Connector", "127.0.0.1", 0, QApplication::UnicodeUTF8));
        pushButton_ok->setText(QApplication::translate("Form_Connector", "ok", 0, QApplication::UnicodeUTF8));
        pushButton_Cancel->setText(QApplication::translate("Form_Connector", "cancel", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class Form_Connector: public Ui_Form_Connector {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CONNECTOR_H
