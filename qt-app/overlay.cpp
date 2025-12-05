#include "overlay.h"
#include <QKeyEvent>
#include <iostream>
#include <QScreen>
#include <QLabel>

Overlay::Overlay(QWidget *parent, QPixmap *image)
    : QWidget(parent) {
    setWindowFlags(Qt::Tool | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    setAttribute(Qt::WA_TranslucentBackground);
    setAttribute(Qt::WA_ShowWithoutActivating);
    setAttribute(Qt::WA_TransparentForMouseEvents);

    QLabel *label = new QLabel(this);

    QScreen *screen = QGuiApplication::primaryScreen();
    this->resize(screen->size().width(), screen->size().height());

    if (image != nullptr){
        label->setPixmap(image->scaled(this->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    }
    label->setAlignment(Qt::AlignCenter);
    label->resize(this->size());

    this->show();

    this->raise();
    this->activateWindow(); // Assure que l'overlay capte les touches
}


// Obligé de gerer les touches dans l'overlay, car il prend le dessus sur tout
void Overlay::keyPressEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_Escape) {
        this->hide();
        std::cout << "Overlay fermé avec la touche Échap" << std::endl;
    } else {
        QWidget::keyPressEvent(event);
    }
}
