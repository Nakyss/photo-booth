#include "settings.h"
#include "ui_settings.h"
#include <QJsonArray>
#include <QStandardPaths>
#include <QDir>

Settings::Settings(WebSocketClient *itsWs, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Settings)
    , ws(itsWs)
{
    ui->setupUi(this);

    roomList = new QVector<QString>;

    connect(ui->validateButton, &QPushButton::clicked, this, &Settings::addRoom);
    connect(ui->textInput, &QLineEdit::returnPressed, this, &Settings::addRoom);

    QString folderPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QString filePath = folderPath + "/rooms.json";

    QDir dir(folderPath);
    if (!dir.exists()) {
        dir.mkpath(".");
    }

    jsonFile = new QFile(filePath);
    if (jsonFile->open(QIODevice::ReadOnly))
    {
        QByteArray bytes = jsonFile->readAll();
        jsonFile->close();

        QJsonParseError JsonError;
        QJsonDocument document = QJsonDocument::fromJson(bytes, &JsonError);
        if (JsonError.error != QJsonParseError::NoError){
            qDebug() << "Erreur de parsing JSON :" << JsonError.errorString();
            qDebug() << "Erreur à l'offset (position) :" << JsonError.offset;

            // Affiche le texte autour de l'erreur pour comprendre où ça coince
            int errorPos = JsonError.offset;
            qDebug() << "Extrait proche de l'erreur :" << bytes.mid(qMax(0, errorPos - 10), 20);
            return;
        }

        if (document.isArray()) {
            QJsonArray array = document.array();

            for (const QJsonValue &value : array) {
                if (value.isString()) {
                    roomList->append(value.toString());
                }
            }
            renderList();
        }
    }
}

Settings::~Settings()
{
    delete ui;
}

void Settings::addRoom()
{
    QString room = getInput();
    if (room.isEmpty()) return;
    for (const QString &roomName : *roomList) {
        if (room == roomName) return;
    }
    roomList->append(room);
    renderList();

    saveRooms();
}

void Settings::removeRoom(QString room){
    roomList->removeAll(room);
    renderList();
    saveRooms();
}

QString Settings::getInput()
{
    QString text = ui->textInput->text();
    ui->textInput->clear();
    return text;
}

void Settings::clearLayout(QLayout *layout) {
    if (!layout) return;

    while (QLayoutItem *item = layout->takeAt(0)) {
        if (QWidget *widget = item->widget()) {
            widget->deleteLater();
        }

        else if (QLayout *childLayout = item->layout()) {
            clearLayout(childLayout);
        }

        delete item;
    }
}

void Settings::saveRooms()
{
    QJsonArray roomsArray;
    for (const QString &roomName : *roomList) {
        roomsArray.append(roomName);
    }
    ws->sendRooms(roomsArray);

    if (jsonFile->open(QIODevice::ReadWrite | QIODevice::Truncate)){
        QJsonDocument document;
        document.setArray(roomsArray);
        jsonFile->write(document.toJson(QJsonDocument::JsonFormat::Compact));
        jsonFile->encodeName("UTF-8");
        jsonFile->close();
    }
    else{
        qDebug() << "Erreur d'ouverture :" << jsonFile->errorString();
        qDebug() << "Chemin tenté :" << jsonFile->fileName();
    }
}

void Settings::renderList()
{
    clearLayout(ui->verticalLayout);

    for (const QString &roomName : *roomList) {
        QHBoxLayout *hlayout = new QHBoxLayout();
        QLabel *label = new QLabel(roomName);
        QPushButton *button = new QPushButton("Supprimer");
        label->setStyleSheet("padding: 5px; border-bottom: 2px solid #4D4D4D;");

        hlayout->addWidget(label);
        hlayout->addWidget(button);

        connect(button, &QPushButton::clicked, this, [this, roomName]() {
            this->removeRoom(roomName);
        });

        ui->verticalLayout->addLayout(hlayout);
    }

    ui->verticalLayout->addStretch(1);
}
