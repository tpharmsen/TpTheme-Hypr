#pragma once

#include <QQuickPaintedItem>
#include <QSvgRenderer>
#include <QPixmap>
#include <QTimer>
#include <QElapsedTimer>

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
    qreal m_secondAngle = 180.0;
    qreal m_cachedScale = 0.0;
    QPixmap m_cachedFace;
    HandAsset m_cachedSecondHand;
    HandAsset m_cachedMinuteHand;
    HandAsset m_cachedHourHand;
    QTimer m_tickTimer;
    qreal m_tickTargetAngle = 180.0;
    qreal m_tickVelocity = 0.0;
    QElapsedTimer m_tickElapsed;
};
