/********************************************************************************
** Form generated from reading UI file 'TransferTool.ui'
**
** Created by: Qt User Interface Compiler version 4.8.6
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_TRANSFERTOOL_H
#define UI_TRANSFERTOOL_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_Form_TransFerTool
{
public:
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout;
    QLabel *label;
    QLineEdit *lineEdit_GBK;
    QHBoxLayout *horizontalLayout_2;
    QLabel *label_2;
    QLineEdit *lineEdit_Hex;
    QHBoxLayout *horizontalLayout_3;
    QLabel *label_3;
    QLineEdit *lineEdit_Dex;
    QHBoxLayout *horizontalLayout_4;
    QPushButton *pushButton_GTD;
    QPushButton *pushButton_HTD;

    void setupUi(QWidget *Form_TransFerTool)
    {
        if (Form_TransFerTool->objectName().isEmpty())
            Form_TransFerTool->setObjectName(QString::fromUtf8("Form_TransFerTool"));
        Form_TransFerTool->resize(754, 127);
        verticalLayout = new QVBoxLayout(Form_TransFerTool);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        label = new QLabel(Form_TransFerTool);
        label->setObjectName(QString::fromUtf8("label"));

        horizontalLayout->addWidget(label);

        lineEdit_GBK = new QLineEdit(Form_TransFerTool);
        lineEdit_GBK->setObjectName(QString::fromUtf8("lineEdit_GBK"));

        horizontalLayout->addWidget(lineEdit_GBK);


        verticalLayout->addLayout(horizontalLayout);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        label_2 = new QLabel(Form_TransFerTool);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        horizontalLayout_2->addWidget(label_2);

        lineEdit_Hex = new QLineEdit(Form_TransFerTool);
        lineEdit_Hex->setObjectName(QString::fromUtf8("lineEdit_Hex"));
        lineEdit_Hex->setReadOnly(false);

        horizontalLayout_2->addWidget(lineEdit_Hex);


        verticalLayout->addLayout(horizontalLayout_2);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        label_3 = new QLabel(Form_TransFerTool);
        label_3->setObjectName(QString::fromUtf8("label_3"));

        horizontalLayout_3->addWidget(label_3);

        lineEdit_Dex = new QLineEdit(Form_TransFerTool);
        lineEdit_Dex->setObjectName(QString::fromUtf8("lineEdit_Dex"));
        lineEdit_Dex->setReadOnly(true);

        horizontalLayout_3->addWidget(lineEdit_Dex);


        verticalLayout->addLayout(horizontalLayout_3);

        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setObjectName(QString::fromUtf8("horizontalLayout_4"));
        pushButton_GTD = new QPushButton(Form_TransFerTool);
        pushButton_GTD->setObjectName(QString::fromUtf8("pushButton_GTD"));

        horizontalLayout_4->addWidget(pushButton_GTD);

        pushButton_HTD = new QPushButton(Form_TransFerTool);
        pushButton_HTD->setObjectName(QString::fromUtf8("pushButton_HTD"));

        horizontalLayout_4->addWidget(pushButton_HTD);


        verticalLayout->addLayout(horizontalLayout_4);


        retranslateUi(Form_TransFerTool);

        QMetaObject::connectSlotsByName(Form_TransFerTool);
    } // setupUi

    void retranslateUi(QWidget *Form_TransFerTool)
    {
        Form_TransFerTool->setWindowTitle(QApplication::translate("Form_TransFerTool", "Transfer to DEX", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("Form_TransFerTool", "GBK", 0, QApplication::UnicodeUTF8));
        lineEdit_GBK->setText(QString());
        label_2->setText(QApplication::translate("Form_TransFerTool", "Hex", 0, QApplication::UnicodeUTF8));
        label_3->setText(QApplication::translate("Form_TransFerTool", "Dex", 0, QApplication::UnicodeUTF8));
        pushButton_GTD->setText(QApplication::translate("Form_TransFerTool", "GBK to Dex", 0, QApplication::UnicodeUTF8));
        pushButton_HTD->setText(QApplication::translate("Form_TransFerTool", "Hex to Dex", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class Form_TransFerTool: public Ui_Form_TransFerTool {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_TRANSFERTOOL_H
