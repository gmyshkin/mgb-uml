#include "mgbUmlActorItem.h"
#include "../../src/util.h"
#include <QPainter>
#include <QPainterPathStroker>
#include <QFont>
#include <QFontMetrics>

static constexpr qreal UML_SCALEF = 40.0;
const QFont MGB_LABEL_FONT("Helvetica", 10);

static QPainterPath actorVisualPath(Node *node)
{
    QPainterPath actorPath;

    actorPath.addEllipse(QPointF(0, -0.45), 0.12, 0.12);
    actorPath.moveTo(0, -0.33);
    actorPath.lineTo(0, 0.20);
    actorPath.moveTo(-0.22, -0.05);
    actorPath.lineTo(0.22, -0.05);
    actorPath.moveTo(0, 0.20);
    actorPath.lineTo(-0.20, 0.55);
    actorPath.moveTo(0, 0.20);
    actorPath.lineTo(0.20, 0.55);

    double rotate = node->data()->property("rotate").toDouble();
    QTransform transform;
    transform.scale(UML_SCALEF, UML_SCALEF).rotate(rotate);
    return transform.map(actorPath);
}

QRectF ActorNodeItem::labelRect() const {
    QString label = replaceTexConstants(_node->label());
    QFontMetrics fm(MGB_LABEL_FONT);
    QRectF rect = fm.boundingRect(label);
    rect.moveCenter(QPointF(0, UML_SCALEF * 0.95));
    return rect.adjusted(-4, -2, 4, 2);
}

QPainterPath ActorNodeItem::shape() const {
    QPainterPath path;
    QPainterPath actorPath = actorVisualPath(_node);

    QPainterPathStroker stroker;
    stroker.setWidth(12);
    path = stroker.createStroke(actorPath);
    path.addPath(actorPath);
    path.addRect(labelRect());
    return path;
}

void ActorNodeItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *) {
    QPen pen(_node->style()->strokeColor());
    pen.setWidth(_node->style()->strokeThickness());
    painter->setPen(pen);
    
    // Actors have no internal fill
    painter->setBrush(Qt::NoBrush);
    painter->drawPath(actorVisualPath(_node));

    painter->setPen(QPen(Qt::black));
    painter->setFont(MGB_LABEL_FONT);
    painter->drawText(labelRect(), Qt::AlignCenter, replaceTexConstants(_node->label()));

    paintSelectionAndOuterLabels(painter);
}
