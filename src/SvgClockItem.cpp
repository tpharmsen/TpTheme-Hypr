#include "SvgClockItem.h"

#include <QPainter>

SvgClockItem::SvgClockItem(QQuickItem *parent)
    : QQuickPaintedItem(parent),
      m_renderer(QStringLiteral(":/TpTheme/Hypr/assets/clock.svgz")),
      m_time(QDateTime::currentDateTime())
{
    setAntialiasing(true);
}

void SvgClockItem::setTime(const QDateTime &time)
{
    if (m_time == time)
        return;
    m_time = time;
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
    painter->save();
    painter->translate(target.topLeft());
    m_renderer.render(painter, QStringLiteral("ClockFace"), QRectF(QPointF(0, 0), target.size()));
    painter->restore();

    const QTime clock = m_time.time();
    const qreal hourAngle = 180.0 + (clock.hour() % 12 * 30.0) + clock.minute() * 0.5;
    const qreal minuteAngle = 180.0 + clock.minute() * 6.0 + clock.second() * 0.1;
    const qreal secondAngle = 180.0 + clock.second() * 6.0;
    const QPointF center = target.center();

    auto drawHand = [&](const QString &id, qreal angle, qreal pivotY) {
        const HandAsset hand = renderHand(id, scale, pivotY);
        if (!hand.valid)
            return;
        painter->save();
        painter->translate(center);
        painter->rotate(angle);
        painter->drawPixmap(-hand.pivot, hand.pixmap);
        painter->restore();
    };

    drawHand(QStringLiteral("HourHand"), hourAngle, 9.0);
    drawHand(QStringLiteral("MinuteHand"), minuteAngle, 6.0);
    drawHand(QStringLiteral("SecondHand"), secondAngle, 4.0);
}
