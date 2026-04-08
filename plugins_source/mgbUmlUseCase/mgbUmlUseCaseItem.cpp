#include "mgbUmlUseCaseItem.h"
#include "../../src/util.h"
#include <QPainter>
#include <QFont>
#include <QFontMetrics>

#define GLOBAL_SCALEF 50.0
const QFont MGB_LABEL_FONT("Helvetica", 10);

QRectF UseCaseNodeItem::labelRect() const { return QRectF(); }

QPainterPath UseCaseNodeItem::shape() const {
    QPainterPath path;
    double rotate = _node->data()->property("rotate").toDouble();
    QTransform transform;
    transform.scale(GLOBAL_SCALEF, GLOBAL_SCALEF).rotate(rotate);

    QString label = replaceTexConstants(_node->label());
    QFontMetrics fm(MGB_LABEL_FONT);
    QRect b = fm.boundingRect(label);
    
    qreal sw = std::max((b.width() / 2.0 + 15) / GLOBAL_SCALEF, 0.5);
    qreal sh = std::max((b.height() / 2.0 + 10) / GLOBAL_SCALEF, 0.25);
    
    path.addEllipse(QPointF(0, 0), sw * GLOBAL_SCALEF, sh * GLOBAL_SCALEF);
    return path;
}

void UseCaseNodeItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *) {
    QPen pen(_node->style()->strokeColor());
    pen.setWidth(_node->style()->strokeThickness());
    painter->setPen(pen);
    painter->setBrush(QBrush(_node->style()->fillColor()));
    painter->drawPath(shape());

    painter->setPen(QPen(Qt::black));
    painter->setFont(MGB_LABEL_FONT);
    painter->drawText(shape().boundingRect(), Qt::AlignCenter, replaceTexConstants(_node->label()));

    paintSelectionAndOuterLabels(painter);
}