#include "mgbUmlUseCaseItem.h"
#include "../../src/util.h"
#include <QPainter>
#include <QFont>
#include <QFontMetrics>
#include <algorithm>

namespace {

static constexpr qreal UML_SCALEF = 40.0;
const QFont MGB_LABEL_FONT("Helvetica", 10);

static int umlLengthToPixels(QString raw, int fallbackPx)
{
    raw = raw.trimmed();
    if (raw.isEmpty()) return fallbackPx;

    bool ok = false;
    if (raw.endsWith("cm")) {
        raw.chop(2);
        double cm = raw.toDouble(&ok);
        if (ok) return static_cast<int>(cm * UML_SCALEF);
    }

    if (raw.endsWith("pt")) {
        raw.chop(2);
        double pt = raw.toDouble(&ok);
        if (ok) return static_cast<int>(pt * 1.3333);
    }

    double plain = raw.toDouble(&ok);
    if (ok) return static_cast<int>(plain);
    return fallbackPx;
}

}

QRectF UseCaseNodeItem::labelRect() const { return QRectF(); }

QPainterPath UseCaseNodeItem::shape() const {
    QPainterPath path;
    double rotate = _node->data()->property("rotate").toDouble();
    QTransform transform;
    transform.scale(UML_SCALEF, UML_SCALEF).rotate(rotate);

    QString label = replaceTexConstants(_node->label());
    QFontMetrics fm(MGB_LABEL_FONT);
    QRect b = fm.boundingRect(label);

    QString widthProp = _node->data()->property("minimum width");
    if (widthProp.isEmpty() && _node->style() && _node->style()->data()) {
        widthProp = _node->style()->data()->property("minimum width");
    }

    QString heightProp = _node->data()->property("minimum height");
    if (heightProp.isEmpty() && _node->style() && _node->style()->data()) {
        heightProp = _node->style()->data()->property("minimum height");
    }

    int minW = umlLengthToPixels(widthProp, 120);
    int minH = umlLengthToPixels(heightProp, 60);

    qreal sw = std::max(std::max(minW / 2.0, b.width() / 2.0 + 15.0) / UML_SCALEF, 0.5);
    qreal sh = std::max(std::max(minH / 2.0, b.height() / 2.0 + 10.0) / UML_SCALEF, 0.25);

    path.addEllipse(QPointF(0, 0), sw * UML_SCALEF, sh * UML_SCALEF);
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
