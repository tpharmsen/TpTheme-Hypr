#pragma once

#include <QQuickPaintedItem>
#include <QSvgRenderer>
#include <QPixmap>

class SvgClockItem : public QQuickPaintedItem {
    Q_OBJECT
    Q_PROPERTY(QDateTime time READ time WRITE setTime NOTIFY timeChanged)
public:
    explicit SvgClockItem(QQuickItem *parent = nullptr);
    void paint(QPainter *painter) override;
    QDateTime time() const { return m_time; }
    void setTime(const QDateTime &time);

signals:
    void timeChanged();

private:
    struct HandAsset {
        QPixmap pixmap;
        QPointF pivot;
        bool valid = false;
    };

    HandAsset renderHand(const QString &id, qreal scale, qreal pivotY);
    QSvgRenderer m_renderer;
    QDateTime m_time;
};
