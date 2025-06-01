#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QJsonObject>
#include <QJsonArray>
#include <QMessageBox>
#include <QListWidgetItem>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    network(nullptr)
{
    ui->setupUi(this);

    connect(ui->btnAddToken, &QPushButton::clicked, this, &MainWindow::addTokenClicked);
    connect(ui->btnDeleteToken, &QPushButton::clicked, this, &MainWindow::deleteTokenClicked);
    connect(ui->listBotTokens, &QListWidget::itemSelectionChanged,
            this, &MainWindow::onTokenSelectionChanged);
    connect(ui->btnSendPost, &QPushButton::clicked, this, &MainWindow::sendPostToSelectedChats);
    connect(ui->btnCleanComments, &QPushButton::clicked, this, &MainWindow::cleanComments);
}

MainWindow::~MainWindow()
{
    qDeleteAll(telegramManagers);
    delete ui;
}

void MainWindow::setAuthData(const QString &token, const QString &username)
{
    authToken = token;
    this->username = username;
    setWindowTitle("TeleChannel Client - " + username);

    if (network) {
        loadBotTokens();
    } else {
        qDebug() << "Network не установлен при вызове setAuthData";
    }
}

void MainWindow::setNetwork(ClientNetwork *net)
{
    network = net;
    if (network) {
        connect(network, &ClientNetwork::responseReceived, this, &MainWindow::handleResponse);
        if (!authToken.isEmpty()) {
            loadBotTokens();
        }
    }
}

void MainWindow::loadBotTokens()
{
    if (!network) {
        showError("Сетевой клиент не инициализирован");
        qDebug() << "Ошибка: network не установлен в loadBotTokens";
        return;
    }

    if (authToken.isEmpty()) {
        showError("Токен авторизации не установлен");
        qDebug() << "Ошибка: authToken пуст в loadBotTokens";
        return;
    }

    QJsonObject request;
    request["action"] = "get_bot_tokens";
    request["username"] = username;
    request["token"] = authToken;

    network->sendRequest(request);
    qDebug() << "Отправлен запрос на загрузку бот-токенов для пользователя:" << username;
}

void MainWindow::updateTokensList(const QStringList &tokens)
{
    ui->listBotTokens->clear();
    ui->listBotTokens->addItems(tokens);
    qDebug() << "Список токенов обновлён, количество:" << tokens.size();
}

void MainWindow::addTokenClicked()
{
    QString newToken = ui->txtNewToken->text().trimmed();
    if (newToken.isEmpty()) {
        showError("Токен не может быть пустым!");
        return;
    }

    QJsonObject request;
    request["action"] = "add_bot_token";
    request["username"] = username;
    request["bot_token"] = newToken;
    request["token"] = authToken;

    network->sendRequest(request);
    ui->txtNewToken->clear();
}

void MainWindow::deleteTokenClicked()
{
    QListWidgetItem *selectedItem = ui->listBotTokens->currentItem();
    if (!selectedItem) {
        showError("Пожалуйста, выберите токен для удаления!");
        return;
    }

    QString tokenToDelete = selectedItem->text().trimmed();
    if (tokenToDelete.isEmpty()) {
        showError("Выбранный токен недействителен!");
        return;
    }

    QJsonObject request;
    request["action"] = "remove_bot_token";
    request["username"] = username;
    request["bot_token"] = tokenToDelete;
    request["token"] = authToken;

    network->sendRequest(request);
}

void MainWindow::onTokenSelectionChanged()
{
    loadChatsForSelectedToken();
}

void MainWindow::loadChatsForSelectedToken()
{
    QList<QListWidgetItem*> selectedItems = ui->listBotTokens->selectedItems();
    if (selectedItems.isEmpty()) {
        ui->listChats->clear();
        showError("Пожалуйста, выберите бот-токен");
        qDebug() << "Ошибка: не выбран токен для загрузки чатов";
        return;
    }

    QString token = selectedItems.first()->text().trimmed();
    if (token.isEmpty()) {
        showError("Выбранный токен пуст");
        qDebug() << "Ошибка: выбранный токен пуст";
        return;
    }

    if (!telegramManagers.contains(token)) {
        TelegramManager *manager = new TelegramManager(this);
        connect(manager, &TelegramManager::chatsLoaded, this, &MainWindow::onChatsLoaded);
        connect(manager, &TelegramManager::errorOccurred, this, &MainWindow::onTelegramError);
        manager->initialize(token);
        telegramManagers.insert(token, manager);
        qDebug() << "Создан TelegramManager для токена:" << token;
    }

    ui->listChats->clear();
    telegramManagers[token]->getChats();
    qDebug() << "Вызван getChats для токена:" << token;
}

