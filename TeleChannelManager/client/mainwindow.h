#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMap>
#include "clientnetwork.h"
#include "telegrammanager.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void setAuthData(const QString &token, const QString &username);
    void setNetwork(ClientNetwork *net);

private slots:
    void addTokenClicked();
    void deleteTokenClicked();
    void onTokenSelectionChanged();
    void onChatsLoaded(const QList<QPair<QString, qint64>> &chats);
    void handleResponse(const QJsonObject &response);
    void sendPostToSelectedChats();
    void cleanComments();
    void onTelegramError(const QString &error);

private:
    Ui::MainWindow *ui;
    ClientNetwork *network;
    QString authToken;
    QString username;

    QMap<QString, TelegramManager*> telegramManagers; // Key: bot token

    void loadBotTokens();
    void updateTokensList(const QStringList &tokens);
    void loadChatsForSelectedToken();
    QString getCurrentSelectedToken() const;

    void showError(const QString &message);
    void showInfo(const QString &message);
};

#endif
