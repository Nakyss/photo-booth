#ifndef HOSTS_H
#define HOSTS_H
#include <QString>


#ifdef QT_DEBUG
  const QString HOST = "localhost:59875";
  const QString WS_URL = "ws://" + HOST;
  const QString BASE_URL = "http://" + HOST;
#else
  const QString HOST = "photo.nakyss.fr";
  const QString WS_URL = "wss://" + HOST;
  const QString BASE_URL = "https://" + HOST;
#endif

const QString USER_URL = BASE_URL + "/users";


#endif // HOSTS_H
