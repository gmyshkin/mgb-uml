#include "mgbUmlNodeItems.h"
#include "tikzit.h"
#include "util.h"
#include <QPainter>

// ---------------------------------------------------------
// USE CASE IMPLEMENTATION
// ---------------------------------------------------------
QRectF UseCaseNodeItem::labelRect() const {
    return QRectF(); // No external yellow box
}

QPainterPath UseCaseNodeItem::shape() const {
    QPainterPath path;
    double rotate = _node->data()->property("rotate").toDouble();
    QTransform transform;
    transform.scale(GLOBAL_SCALEF, GLOBAL_SCALEF).rotate(rotate);

    QString label = replaceTexConstants(_node->label());
    QFontMetrics fm(Tikzit::LABEL_FONT);
    QRect b = fm.boundingRect(label);
    
    qreal sw = std::max((b.width() / 2.0 + 15) / GLOBAL_SCALEF, 0.5);
    qreal sh = std::max((b.height() / 2.0 + 10) / GLOBAL_SCALEF, 0.25);
    
    path.addEllipse(QPointF(0, 0), sw * GLOBAL_SCALEF, sh * GLOBAL_SCALEF);
    return path;
}

void UseCaseNodeItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *) {
    // Draw the oval
    QPen pen(_node->style()->strokeColor());
    pen.setWidth(_node->style()->strokeThickness());
    painter->setPen(pen);
    painter->setBrush(QBrush(_node->style()->fillColor()));
    painter->drawPath(shape());

    // Draw internal text
    painter->setPen(QPen(Qt::black));
    painter->setFont(Tikzit::LABEL_FONT);
    painter->drawText(shape().boundingRect(), Qt::AlignCenter, replaceTexConstants(_node->label()));

    // Apply selection outlines
    paintSelectionAndOuterLabels(painter);
}

// ---------------------------------------------------------
// CLASS IMPLEMENTATION
// ---------------------------------------------------------
QRectF ClassNodeItem::labelRect() const {
    return QRectF(); // No external yellow box
}

QPainterPath ClassNodeItem::shape() const {
    QPainterPath path;
    double rotate = _node->data()->property("rotate").toDouble();
    QTransform transform;
    transform.scale(GLOBAL_SCALEF, GLOBAL_SCALEF).rotate(rotate);

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

    QFontMetrics fm(Tikzit::LABEL_FONT);
    QFont boldFont = Tikzit::LABEL_FONT; boldFont.setBold(true);
    QFontMetrics fmBold(boldFont);
    
    int padding = 10;
    QRect b1 = fmBold.boundingRect(QRect(0,0,1000,1000), Qt::AlignCenter, printPart1);
    QRect b2 = fm.boundingRect(QRect(0,0,1000,1000), Qt::AlignLeft, printPart2);
    QRect b3 = fm.boundingRect(QRect(0,0,1000,1000), Qt::AlignLeft, printPart3);
    
    int w = std::max({b1.width(), b2.width(), b3.width(), 60}) + padding * 2;
    int h = std::max(b1.height() + b2.height() + b3.height() + padding * 3, 40);
    
    qreal sw = (w / 2.0) / GLOBAL_SCALEF;
    qreal sh = (h / 2.0) / GLOBAL_SCALEF;
    
    QVector<QPointF> points ({ QPointF(-sw, -sh), QPointF(-sw,  sh), QPointF( sw,  sh), QPointF( sw, -sh) });
    path.addPolygon(transform.map(QPolygonF(points)));
    path.closeSubpath();
    return path;
}

void ClassNodeItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *) {
    // Draw outer box
    QPen pen(_node->style()->strokeColor());
    pen.setWidth(_node->style()->strokeThickness());
    painter->setPen(pen);
    painter->setBrush(QBrush(_node->style()->fillColor()));
    painter->drawPath(shape());

    QRectF r = shape().boundingRect();
    
    // String parsing
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

    QFont titleFont = Tikzit::LABEL_FONT; titleFont.setBold(true);
    QFontMetrics fmTitle(titleFont);
    QFontMetrics fm(Tikzit::LABEL_FONT);
    
    int padding = 10;
    QRect b1 = fmTitle.boundingRect(QRect(0,0,1000,1000), Qt::AlignCenter, printPart1);
    QRect b2 = fm.boundingRect(QRect(0,0,1000,1000), Qt::AlignLeft, printPart2);
    
    qreal y1 = r.top() + b1.height() + padding;
    qreal y2 = y1 + b2.height() + padding;

    // Inner lines
    painter->setPen(QPen(Qt::black));
    painter->drawLine(QPointF(r.left(), y1), QPointF(r.right(), y1));
    painter->drawLine(QPointF(r.left(), y2), QPointF(r.right(), y2));

    // Text
    painter->setFont(titleFont);
    painter->drawText(QRectF(r.left(), r.top() + (padding/2), r.width(), b1.height()), Qt::AlignCenter, printPart1);
    
    painter->setFont(Tikzit::LABEL_FONT);
    painter->drawText(QRectF(r.left() + 5, y1 + (padding/2), r.width() - 10, b2.height()), Qt::AlignLeft | Qt::AlignTop, printPart2);
    painter->drawText(QRectF(r.left() + 5, y2 + (padding/2), r.width() - 10, r.bottom() - y2), Qt::AlignLeft | Qt::AlignTop, printPart3);

    paintSelectionAndOuterLabels(painter);
}