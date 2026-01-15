#ifndef SETTINGS_H
#define SETTINGS_H

#include <QWidget>
#include <QLabel>
#include "websocketclient.h"
#include <QFile>

namespace Ui {
class Settings;
}

class Settings : public QWidget
{
    Q_OBJECT

public:
    explicit Settings(WebSocketClient *itsWs,QWidget *parent = nullptr);
    ~Settings();

private slots:
    void addRoom();
    void removeRoom(QString room);

private:
    Ui::Settings *ui;
    QVector<QString> *roomList;
    WebSocketClient *ws;
    QString getInput();
    void renderList();
    void clearLayout(QLayout* layout);
    QFile *jsonFile;
    void saveRooms();

signals:
    void roomChanged(const QVector<QString> *rooms);

};

#endif // SETTINGS_H
