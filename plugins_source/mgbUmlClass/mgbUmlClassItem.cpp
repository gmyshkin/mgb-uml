#include "mgbUmlClassItem.h"
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

QRectF ClassNodeItem::labelRect() const {
    return QRectF(); // No external yellow box
}

QPainterPath ClassNodeItem::shape() const {
    QPainterPath path;
    double rotate = _node->data()->property("rotate").toDouble();
    QTransform transform;
    transform.scale(UML_SCALEF, UML_SCALEF).rotate(rotate);

    QString label = _node->label();
    QString part1 = label, part2 = "", part3 = "";
    int idx2 = label.indexOf("\\nodepart{two}");
    int idx3 = label.indexOf("\\nodepart{three}");

    if (idx2 != -1) {
        part1 = label.left(idx2).trimmed();
        if (idx3 != -1) {
            part2 = label.mid(idx2 + 14, idx3 - (idx2 + 14)).trimmed();
            part3 = label.mid(idx3 + 16).trimmed();
        } else {
            part2 = label.mid(idx2 + 14).trimmed();
        }
    }
    
    QString printPart1 = replaceTexConstants(part1).replace("\\\\", "\n").replace(" \\ ", "\n");
    QString printPart2 = replaceTexConstants(part2).replace("\\\\", "\n").replace(" \\ ", "\n");
    QString printPart3 = replaceTexConstants(part3).replace("\\\\", "\n").replace(" \\ ", "\n");

    QFontMetrics fm(MGB_LABEL_FONT);
    QFont boldFont = MGB_LABEL_FONT; boldFont.setBold(true);
    QFontMetrics fmBold(boldFont);
    
    int padding = 10;
    QRect b1 = fmBold.boundingRect(QRect(0,0,1000,1000), Qt::AlignCenter, printPart1);
    QRect b2 = fm.boundingRect(QRect(0,0,1000,1000), Qt::AlignLeft, printPart2);
    QRect b3 = fm.boundingRect(QRect(0,0,1000,1000), Qt::AlignLeft, printPart3);
    
    QString widthProp = _node->data()->property("minimum width");
    if (widthProp.isEmpty() && _node->style() && _node->style()->data()) {
        widthProp = _node->style()->data()->property("minimum width");
    }

    QString heightProp = _node->data()->property("minimum height");
    if (heightProp.isEmpty() && _node->style() && _node->style()->data()) {
        heightProp = _node->style()->data()->property("minimum height");
    }

    int minW = umlLengthToPixels(widthProp, 120);
    int minH = umlLengthToPixels(heightProp, 80);

    int w = std::max(minW, std::max({b1.width(), b2.width(), b3.width(), 60}) + padding * 2);
    int h = std::max(minH, std::max(b1.height() + b2.height() + b3.height() + padding * 3, 40));
    
    qreal sw = (w / 2.0) / UML_SCALEF;
    qreal sh = (h / 2.0) / UML_SCALEF;
    
    QVector<QPointF> points ({ QPointF(-sw, -sh), QPointF(-sw,  sh), QPointF( sw,  sh), QPointF( sw, -sh) });
    path.addPolygon(transform.map(QPolygonF(points)));
    path.closeSubpath();
    return path;
}

void ClassNodeItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *) {
    QPen pen(_node->style()->strokeColor());
    pen.setWidth(_node->style()->strokeThickness());
    painter->setPen(pen);
    painter->setBrush(QBrush(_node->style()->fillColor()));
    painter->drawPath(shape());

    QRectF r = shape().boundingRect();
    
    QString label = _node->label();
    QString part1 = label, part2 = "", part3 = "";
    int idx2 = label.indexOf("\\nodepart{two}");
    int idx3 = label.indexOf("\\nodepart{three}");
    if (idx2 != -1) {
        part1 = label.left(idx2).trimmed();
        if (idx3 != -1) {
            part2 = label.mid(idx2 + 14, idx3 - (idx2 + 14)).trimmed();
            part3 = label.mid(idx3 + 16).trimmed();
        } else {
            part2 = label.mid(idx2 + 14).trimmed();
        }
    }

    QString printPart1 = replaceTexConstants(part1).replace("\\\\", "\n").replace(" \\ ", "\n");
    QString printPart2 = replaceTexConstants(part2).replace("\\\\", "\n").replace(" \\ ", "\n");
    QString printPart3 = replaceTexConstants(part3).replace("\\\\", "\n").replace(" \\ ", "\n");

    QFont titleFont = MGB_LABEL_FONT; titleFont.setBold(true);
    QFontMetrics fmTitle(titleFont);
    QFontMetrics fm(MGB_LABEL_FONT);
    
    int padding = 10;
    QRect b1 = fmTitle.boundingRect(QRect(0,0,1000,1000), Qt::AlignCenter, printPart1);
    QRect b2 = fm.boundingRect(QRect(0,0,1000,1000), Qt::AlignLeft, printPart2);
    
    qreal y1 = r.top() + b1.height() + padding;
    qreal y2 = y1 + b2.height() + padding;

    painter->setPen(QPen(Qt::black));
    painter->drawLine(QPointF(r.left(), y1), QPointF(r.right(), y1));
    painter->drawLine(QPointF(r.left(), y2), QPointF(r.right(), y2));

    painter->setFont(titleFont);
    painter->drawText(QRectF(r.left(), r.top() + (padding/2), r.width(), b1.height()), Qt::AlignCenter, printPart1);
    
    painter->setFont(MGB_LABEL_FONT);
    painter->drawText(QRectF(r.left() + 5, y1 + (padding/2), r.width() - 10, b2.height()), Qt::AlignLeft | Qt::AlignTop, printPart2);
    painter->drawText(QRectF(r.left() + 5, y2 + (padding/2), r.width() - 10, r.bottom() - y2), Qt::AlignLeft | Qt::AlignTop, printPart3);

    paintSelectionAndOuterLabels(painter);
}
