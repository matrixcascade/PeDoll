/********************************************************************************
** Form generated from reading UI file 'About.ui'
**
** Created by: Qt User Interface Compiler version 4.8.6
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_ABOUT_H
#define UI_ABOUT_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_Form_About
{
public:
    QVBoxLayout *verticalLayout_2;
    QHBoxLayout *horizontalLayout;
    QWidget *widget;
    QVBoxLayout *verticalLayout;
    QLabel *label;
    QPushButton *pushButton;

    void setupUi(QWidget *Form_About)
    {
        if (Form_About->objectName().isEmpty())
            Form_About->setObjectName(QString::fromUtf8("Form_About"));
        Form_About->resize(440, 116);
        verticalLayout_2 = new QVBoxLayout(Form_About);
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        widget = new QWidget(Form_About);
        widget->setObjectName(QString::fromUtf8("widget"));
        widget->setMinimumSize(QSize(91, 91));
        widget->setMaximumSize(QSize(91, 91));
        widget->setStyleSheet(QString::fromUtf8("background-image: url(:/PeDollc/Resources/52pojie.bmp);"));

        horizontalLayout->addWidget(widget);

        verticalLayout = new QVBoxLayout();
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        label = new QLabel(Form_About);
        label->setObjectName(QString::fromUtf8("label"));
        label->setMinimumSize(QSize(321, 0));

        verticalLayout->addWidget(label);

        pushButton = new QPushButton(Form_About);
        pushButton->setObjectName(QString::fromUtf8("pushButton"));

        verticalLayout->addWidget(pushButton);


        horizontalLayout->addLayout(verticalLayout);


        verticalLayout_2->addLayout(horizontalLayout);


        retranslateUi(Form_About);
        QObject::connect(pushButton, SIGNAL(clicked()), Form_About, SLOT(close()));

        QMetaObject::connectSlotsByName(Form_About);
    } // setupUi

    void retranslateUi(QWidget *Form_About)
    {
        Form_About->setWindowTitle(QApplication::translate("Form_About", "About", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("Form_About", "PeDoll v1.6.1 Code by DBinary from www.52pojie.cn", 0, QApplication::UnicodeUTF8));
        pushButton->setText(QApplication::translate("Form_About", "Ok", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class Form_About: public Ui_Form_About {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_ABOUT_H
