#ifndef OVERLAYMAC_H
#define OVERLAYMAC_H

#include <QWidget>
#include <QPixmap>
#include <QKeyEvent>

class OverlayMac : public QWidget {
    Q_OBJECT

public:
    explicit OverlayMac(QWidget *parent = nullptr, QPixmap *image = nullptr);

protected:
    void keyPressEvent(QKeyEvent *event) override;

};

#endif // OVERLAYMAC_H

