#include "overlay.h"
#include <QKeyEvent>
#include <iostream>
#include <QScreen>
#include <QLabel>

Overlay::Overlay(QPixmap *image, QWidget *parent)
    : QWidget(parent) {
    setWindowFlags(Qt::Tool | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    setAttribute(Qt::WA_TranslucentBackground);
    setAttribute(Qt::WA_ShowWithoutActivating);
    setAttribute(Qt::WA_TransparentForMouseEvents);

    QLabel *label = new QLabel(this);

    QSize targetSize = QGuiApplication::primaryScreen()->size();
    this->resize(targetSize);

    if (image != nullptr) {
        if (image->size() != targetSize) {
            label->setPixmap(image->scaled(targetSize, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        } else {
            label->setPixmap(*image);
        }
    }

    label->setGeometry(0, 0, targetSize.width(), targetSize.height());
    label->setAlignment(Qt::AlignCenter);

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
