/*
    TikZiT - a GUI diagram editor for TikZ
    Copyright (C) 2018 Aleks Kissinger

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include "tikzit.h"
#include "nodeitem.h"
#include "tikzscene.h"
#include "util.h"
#include <cmath>
#include <algorithm>

#include <QPen>
#include <QApplication>
#include <QBrush>
#include <QDebug>
#include <QFont>
#include <QFontMetrics>
#include <QPainterPathStroker>

NodeItem::NodeItem(Node *node)
{
    _node = node;
    setFlag(QGraphicsItem::ItemIsSelectable);
    readPos();
    updateBounds();
}

void NodeItem::readPos()
{
    setPos(toScreen(_node->point()));
}

void NodeItem::writePos()
{
    _node->setPoint(fromScreen(pos()));
}

QRectF NodeItem::labelRect() const {
    // MGB-UML: UML shapes draw their text internally. Do not draw the external yellow box.
QRectF NodeItem::labelRect() const {
    QString styleName = _node->style()->name();

    if (styleName == "UML Use Case" ||
        styleName == "UML Class" ||
        styleName == "UML Actor" ||
        styleName == "UML System") {
        return QRectF();
    }

    QString label = replaceTexConstants(_node->label());
    QFontMetrics fm(Tikzit::LABEL_FONT);
    QRectF rect = fm.boundingRect(label);
    rect.moveCenter(QPointF(0,0));
    return rect;
}

QRectF NodeItem::outerLabelRect() const {
    QString label = replaceTexConstants(_node->data()->property("label"));
    label.replace(QRegularExpression("^[^:]*:"), "");
    QFontMetrics fm(Tikzit::LABEL_FONT);
    QRectF rect = fm.boundingRect(label);
    rect.moveCenter(QPointF(0, -0.5 * GLOBAL_SCALEF));
    return rect;
}

void NodeItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    QString styleName = _node->style()->name();

bool isUmlClass = (styleName == "UML Class");
bool isUseCase = (styleName == "UML Use Case");
bool isActor = (styleName == "UML Actor");
bool isSystem = (styleName == "UML System");

    if (_node->style()->isNone()) {
        QColor c(180,180,200);
        painter->setPen(QPen(c));
        painter->setBrush(QBrush(c));
        painter->drawEllipse(QPointF(0,0), 1,1);

        QPen pen(QColor(180,180,220));
        QVector<qreal> p;
        p << 1.0 << 2.0;
        pen.setDashPattern(p);
        pen.setWidthF(2.0);
        painter->setPen(pen);
        painter->setBrush(Qt::NoBrush);
        painter->drawPath(shape());
    } else {
        if (!isActor) {
    QPen pen(_node->style()->strokeColor());
    pen.setWidth(_node->style()->strokeThickness());
    painter->setPen(pen);
    painter->setBrush(QBrush(_node->style()->fillColor()));
    painter->drawPath(shape());
}

        // =================================================================
        // MGB-UML: CUSTOM RENDERING FOR UML ELEMENTS
        // =================================================================
        if (isUseCase) {
            // Draw Use Case text perfectly centered
            painter->setPen(QPen(Qt::black));
            painter->setFont(Tikzit::LABEL_FONT);
            QRectF r = shape().boundingRect();
            painter->drawText(r, Qt::AlignCenter, replaceTexConstants(_node->label()));
        }
        else if (isUmlClass) {
            QRectF r = shape().boundingRect();
            
            // Re-parse the text to figure out where to draw the horizontal lines
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

            // Calculate heights using font metrics so the lines match the text size
            QFont titleFont = Tikzit::LABEL_FONT; titleFont.setBold(true);
            QFontMetrics fmTitle(titleFont);
            QFontMetrics fm(Tikzit::LABEL_FONT);
            
            // Clean up LaTeX linebreaks (\\) into actual Qt newlines (\n) for measuring/drawing
            QString printPart1 = replaceTexConstants(part1).replace("\\\\", "\n").replace(" \\ ", "\n");
            QString printPart2 = replaceTexConstants(part2).replace("\\\\", "\n").replace(" \\ ", "\n");
            QString printPart3 = replaceTexConstants(part3).replace("\\\\", "\n").replace(" \\ ", "\n");

            int padding = 10;
            QRect b1 = fmTitle.boundingRect(QRect(0,0,1000,1000), Qt::AlignCenter, printPart1);
            QRect b2 = fm.boundingRect(QRect(0,0,1000,1000), Qt::AlignLeft, printPart2);
            
            // Calculate where the dividing lines go dynamically
            qreal y1 = r.top() + b1.height() + padding;
            qreal y2 = y1 + b2.height() + padding;

            // Draw the horizontal lines
            painter->drawLine(QPointF(r.left(), y1), QPointF(r.right(), y1));
            painter->drawLine(QPointF(r.left(), y2), QPointF(r.right(), y2));

            // Draw the text inside the boxes
            painter->setPen(QPen(Qt::black));

            // Compartment 1: Class Name (Bold, Centered)
            painter->setFont(titleFont);
            QRectF rect1(r.left(), r.top() + (padding/2), r.width(), b1.height());
            painter->drawText(rect1, Qt::AlignCenter, printPart1);

            // Compartment 2: Attributes (Left Aligned)
            painter->setFont(Tikzit::LABEL_FONT);
            QRectF rect2(r.left() + 5, y1 + (padding/2), r.width() - 10, b2.height());
            painter->drawText(rect2, Qt::AlignLeft | Qt::AlignTop, printPart2);

            // Compartment 3: Methods (Left Aligned)
            QRectF rect3(r.left() + 5, y2 + (padding/2), r.width() - 10, r.bottom() - y2);
            painter->drawText(rect3, Qt::AlignLeft | Qt::AlignTop, printPart3);
        }
    }

    // Only draw the standard yellow label box if it is NOT a UML shape
    bool drawLabel = (_node->label() != "") && !isUmlClass && !isUseCase;
    if (scene()) {
        TikzScene *sc = static_cast<TikzScene*>(scene());
        drawLabel = drawLabel && sc->drawNodeLabels();
    }

    if (drawLabel) {
        QRectF rect = labelRect();
        QPen pen(QColor(200,0,0,120));
        QVector<qreal> d;
        d << 2.0 << 2.0;
        pen.setDashPattern(d);
        painter->setPen(pen);
        painter->setBrush(QBrush(QColor(255,255,100,120)));
        painter->drawRect(rect);

        painter->setPen(QPen(Qt::black));
        painter->setFont(Tikzit::LABEL_FONT);
        painter->drawText(rect, Qt::AlignCenter, replaceTexConstants(_node->label()));
    }

    if (_node->data()->hasProperty("label")) {
        QString label = replaceTexConstants(_node->data()->property("label"));
        label.replace(QRegularExpression("^[^:]*:"), "");

        QRectF rect = outerLabelRect();
        QPen pen(QColor(0,0,200,120));
        QVector<qreal> d;
        d << 2.0 << 2.0;
        pen.setDashPattern(d);
        painter->setPen(pen);
        painter->setBrush(QBrush(QColor(100,255,255,120)));
        painter->drawRect(rect);

        painter->setPen(QPen(Qt::black));
        painter->setFont(Tikzit::LABEL_FONT);
        painter->drawText(rect, Qt::AlignCenter, label);
    }

    if (isSelected()) {
        QPainterPath sh = shape();
        QPainterPathStroker stroker;
        stroker.setWidth(4);
        QPainterPath outline = (stroker.createStroke(sh) + sh).simplified();
        painter->setPen(Qt::NoPen);
        painter->setBrush(QBrush(QColor(150,200,255,100)));
        painter->drawPath(outline);
    }
}

QPainterPath NodeItem::shape() const
{
    QPainterPath path;
    double rotate = _node->data()->property("rotate").toDouble();
    QTransform transform;
    transform.scale(GLOBAL_SCALEF, GLOBAL_SCALEF).rotate(rotate);

    // =================================================================
    // MGB-UML: DYNAMIC AUTO-RESIZING
    // =================================================================
    if (_node->style()->shape() == "rectangle split") {
        
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
        
        // Calculate required box sizes based on exact text width
        int padding = 10;
        QRect b1 = fmBold.boundingRect(QRect(0,0,1000,1000), Qt::AlignCenter, printPart1);
        QRect b2 = fm.boundingRect(QRect(0,0,1000,1000), Qt::AlignLeft, printPart2);
        QRect b3 = fm.boundingRect(QRect(0,0,1000,1000), Qt::AlignLeft, printPart3);
        
        int w = std::max({b1.width(), b2.width(), b3.width()}) + padding * 2;
        int h = b1.height() + b2.height() + b3.height() + padding * 3;
        
        // Set minimum UML boundaries so it doesn't vanish when empty
        if (w < 80) w = 80;
        if (h < 60) h = 60;
        
        // Unscale it back because transform.map() will multiply it by GLOBAL_SCALEF
        qreal sw = (w / 2.0) / GLOBAL_SCALEF;
        qreal sh = (h / 2.0) / GLOBAL_SCALEF;
        
        QVector<QPointF> points ({
            QPointF(-sw, -sh),
            QPointF(-sw,  sh),
            QPointF( sw,  sh),
            QPointF( sw, -sh)
        });
        QPolygonF rect(points);
        path.addPolygon(transform.map(rect));
        path.closeSubpath();
        
    } else if (_node->style()->shape() == "ellipse") {
        
        // Auto-resizing for Use Case
        QString label = replaceTexConstants(_node->label());
        QFontMetrics fm(Tikzit::LABEL_FONT);
        QRect b = fm.boundingRect(label);
        
        qreal sw = (b.width() / 2.0 + 15) / GLOBAL_SCALEF;
        qreal sh = (b.height() / 2.0 + 10) / GLOBAL_SCALEF;
        
        // Minimum oval limits
        if (sw < 0.5) sw = 0.5;
        if (sh < 0.25) sh = 0.25;
        
        path.addEllipse(QPointF(0, 0), sw * GLOBAL_SCALEF, sh * GLOBAL_SCALEF);
        
    } else if (_node->style()->shape() == "rectangle") {
        QVector<QPointF> points ({
            QPointF(-0.2, -0.2),
            QPointF(-0.2,  0.2),
            QPointF( 0.2,  0.2),
            QPointF( 0.2, -0.2)
        });
        QPolygonF rect(points);
        path.addPolygon(transform.map(rect));
        path.closeSubpath();
    } else if (_node->style()->shape() == "triangle") {
        QVector<QPointF> points ({
            QPointF(-0.2,  0.2),
            QPointF( 0.0, -0.1464),
            QPointF( 0.2,  0.2)
        });

        QPolygonF triangle(points);
        path.addPolygon(transform.map(triangle));
        path.closeSubpath();
    } else {
        path.addEllipse(QPointF(0, 0), GLOBAL_SCALEF * 0.2, GLOBAL_SCALEF * 0.2);
    }
    return path;
}

QRectF NodeItem::boundingRect() const
{
    return _boundingRect;
}

void NodeItem::updateBounds()
{
    prepareGeometryChange();
    QString label = _node->label();
    QString outerLabel = _node->data()->property("label");
    QRectF rect = shape().boundingRect();
    if (label != "") rect = rect.united(labelRect());
    if (outerLabel != "") rect = rect.united(outerLabelRect());
    _boundingRect = rect.adjusted(-4, -4, 4, 4);
}

Node *NodeItem::node() const
{
    return _node;
}
