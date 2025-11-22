# photo-booth
Fait apparaitre des images sur l'écran des personnes avec l'application installé


- [x] Pour mettre l'app en background : QSystemTrayIcon Class
- [x] Dans `.pro` ajouter la ligne `RC_ICONS = myappico.ico` Pour icone & `QGuiApplication::instance()->setWindowIcon(QIcon("path/to/icon.png"));` [source](https://forum.qt.io/topic/130110/setting-default-application-icon-taskbar-exe-etc/2) (icone dans .qrc je suppose)
- [ ] Ajouter un ping pour le websocket



- deploy
```bash
C:\Qt\6.10.0\mingw_64\bin\windeployqt.exe --release E:\Code\photo-booth\qt-app\build\Desktop_Qt_6_10_0_MinGW_64_bit-Release\release\photo-booth.exe --dir E:\Code\photo-booth\deploy
```
