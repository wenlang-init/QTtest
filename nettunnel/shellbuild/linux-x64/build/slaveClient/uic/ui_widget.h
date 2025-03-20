/********************************************************************************
** Form generated from reading UI file 'widget.ui'
**
** Created by: Qt User Interface Compiler version 6.8.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_WIDGET_H
#define UI_WIDGET_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_Widget
{
public:
    QVBoxLayout *verticalLayout_3;
    QHBoxLayout *horizontalLayout_5;
    QHBoxLayout *horizontalLayout_3;
    QHBoxLayout *horizontalLayout_2;
    QLabel *label;
    QLineEdit *lineEdit;
    QHBoxLayout *horizontalLayout;
    QLabel *label_3;
    QLineEdit *lineEditport;
    QHBoxLayout *horizontalLayout_4;
    QPushButton *pushButton;
    QSpacerItem *horizontalSpacer;
    QHBoxLayout *horizontalLayout_6;
    QLabel *label_4;
    QComboBox *comboBox;
    QPushButton *pushButtonsend;
    QHBoxLayout *horizontalLayout_7;
    QVBoxLayout *verticalLayout;
    QLabel *label_6;
    QTextEdit *textEditsend;
    QVBoxLayout *verticalLayout_2;
    QLabel *label_5;
    QTextEdit *textEdit;

    void setupUi(QWidget *Widget)
    {
        if (Widget->objectName().isEmpty())
            Widget->setObjectName("Widget");
        Widget->resize(800, 600);
        verticalLayout_3 = new QVBoxLayout(Widget);
        verticalLayout_3->setObjectName("verticalLayout_3");
        horizontalLayout_5 = new QHBoxLayout();
        horizontalLayout_5->setObjectName("horizontalLayout_5");
        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName("horizontalLayout_3");
        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName("horizontalLayout_2");
        label = new QLabel(Widget);
        label->setObjectName("label");

        horizontalLayout_2->addWidget(label);

        lineEdit = new QLineEdit(Widget);
        lineEdit->setObjectName("lineEdit");

        horizontalLayout_2->addWidget(lineEdit);


        horizontalLayout_3->addLayout(horizontalLayout_2);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName("horizontalLayout");
        label_3 = new QLabel(Widget);
        label_3->setObjectName("label_3");

        horizontalLayout->addWidget(label_3);

        lineEditport = new QLineEdit(Widget);
        lineEditport->setObjectName("lineEditport");

        horizontalLayout->addWidget(lineEditport);


        horizontalLayout_3->addLayout(horizontalLayout);


        horizontalLayout_5->addLayout(horizontalLayout_3);

        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setObjectName("horizontalLayout_4");

        horizontalLayout_5->addLayout(horizontalLayout_4);

        pushButton = new QPushButton(Widget);
        pushButton->setObjectName("pushButton");

        horizontalLayout_5->addWidget(pushButton);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        horizontalLayout_5->addItem(horizontalSpacer);


        verticalLayout_3->addLayout(horizontalLayout_5);

        horizontalLayout_6 = new QHBoxLayout();
        horizontalLayout_6->setObjectName("horizontalLayout_6");
        label_4 = new QLabel(Widget);
        label_4->setObjectName("label_4");

        horizontalLayout_6->addWidget(label_4);

        comboBox = new QComboBox(Widget);
        comboBox->setObjectName("comboBox");

        horizontalLayout_6->addWidget(comboBox);

        pushButtonsend = new QPushButton(Widget);
        pushButtonsend->setObjectName("pushButtonsend");
        pushButtonsend->setMinimumSize(QSize(0, 20));
        pushButtonsend->setMaximumSize(QSize(16777215, 2030));

        horizontalLayout_6->addWidget(pushButtonsend);


        verticalLayout_3->addLayout(horizontalLayout_6);

        horizontalLayout_7 = new QHBoxLayout();
        horizontalLayout_7->setObjectName("horizontalLayout_7");
        verticalLayout = new QVBoxLayout();
        verticalLayout->setObjectName("verticalLayout");
        label_6 = new QLabel(Widget);
        label_6->setObjectName("label_6");
        label_6->setMinimumSize(QSize(0, 20));
        label_6->setMaximumSize(QSize(16777215, 20));

        verticalLayout->addWidget(label_6);

        textEditsend = new QTextEdit(Widget);
        textEditsend->setObjectName("textEditsend");

        verticalLayout->addWidget(textEditsend);


        horizontalLayout_7->addLayout(verticalLayout);

        verticalLayout_2 = new QVBoxLayout();
        verticalLayout_2->setObjectName("verticalLayout_2");
        label_5 = new QLabel(Widget);
        label_5->setObjectName("label_5");
        label_5->setMinimumSize(QSize(0, 20));
        label_5->setMaximumSize(QSize(16777215, 20));

        verticalLayout_2->addWidget(label_5);

        textEdit = new QTextEdit(Widget);
        textEdit->setObjectName("textEdit");

        verticalLayout_2->addWidget(textEdit);


        horizontalLayout_7->addLayout(verticalLayout_2);


        verticalLayout_3->addLayout(horizontalLayout_7);


        retranslateUi(Widget);

        QMetaObject::connectSlotsByName(Widget);
    } // setupUi

    void retranslateUi(QWidget *Widget)
    {
        Widget->setWindowTitle(QCoreApplication::translate("Widget", "Widget", nullptr));
        label->setText(QCoreApplication::translate("Widget", "\344\270\255\345\277\203\346\234\215\345\212\241\345\231\250\345\234\260\345\235\200:", nullptr));
        lineEdit->setText(QCoreApplication::translate("Widget", "127.0.0.1", nullptr));
        label_3->setText(QCoreApplication::translate("Widget", "\347\253\257\345\217\243:", nullptr));
        lineEditport->setText(QCoreApplication::translate("Widget", "12345", nullptr));
        pushButton->setText(QCoreApplication::translate("Widget", "\350\277\236\346\216\245", nullptr));
        label_4->setText(QCoreApplication::translate("Widget", "\346\234\215\345\212\241\345\231\250\345\234\260\345\235\200:", nullptr));
        pushButtonsend->setText(QCoreApplication::translate("Widget", "\345\217\221\351\200\201", nullptr));
        label_6->setText(QCoreApplication::translate("Widget", "\345\217\221\351\200\201", nullptr));
        label_5->setText(QCoreApplication::translate("Widget", "\346\216\245\346\224\266", nullptr));
    } // retranslateUi

};

namespace Ui {
    class Widget: public Ui_Widget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_WIDGET_H
