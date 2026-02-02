#ifndef OVERLAY_H
#define OVERLAY_H

#include <QWidget>
#include <QPixmap>
#include <QKeyEvent>

class Overlay : public QWidget {
    Q_OBJECT

public:
    explicit Overlay(QPixmap *image, QWidget *parent = nullptr);

protected:
    void keyPressEvent(QKeyEvent *event) override;

};

#endif // OVERLAY_H
