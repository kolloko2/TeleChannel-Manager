/**
 * @file test_authwindow.cpp
 * @brief Unit tests for AuthWindow class of TeleChannel Manager client.
 * @author Vladimir Ilyin
 * @date 2025-06-22
 */

#include <QtTest/QtTest>
#include <QJsonObject>
#include "authwindow.h"
#include "clientnetwork.h"

/**
 * @class TestAuthWindow
 * @brief Test suite for AuthWindow class.
 */
class TestAuthWindow : public QObject {
    Q_OBJECT

private:
    AuthWindow *authWindow;

private slots:
    /**
     * @brief Initialize test case.
     */
    void initTestCase() {
        authWindow = new AuthWindow(nullptr);
        QVERIFY2(authWindow != nullptr, "AuthWindow instance should be created.");
    }

    /**
     * @brief Test login form setup.
     */
    void testLoginFormSetup() {
        QVERIFY2(authWindow->findChild<QLineEdit*>("loginUsername") != nullptr, "Login username field should exist.");
        QVERIFY2(authWindow->findChild<QLineEdit*>("loginPassword") != nullptr, "Login password field should exist.");
        QVERIFY2(authWindow->findChild<QLabel*>("loginStatus") != nullptr, "Login status label should exist.");
    }

    /**
     * @brief Test register form setup.
     */
    void testRegisterFormSetup() {
        authWindow->showRegisterForm();
        QVERIFY2(authWindow->findChild<QLineEdit*>("registerUsername") != nullptr, "Register username field should exist.");
        QVERIFY2(authWindow->findChild<QLineEdit*>("registerPassword") != nullptr, "Register password field should exist.");
        QVERIFY2(authWindow->findChild<QLineEdit*>("registerConfirmPassword") != nullptr, "Register confirm password field should exist.");
        QVERIFY2(authWindow->findChild<QLabel*>("registerStatus") != nullptr, "Register status label should exist.");
    }

    /**
     * @brief Test login attempt with empty fields.
     */
    void testLoginEmptyFields() {
        authWindow->showLoginForm();
        QLineEdit *loginUsername = authWindow->findChild<QLineEdit*>("loginUsername");
        QLineEdit *loginPassword = authWindow->findChild<QLineEdit*>("loginPassword");
        QLabel *loginStatus = authWindow->findChild<QLabel*>("loginStatus");

        loginUsername->setText("");
        loginPassword->setText("");
        authWindow->attemptLogin();

        QCOMPARE(loginStatus->text(), QString("Please fill in all fields."), "Empty fields should show error.");
    }

    /**
     * @brief Test registration with mismatching passwords.
     */
    void testRegisterMismatchingPasswords() {
        authWindow->showRegisterForm();
        QLineEdit *registerUsername = authWindow->findChild<QLineEdit*>("registerUsername");
        QLineEdit *registerPassword = authWindow->findChild<QLineEdit*>("registerPassword");
        QLineEdit *registerConfirmPassword = authWindow->findChild<QLineEdit*>("registerConfirmPassword");
        QLabel *registerStatus = authWindow->findChild<QLabel*>("registerStatus");

        registerUsername->setText("testuser");
        registerPassword->setText("pass123");
        registerConfirmPassword->setText("pass456");
        authWindow->attemptRegister();

        QCOMPARE(registerStatus->text(), QString("Passwords do not match!"), "Mismatching passwords should show error.");
    }

    /**
     * @brief Test registration with short password.
     */
    void testRegisterShortPassword() {
        authWindow->showRegisterForm();
        QLineEdit *registerUsername = authWindow->findChild<QLineEdit*>("registerUsername");
        QLineEdit *registerPassword = authWindow->findChild<QLineEdit*>("registerPassword");
        QLineEdit *registerConfirmPassword = authWindow->findChild<QLineEdit*>("registerConfirmPassword");
        QLabel *registerStatus = authWindow->findChild<QLabel*>("registerStatus");

        registerUsername->setText("testuser");
        registerPassword->setText("short");
        registerConfirmPassword->setText("short");
        authWindow->attemptRegister();

        QCOMPARE(registerStatus->text(), QString("Password must be at least 6 characters!"), "Short password should show error.");
    }

    /**
     * @brief Cleanup test case.
     */
    void cleanupTestCase() {
        delete authWindow;
    }
};

QTEST_MAIN(TestAuthWindow)
#include "test_authwindow.moc"