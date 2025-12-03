#include "overlay_mac.h"

#include <QLabel>
#include <QScreen>
#include <QGuiApplication>
#include <QVariant>

#ifdef Q_OS_MAC
#import <Cocoa/Cocoa.h>
#endif

OverlayMac::OverlayMac(QWidget *parent, QPixmap *image)
    : QWidget(parent)
{
    setWindowFlags(Qt::FramelessWindowHint
                   | Qt::Tool            // évite App Exposé
                   | Qt::WindowDoesNotAcceptFocus
                   | Qt::WindowStaysOnTopHint);

    setAttribute(Qt::WA_TranslucentBackground);
    setAttribute(Qt::WA_ShowWithoutActivating);
    setAttribute(Qt::WA_TransparentForMouseEvents); // opc

    // Prend toute la taille d’écran
    QScreen *screen = QGuiApplication::primaryScreen();
    resize(screen->geometry().size());

    // Contenu
    QLabel *label = new QLabel(this);
    label->setAlignment(Qt::AlignCenter);
    label->resize(size());

    if (image != nullptr)
        label->setPixmap(image->scaled(size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));

#ifdef Q_OS_MAC
    //
    // ⚠️ Qt 6 : impossible d'utiliser winId() → renvoie QNSView
    // On doit récupérer NSWindow via la propriété privée "_q_nswindow"
    //
    QWidget *w = this->window();
    NSWindow *nsWindow = nullptr;

    QVariant handle = w->property("_q_nswindow");
    if (handle.isValid()) {
        nsWindow = (__bridge NSWindow *)handle.value<void *>();
    }

    if (nsWindow) {
        // Toujours au-dessus du fullscreen, dock, Mission Control
        [nsWindow setLevel: NSScreenSaverWindowLevel];

        // S'affiche dans tous les bureaux + aux écrans fullscreen
        [nsWindow setCollectionBehavior:
            NSWindowCollectionBehaviorCanJoinAllSpaces |
            NSWindowCollectionBehaviorFullScreenAuxiliary |
            NSWindowCollectionBehaviorTransient];

        // Facultatif : empêcher Exposé / Cmd+Tab de cacher la fenêtre
        [nsWindow setIgnoresMouseEvents:YES];
    } else {
        qWarning("❌ OverlayMac : Impossible de récupérer NSWindow via _q_nswindow !");
    }
#endif

    show();
    raise();
}

void OverlayMac::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Escape)
        hide();
    else
        QWidget::keyPressEvent(event);
}
