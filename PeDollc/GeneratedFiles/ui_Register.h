/********************************************************************************
** Form generated from reading UI file 'Register.ui'
**
** Created by: Qt User Interface Compiler version 4.8.6
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_REGISTER_H
#define UI_REGISTER_H

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

class Ui_Form_Register
{
public:
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout_4;
    QHBoxLayout *horizontalLayout_3;
    QLabel *label_2;
    QLabel *label_MacID;
    QHBoxLayout *horizontalLayout_2;
    QLabel *label;
    QLineEdit *lineEdit_Key;
    QHBoxLayout *horizontalLayout;
    QSpacerItem *horizontalSpacer;
    QPushButton *pushButton_ok;
    QPushButton *pushButton_Buy;
    QPushButton *pushButton_Cancel;

    void setupUi(QWidget *Form_Register)
    {
        if (Form_Register->objectName().isEmpty())
            Form_Register->setObjectName(QString::fromUtf8("Form_Register"));
        Form_Register->resize(379, 91);
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/PeDollc/Resources/key.png"), QSize(), QIcon::Normal, QIcon::Off);
        Form_Register->setWindowIcon(icon);
        verticalLayout = new QVBoxLayout(Form_Register);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setObjectName(QString::fromUtf8("horizontalLayout_4"));
        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        label_2 = new QLabel(Form_Register);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        horizontalLayout_3->addWidget(label_2);

        label_MacID = new QLabel(Form_Register);
        label_MacID->setObjectName(QString::fromUtf8("label_MacID"));

        horizontalLayout_3->addWidget(label_MacID);


        horizontalLayout_4->addLayout(horizontalLayout_3);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        label = new QLabel(Form_Register);
        label->setObjectName(QString::fromUtf8("label"));

        horizontalLayout_2->addWidget(label);

        lineEdit_Key = new QLineEdit(Form_Register);
        lineEdit_Key->setObjectName(QString::fromUtf8("lineEdit_Key"));
        lineEdit_Key->setMaxLength(16);

        horizontalLayout_2->addWidget(lineEdit_Key);


        horizontalLayout_4->addLayout(horizontalLayout_2);


        verticalLayout->addLayout(horizontalLayout_4);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        pushButton_ok = new QPushButton(Form_Register);
        pushButton_ok->setObjectName(QString::fromUtf8("pushButton_ok"));

        horizontalLayout->addWidget(pushButton_ok);

        pushButton_Buy = new QPushButton(Form_Register);
        pushButton_Buy->setObjectName(QString::fromUtf8("pushButton_Buy"));

        horizontalLayout->addWidget(pushButton_Buy);

        pushButton_Cancel = new QPushButton(Form_Register);
        pushButton_Cancel->setObjectName(QString::fromUtf8("pushButton_Cancel"));

        horizontalLayout->addWidget(pushButton_Cancel);


        verticalLayout->addLayout(horizontalLayout);


        retranslateUi(Form_Register);
        QObject::connect(pushButton_Cancel, SIGNAL(clicked()), Form_Register, SLOT(close()));

        QMetaObject::connectSlotsByName(Form_Register);
    } // setupUi

    void retranslateUi(QWidget *Form_Register)
    {
        Form_Register->setWindowTitle(QApplication::translate("Form_Register", "Regist", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("Form_Register", "Mac ID\357\274\232", 0, QApplication::UnicodeUTF8));
        label_MacID->setText(QApplication::translate("Form_Register", "FFFFFF", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("Form_Register", "Key", 0, QApplication::UnicodeUTF8));
        pushButton_ok->setText(QApplication::translate("Form_Register", "OK", 0, QApplication::UnicodeUTF8));
        pushButton_Buy->setText(QApplication::translate("Form_Register", "Buy", 0, QApplication::UnicodeUTF8));
        pushButton_Cancel->setText(QApplication::translate("Form_Register", "Cancel", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class Form_Register: public Ui_Form_Register {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_REGISTER_H
