/********************************************************************************
** Form generated from reading UI file 'QtWidgetsApplicationFinal.ui'
**
** Created by: Qt User Interface Compiler version 5.15.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_QTWIDGETSAPPLICATIONFINAL_H
#define UI_QTWIDGETSAPPLICATIONFINAL_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_QtWidgetsApplicationFinalClass
{
public:
    QMenuBar *menuBar;
    QToolBar *mainToolBar;
    QWidget *centralWidget;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *QtWidgetsApplicationFinalClass)
    {
        if (QtWidgetsApplicationFinalClass->objectName().isEmpty())
            QtWidgetsApplicationFinalClass->setObjectName(QString::fromUtf8("QtWidgetsApplicationFinalClass"));
        QtWidgetsApplicationFinalClass->resize(600, 400);
        menuBar = new QMenuBar(QtWidgetsApplicationFinalClass);
        menuBar->setObjectName(QString::fromUtf8("menuBar"));
        QtWidgetsApplicationFinalClass->setMenuBar(menuBar);
        mainToolBar = new QToolBar(QtWidgetsApplicationFinalClass);
        mainToolBar->setObjectName(QString::fromUtf8("mainToolBar"));
        QtWidgetsApplicationFinalClass->addToolBar(mainToolBar);
        centralWidget = new QWidget(QtWidgetsApplicationFinalClass);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
        QtWidgetsApplicationFinalClass->setCentralWidget(centralWidget);
        statusBar = new QStatusBar(QtWidgetsApplicationFinalClass);
        statusBar->setObjectName(QString::fromUtf8("statusBar"));
        QtWidgetsApplicationFinalClass->setStatusBar(statusBar);

        retranslateUi(QtWidgetsApplicationFinalClass);

        QMetaObject::connectSlotsByName(QtWidgetsApplicationFinalClass);
    } // setupUi

    void retranslateUi(QMainWindow *QtWidgetsApplicationFinalClass)
    {
        QtWidgetsApplicationFinalClass->setWindowTitle(QCoreApplication::translate("QtWidgetsApplicationFinalClass", "QtWidgetsApplicationFinal", nullptr));
    } // retranslateUi

};

namespace Ui {
    class QtWidgetsApplicationFinalClass: public Ui_QtWidgetsApplicationFinalClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_QTWIDGETSAPPLICATIONFINAL_H
