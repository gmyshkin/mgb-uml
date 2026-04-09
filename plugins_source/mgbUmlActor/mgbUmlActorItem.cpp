#include "mgbUmlActorItem.h"
#include "../../src/util.h"
#include <QPainter>
#include <QFont>
#include <QFontMetrics>

#define GLOBAL_SCALEF 50.0
const QFont MGB_LABEL_FONT("Helvetica", 10);

QRectF ActorNodeItem::labelRect() const {
    QString label = replaceTexConstants(_node->label());
    QFontMetrics fm(MGB_LABEL_FONT);
    QRectF rect = fm.boundingRect(label);
    rect.moveCenter(QPointF(0, GLOBAL_SCALEF * 0.95));
    return rect.adjusted(-4, -2, 4, 2);
}

QPainterPath ActorNodeItem::shape() const {
    QPainterPath path;
    QPainterPath actorPath;
    
    // Draw the stick figure
    actorPath.addEllipse(QPointF(0, -0.45), 0.12, 0.12); // head
    actorPath.moveTo(0, -0.33);
    actorPath.lineTo(0, 0.20);                           // body
    actorPath.moveTo(-0.22, -0.05);
    actorPath.lineTo(0.22, -0.05);                       // arms
    actorPath.moveTo(0, 0.20);
    actorPath.lineTo(-0.20, 0.55);                       // left leg
    actorPath.moveTo(0, 0.20);
    actorPath.lineTo(0.20, 0.55);                        // right leg

    double rotate = _node->data()->property("rotate").toDouble();
    QTransform transform;
    transform.scale(GLOBAL_SCALEF, GLOBAL_SCALEF).rotate(rotate);
    
    path = transform.map(actorPath);
    return path;
}

void ActorNodeItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *) {
    QPen pen(_node->style()->strokeColor());
    pen.setWidth(_node->style()->strokeThickness());
    painter->setPen(pen);
    
    // Actors have no internal fill
    painter->setBrush(Qt::NoBrush);
    painter->drawPath(shape());

    painter->setPen(QPen(Qt::black));
    painter->setFont(MGB_LABEL_FONT);
    painter->drawText(labelRect(), Qt::AlignCenter, replaceTexConstants(_node->label()));

    paintSelectionAndOuterLabels(painter);
}