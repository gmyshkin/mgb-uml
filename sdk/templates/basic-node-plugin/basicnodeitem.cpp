#include "basicnodeitem.h"

#include "src/data/node.h"
#include "src/data/style.h"

#include <QFont>
#include <QFontMetrics>
#include <QPainter>

namespace {

static constexpr qreal SCALE = 40.0;
const QFont LABEL_FONT("Helvetica", 10);

}

QRectF BasicNodeItem::labelRect() const
{
    QFontMetrics fm(LABEL_FONT);
    QRectF rect = fm.boundingRect(_node->label());
    rect.moveCenter(QPointF(0, 0));
    return rect.adjusted(-6, -4, 6, 4);
}

QPainterPath BasicNodeItem::shape() const
{
    QPainterPath path;
    path.addRoundedRect(QRectF(-1.5 * SCALE, -0.75 * SCALE, 3.0 * SCALE, 1.5 * SCALE), 4, 4);
    return path;
}

void BasicNodeItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    QPen pen(_node->style()->strokeColor());
    pen.setWidth(_node->style()->strokeThickness());
    painter->setPen(pen);
    painter->setBrush(_node->style()->fillColor());
    painter->drawPath(shape());

    painter->setFont(LABEL_FONT);
    painter->setPen(Qt::black);
    painter->drawText(shape().boundingRect(), Qt::AlignCenter, _node->label());

    paintSelectionAndOuterLabels(painter);
}
