#include "authwindow.h"
#include <QVBoxLayout>
#include <QFormLayout>
#include <QMessageBox>
#include <QTimer>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonValue>
#include <QDebug>

#include "mainwindow.h"
#include <QVBoxLayout>
#include <QFormLayout>
#include <QMessageBox>

AuthWindow::AuthWindow(QWidget *parent) : QWidget(parent)
{
    setWindowTitle("TeleChannel Client - Authorization");
    resize(400, 300);

    network = new ClientNetwork(this);
    connect(network, &ClientNetwork::responseReceived, this, &AuthWindow::handleLoginResponse);
    connect(network, &ClientNetwork::connectionError, this, &AuthWindow::handleConnectionError);
    connect(network, &ClientNetwork::connected, this, []() {
        qDebug() << "Connected to server";
    });

    stack = new QStackedWidget(this);

    setupLoginForm();
    setupRegisterForm();

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(stack);
    setLayout(layout);

    // Пытаемся подключиться к серверу
    network->connectToServer("localhost", 12345);
}

AuthWindow::~AuthWindow()
{
    delete network;
}

void AuthWindow::handleConnectionError(const QString &error)
{
    QMessageBox::warning(this, "Connection Error", error);
}

void AuthWindow::setupLoginForm()
{
    loginWidget = new QWidget();
    QFormLayout *form = new QFormLayout();

    loginUsername = new QLineEdit();
    loginPassword = new QLineEdit();
    loginPassword->setEchoMode(QLineEdit::Password);

    form->addRow("Username:", loginUsername);
    form->addRow("Password:", loginPassword);

    loginStatus = new QLabel();
    loginStatus->setWordWrap(true);

    QPushButton *loginButton = new QPushButton("Login");
    QPushButton *showRegisterButton = new QPushButton("Register");

    connect(loginButton, &QPushButton::clicked, this, &AuthWindow::attemptLogin);
    connect(showRegisterButton, &QPushButton::clicked, this, &AuthWindow::showRegisterForm);

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addWidget(loginButton);
    buttonLayout->addWidget(showRegisterButton);

    QVBoxLayout *mainLayout = new QVBoxLayout(loginWidget);
    mainLayout->addLayout(form);
    mainLayout->addWidget(loginStatus);
    mainLayout->addLayout(buttonLayout);

    stack->addWidget(loginWidget);
}

void AuthWindow::setupRegisterForm()
{
    registerWidget = new QWidget();
    QFormLayout *form = new QFormLayout();

    registerUsername = new QLineEdit();
    registerPassword = new QLineEdit();
    registerPassword->setEchoMode(QLineEdit::Password);
    registerConfirmPassword = new QLineEdit();
    registerConfirmPassword->setEchoMode(QLineEdit::Password);

    form->addRow("Username:", registerUsername);
    form->addRow("Password:", registerPassword);
    form->addRow("Confirm Password:", registerConfirmPassword);

    registerStatus = new QLabel();
    registerStatus->setWordWrap(true);

    QPushButton *registerButton = new QPushButton("Register");
    QPushButton *showLoginButton = new QPushButton("Back to Login");

    connect(registerButton, &QPushButton::clicked, this, &AuthWindow::attemptRegister);
    connect(showLoginButton, &QPushButton::clicked, this, &AuthWindow::showLoginForm);

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addWidget(registerButton);
    buttonLayout->addWidget(showLoginButton);

    QVBoxLayout *mainLayout = new QVBoxLayout(registerWidget);
    mainLayout->addLayout(form);
    mainLayout->addWidget(registerStatus);
    mainLayout->addLayout(buttonLayout);

    stack->addWidget(registerWidget);
}

void AuthWindow::showLoginForm() {
    stack->setCurrentWidget(loginWidget);
}

void AuthWindow::showRegisterForm() {
    stack->setCurrentWidget(registerWidget);
}

void AuthWindow::attemptLogin()
{
    if (loginUsername->text().isEmpty() || loginPassword->text().isEmpty()) {
        loginStatus->setText("Please fill in all fields.");
        return;
    }

    QJsonObject request;
    request["action"] = "login";
    request["username"] = loginUsername->text();
    request["password"] = loginPassword->text();

    network->sendRequest(request);
    loginStatus->setText("Logging in...");
}

void AuthWindow::attemptRegister()
{
    if (registerUsername->text().isEmpty() ||
        registerPassword->text().isEmpty() ||
        registerConfirmPassword->text().isEmpty())
    {
        registerStatus->setText("Please fill in all fields.");
        return;
    }

    if(registerPassword->text() != registerConfirmPassword->text()) {
        registerStatus->setText("Passwords do not match!");
        return;
    }

    if(registerPassword->text().length() < 6) {
        registerStatus->setText("Password must be at least 6 characters!");
        return;
    }

    QJsonObject request;
    request["action"] = "register";
    request["username"] = registerUsername->text();
    request["password"] = registerPassword->text();

    network->sendRequest(request);
    registerStatus->setText("Registering...");
}

void AuthWindow::handleLoginResponse(const QJsonObject &response)
{
    if(response.value("action").toString() == "login_response") {
        if(response.value("success").toBool()) {
            QString token = response.value("token").toString();
            QString username = loginUsername->text();

            // Отключаем сеть от текущего окна
            disconnect(network, &ClientNetwork::responseReceived, this, &AuthWindow::handleLoginResponse);
            disconnect(network, &ClientNetwork::responseReceived, this, &AuthWindow::handleRegisterResponse);

            MainWindow *mainWindow = new MainWindow();
            mainWindow->setAuthData(token, username);
            mainWindow->setNetwork(network);
            mainWindow->show();

            this->close();
        } else {
            loginStatus->setText("Login failed: " + response.value("message").toString());
        }
    }
}

void AuthWindow::handleRegisterResponse(const QJsonObject &response)
{
    if(response.value("action").toString() == "register_response") {
        if(response.value("success").toBool()) {
            registerStatus->setText("Registration successful! You can now log in.");
            registerUsername->clear();
            registerPassword->clear();
            registerConfirmPassword->clear();
            QTimer::singleShot(2000, this, &AuthWindow::showLoginForm);
        } else {
            registerStatus->setText("Registration failed: " + response.value("message").toString());
        }
    }
}
