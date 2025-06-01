#ifndef AUTHWINDOW_H
#define AUTHWINDOW_H

#include <QWidget>
#include <QStackedWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QTimer>
#include "clientnetwork.h"

class AuthWindow : public QWidget
{
    Q_OBJECT
public:
    explicit AuthWindow(QWidget *parent = nullptr);
    ~AuthWindow();

signals:
    void authenticated(const QString &token, const QString &username);

private slots:
    void handleLoginResponse(const QJsonObject &response);
    void handleRegisterResponse(const QJsonObject &response);
    void showLoginForm();
    void showRegisterForm();
    void attemptLogin();
    void attemptRegister();
    void handleConnectionError(const QString &error);

private:
    ClientNetwork *network;

    QStackedWidget *stack;


    QWidget *loginWidget;
    QLineEdit *loginUsername;
    QLineEdit *loginPassword;
    QLabel *loginStatus;


    QWidget *registerWidget;
    QLineEdit *registerUsername;
    QLineEdit *registerPassword;
    QLineEdit *registerConfirmPassword;
    QLabel *registerStatus;

    void setupLoginForm();
    void setupRegisterForm();
};

#endif
