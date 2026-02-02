#ifndef HOSTS_H
#define HOSTS_H
#include <QString>

const QString HOST = "photo.nakyss.fr";
// const QString HOST = "localhost:59875";

const QString WS_URL = "wss://" + HOST;

const QString BASE_URL = "https://" + HOST;

const QString USER_URL = BASE_URL + "/users";


#endif // HOSTS_H
