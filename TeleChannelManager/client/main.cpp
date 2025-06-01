#include <QApplication>
#include <QStyleFactory>
#include <QPalette>
#include "authwindow.h"
#include "mainwindow.h"
#include "clientnetwork.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);


#ifdef Q_OS_WIN
    QApplication::setStyle(QStyleFactory::create("Fusion"));


    QPalette darkPalette;
    darkPalette.setColor(QPalette::Window, QColor(53,53,53));
    darkPalette.setColor(QPalette::WindowText, Qt::white);
    darkPalette.setColor(QPalette::Base, QColor(25,25,25));
    darkPalette.setColor(QPalette::AlternateBase, QColor(53,53,53));
    darkPalette.setColor(QPalette::ToolTipBase, Qt::white);
    darkPalette.setColor(QPalette::ToolTipText, Qt::white);
    darkPalette.setColor(QPalette::Text, Qt::white);
    darkPalette.setColor(QPalette::Button, QColor(53,53,53));
    darkPalette.setColor(QPalette::ButtonText, Qt::white);
    darkPalette.setColor(QPalette::BrightText, Qt::red);
    darkPalette.setColor(QPalette::Link, QColor(42, 130, 218));
    darkPalette.setColor(QPalette::Highlight, QColor(42, 130, 218));
    darkPalette.setColor(QPalette::HighlightedText, Qt::black);
    qApp->setPalette(darkPalette);
#endif

    ClientNetwork *network = new ClientNetwork();
    AuthWindow authWindow;


    QObject::connect(&authWindow, &AuthWindow::authenticated, [&](const QString &token, const QString &username) {
        MainWindow *mainWindow = new MainWindow();
        mainWindow->setNetwork(network);
        mainWindow->setAuthData(token, username);
        mainWindow->show();
        authWindow.close();
    });

    authWindow.show();

    return a.exec();
}
