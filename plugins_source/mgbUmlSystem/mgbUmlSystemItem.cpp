#include "mgbUmlSystemItem.h"
#include "../../src/util.h"
#include <QPainter>
#include <QFont>
#include <QFontMetrics>
#include <algorithm>

#define GLOBAL_SCALEF 50.0
const QFont MGB_LABEL_FONT("Helvetica", 10);

// Helper function to convert width/height properties to pixels
static int umlLengthToPixels(QString raw, int fallbackPx) {
    raw = raw.trimmed();
    if (raw.isEmpty()) return fallbackPx;

    bool ok = false;
    if (raw.endsWith("cm")) {
        raw.chop(2);
        double cm = raw.toDouble(&ok);
        if (ok) return static_cast<int>(cm * GLOBAL_SCALEF);
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

QRectF SystemNodeItem::labelRect() const {
    return QRectF(); // Suppress the default external label box
}

QPainterPath SystemNodeItem::shape() const {
    QPainterPath path;
    
    QString label = replaceTexConstants(_node->label());
    QFont titleFont = MGB_LABEL_FONT;
    titleFont.setBold(true);
    QFontMetrics fm(titleFont);
    QRect b = fm.boundingRect(label);

    QString widthProp = _node->data()->property("minimum width");
    if (widthProp.isEmpty() && _node->style() && _node->style()->data()) {
        widthProp = _node->style()->data()->property("minimum width");
    }

    QString heightProp = _node->data()->property("minimum height");
    if (heightProp.isEmpty() && _node->style() && _node->style()->data()) {
        heightProp = _node->style()->data()->property("minimum height");
    }

    int minW = umlLengthToPixels(widthProp, 240);
    int minH = umlLengthToPixels(heightProp, 160);

    int w = std::max(b.width() + 40, minW);
    int h = std::max(b.height() + 40, minH);

    qreal sw = (w / 2.0) / GLOBAL_SCALEF;
    qreal sh = (h / 2.0) / GLOBAL_SCALEF;

    QVector<QPointF> points ({
        QPointF(-sw, -sh),
        QPointF(-sw,  sh),
        QPointF( sw,  sh),
        QPointF( sw, -sh)
    });

    double rotate = _node->data()->property("rotate").toDouble();
    QTransform transform;
    transform.scale(GLOBAL_SCALEF, GLOBAL_SCALEF).rotate(rotate);

    path.addPolygon(transform.map(QPolygonF(points)));
    path.closeSubpath();
    
    return path;
}

void SystemNodeItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *) {
    QPen pen(_node->style()->strokeColor());
    pen.setWidth(_node->style()->strokeThickness());
    painter->setPen(pen);
    painter->setBrush(QBrush(_node->style()->fillColor()));

    painter->drawPath(shape());

    painter->setPen(QPen(Qt::black));
    QFont titleFont = MGB_LABEL_FONT;
    titleFont.setBold(true);
    painter->setFont(titleFont);

    QRectF r = shape().boundingRect();
    QRectF titleRect(r.left() + 10, r.top() + 6, r.width() - 20, 24);
    painter->drawText(titleRect, Qt::AlignLeft | Qt::AlignTop, replaceTexConstants(_node->label()));

    paintSelectionAndOuterLabels(painter);
}