void MainWindow::onChatsLoaded(const QList<QPair<QString, qint64>> &chats)
{
    qDebug() << "onChatsLoaded вызван, получено" << chats.size() << "каналов";
    ui->listChats->clear();

    for (const auto &chat : chats) {
        QString chatName = chat.first;
        qint64 chatId = chat.second;
        qDebug() << "Добавление чата:" << chatName << "(ID:" << chatId << ")";
        QListWidgetItem *item = new QListWidgetItem(chatName);
        item->setData(Qt::UserRole, QVariant(chatId));
        ui->listChats->addItem(item);
    }

    qDebug() << "Загружено каналов в listChats:" << ui->listChats->count();
    ui->listChats->update();
}

void MainWindow::handleResponse(const QJsonObject &response)
{
    QString message = response.value("action").toString();

    if (message == "get_bot_tokens_response") {
        if (response.value("success").toBool()) {
            QJsonArray tokenArray = response.value("tokens").toArray();
            QStringList tokens;
            for (const QJsonValue &val : tokenArray) {
                tokens.append(val.toString());
            }
            updateTokensList(tokens);
            showInfo("Токены успешно загружены");
        } else {
            showError("Не удалось загрузить токены: " + response.value("message").toString());
        }
    }
    else if (message == "add_bot_response") {
        if (response.value("success").toBool()) {
            showInfo("Токен успешно добавлен!");
            loadBotTokens();
        } else {
            showError("Не удалось добавить токен: " + response.value("message").toString());
        }
    }
    else if (message == "remove_bot_response") {
        if (response.value("success").toBool()) {
            QString token = response.value("bot_token").toString();
            if (telegramManagers.contains(token)) {
                delete telegramManagers[token];
                telegramManagers.remove(token);
            }
            showInfo("Токен успешно удалён!");
            loadBotTokens();
        } else {
            showError("Не удалось удалить токен: " + response.value("message").toString());
        }
    }
    qDebug() << "Получен ответ от сервера, действие:" << message;
}

void MainWindow::sendPostToSelectedChats()
{
    QString postText = ui->textEditPost->toPlainText().trimmed();
    if (postText.isEmpty()) {
        showError("Текст поста не может быть пустым");
        return;
    }

    QList<QListWidgetItem*> selectedItems = ui->listChats->selectedItems();
    if (selectedItems.isEmpty()) {
        showError("Выберите хотя бы один канал");
        return;
    }

    QString token = getCurrentSelectedToken();
    if (!telegramManagers.contains(token)) {
        showError("Менеджер для данного токена не найден");
        return;
    }

    TelegramManager *manager = telegramManagers[token];

    for (QListWidgetItem *item : selectedItems) {
        qint64 chatId = item->data(Qt::UserRole).toLongLong();
        manager->sendMessage(chatId, postText);
    }

    showInfo("Пост отправлен в выбранные каналы");
}

void MainWindow::cleanComments()
{
    int minWords = ui->spinMinWords->value();

    QString token = getCurrentSelectedToken();
    if (!telegramManagers.contains(token)) {
        showError("Токен не выбран");
        return;
    }

    telegramManagers[token]->deleteCommentsWithFilter(minWords, QStringList());
}

QString MainWindow::getCurrentSelectedToken() const
{
    QList<QListWidgetItem*> selectedItems = ui->listBotTokens->selectedItems();
    if (selectedItems.isEmpty())
        return QString();
    return selectedItems.first()->text().trimmed();
}

void MainWindow::onTelegramError(const QString &error)
{
    showError("Ошибка Telegram API: " + error);
    qDebug() << "Ошибка Telegram API:" << error;
}

void MainWindow::showError(const QString &message)
{
    QMessageBox::critical(this, "Ошибка", message);
}

void MainWindow::showInfo(const QString &message)
{
    QMessageBox::information(this, "Информация", message);
}
