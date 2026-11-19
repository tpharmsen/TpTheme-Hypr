#include "SvgClockItem.h"

#include <QPainter>

SvgClockItem::SvgClockItem(QQuickItem *parent)
    : QQuickPaintedItem(parent),
      m_renderer(QStringLiteral(":/TpTheme/Hypr/assets/clock.svgz")),
      m_time(QDateTime::currentDateTime())
{
    setAntialiasing(true);
    const QTime clock = m_time.time();
    m_secondAngle = 180.0 + clock.second() * 6.0;

    m_tickTimer.setInterval(25);
    m_tickTimer.setTimerType(Qt::CoarseTimer);
    connect(&m_tickTimer, &QTimer::timeout, this, [this]() {
        qreal dt = m_tickElapsed.nsecsElapsed() / 1000000000.0;
        m_tickElapsed.restart();
        dt = qBound<qreal>(0.016, dt, 0.050);

        const qreal displacement = m_tickTargetAngle - m_secondAngle;
        m_tickVelocity += (420.0 * displacement - 28.0 * m_tickVelocity) * dt;
        m_secondAngle += m_tickVelocity * dt;

        if (qAbs(displacement) < 0.05 && qAbs(m_tickVelocity) < 0.25) {
            m_secondAngle = m_tickTargetAngle;
            m_tickVelocity = 0.0;
            m_tickTimer.stop();
        }
        update();
    });
}

void SvgClockItem::setTime(const QDateTime &time)
{
    const QTime previousClock = m_time.time();
    const QTime nextClock = time.time();
    if (previousClock.hour() == nextClock.hour()
        && previousClock.minute() == nextClock.minute()
        && previousClock.second() == nextClock.second()) {
        return;
    }

    m_time = time;
    const QTime clock = m_time.time();
    qreal target = 180.0 + clock.second() * 6.0;
    while (target - m_secondAngle < -180.0)
        target += 360.0;
    while (target - m_secondAngle > 180.0)
        target -= 360.0;

    m_tickTargetAngle = target;
    m_tickVelocity = 0.0;
    m_tickElapsed.restart();
    m_tickTimer.start();

    update();
    emit timeChanged();
}

SvgClockItem::HandAsset SvgClockItem::renderHand(const QString &id, qreal scale, qreal pivotY)
{
    HandAsset asset;
    const QRectF bounds = m_renderer.boundsOnElement(id);
    if (bounds.isEmpty() || scale <= 0)
        return {};

    const QSize size(qMax(1, qRound(bounds.width() * scale)),
                     qMax(1, qRound(bounds.height() * scale)));
    asset.pixmap = QPixmap(size);
    asset.pixmap.fill(Qt::transparent);
    QPainter painter(&asset.pixmap);
    painter.setRenderHint(QPainter::Antialiasing);
    m_renderer.render(&painter, id, QRectF(QPointF(0, 0), QSizeF(size)));
    asset.pivot = QPointF(size.width() / 2.0, pivotY * scale);
    asset.valid = true;
    return asset;
}

void SvgClockItem::paint(QPainter *painter)
{
    const int side = qMin(width(), height());
    if (side <= 0 || !m_renderer.isValid())
        return;

    const QRectF target((width() - side) / 2.0, (height() - side) / 2.0, side, side);
    const QRectF faceBounds = m_renderer.boundsOnElement(QStringLiteral("ClockFace"));
    if (faceBounds.isEmpty())
        return;

    const qreal scale = side / faceBounds.width();
    painter->setRenderHint(QPainter::SmoothPixmapTransform);

    if (!qFuzzyCompare(m_cachedScale, scale)) {
        m_cachedScale = scale;
        m_cachedFace = QPixmap(qMax(1, qRound(target.width())),
                               qMax(1, qRound(target.height())));
        m_cachedFace.fill(Qt::transparent);
        QPainter facePainter(&m_cachedFace);
        facePainter.setRenderHint(QPainter::Antialiasing);
        m_renderer.render(&facePainter, QStringLiteral("ClockFace"),
                          QRectF(QPointF(0, 0), QSizeF(m_cachedFace.size())));
        m_cachedSecondHand = renderHand(QStringLiteral("SecondHand"), scale, 4.0);
        m_cachedMinuteHand = renderHand(QStringLiteral("MinuteHand"), scale, 6.0);
        m_cachedHourHand = renderHand(QStringLiteral("HourHand"), scale, 9.0);
    }

    painter->drawPixmap(target.topLeft(), m_cachedFace);

    const QTime clock = m_time.time();
    const qreal hourAngle = 180.0 + (clock.hour() % 12) * 30.0
                            + clock.minute() * 0.5
                            + clock.second() / 120.0;
    const qreal minuteAngle = 180.0 + clock.minute() * 6.0
                              + clock.second() * 0.1;
    const QPointF center = target.center();

    auto drawHand = [&](const HandAsset &hand, qreal angle) {
        if (!hand.valid)
            return;
        painter->save();
        painter->translate(center);
        painter->rotate(angle);
        painter->drawPixmap(-hand.pivot, hand.pixmap);
        painter->restore();
    };

    drawHand(m_cachedHourHand, hourAngle);
    drawHand(m_cachedMinuteHand, minuteAngle);
    drawHand(m_cachedSecondHand, m_secondAngle);
}
