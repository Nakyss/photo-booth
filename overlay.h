#ifndef OVERLAY_H
#define OVERLAY_H

#include <QWidget>
#include <QPixmap>
#include <QKeyEvent>

class Overlay : public QWidget {
    Q_OBJECT

public:
    explicit Overlay(QWidget *parent = nullptr, QPixmap *image = nullptr);

protected:
    void keyPressEvent(QKeyEvent *event) override;

};

#endif // OVERLAY_H
