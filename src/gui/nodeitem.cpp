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
#include "mgbPluginManager.h"
#include "mgbPluginInterface.h"
#include <cmath>
#include <algorithm>
#include <QPen>
#include <QApplication>
#include <QBrush>
#include <QDebug>
#include <QFont>
#include <QFontMetrics>
#include <QPainterPathStroker>

//read width/height from node properties
namespace {

static int umlLengthToPixels(QString raw, int fallbackPx)
{
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
static QPainterPath rectanglePathFromProperties(
    const Node *node,
    const QTransform &transform,
    int fallbackW,
    int fallbackH,
    bool fitLabel = false,
    bool boldLabel = false)
{
    QString widthProp = node->data()->property("minimum width");
    if (widthProp.isEmpty()) {
        widthProp = node->style()->data()->property("minimum width");
    }

    QString heightProp = node->data()->property("minimum height");
    if (heightProp.isEmpty()) {
        heightProp = node->style()->data()->property("minimum height");
    }

    int minW = umlLengthToPixels(widthProp, fallbackW);
    int minH = umlLengthToPixels(heightProp, fallbackH);

    int w = minW;
    int h = minH;

    if (fitLabel) {
        QString label = replaceTexConstants(node->label());
        QFont font = Tikzit::LABEL_FONT;
        font.setBold(boldLabel);
        QFontMetrics fm(font);
        QRect b = fm.boundingRect(label);

        w = std::max(w, b.width() + 40);
        h = std::max(h, b.height() + 40);
    }

    qreal sw = (w / 2.0) / GLOBAL_SCALEF;
    qreal sh = (h / 2.0) / GLOBAL_SCALEF;

    QVector<QPointF> points({
        QPointF(-sw, -sh),
        QPointF(-sw,  sh),
        QPointF( sw,  sh),
        QPointF( sw, -sh)
    });

    QPainterPath path;
    path.addPolygon(transform.map(QPolygonF(points)));
    path.closeSubpath();
    return path;
}

}

// =================================================================
// THE FACTORY MANAGER
// =================================================================
NodeItem* NodeItem::createNode(Node *node)
{
    NodeItem *ni = nullptr;

    QList<mgb::ElementPluginInterface*> plugins = mgb::PluginManager::instance().getCompiledInterfaces();
    // 1. Ask compiled C++ Plugins FIRST.
    foreach (mgb::ElementPluginInterface* plugin, plugins) {
        ni = plugin->createCustomNode(node);
        if (ni != nullptr) {
            break;
        }
    }

    // 2. If no plugin claimed this node, default to the standard circle.
    if (ni == nullptr) {
        ni = new NodeItem(node);
    }

    ni->updateBounds();
    return ni;
}

NodeItem::NodeItem(Node *node)
{
    _node = node;
    setFlag(QGraphicsItem::ItemIsSelectable);
    readPos();
    // CRITICAL FIX: Do NOT call updateBounds() here! It causes the Virtual Constructor Trap.
}

void NodeItem::readPos() { setPos(toScreen(_node->point())); }
void NodeItem::writePos() { _node->setPoint(fromScreen(pos())); }
Node *NodeItem::node() const { return _node; }
QRectF NodeItem::boundingRect() const { return _boundingRect; }

QRectF NodeItem::labelRect() const 
{
    QString label = replaceTexConstants(_node->label());
    QFontMetrics fm(Tikzit::LABEL_FONT);
    QRectF rect = fm.boundingRect(label);
    rect.moveCenter(QPointF(0,0));
    return rect;
}

QRectF NodeItem::outerLabelRect() const 
{
    QString label = replaceTexConstants(_node->data()->property("label"));
    label.replace(QRegularExpression("^[^:]*:"), "");
    QFontMetrics fm(Tikzit::LABEL_FONT);
    QRectF rect = fm.boundingRect(label);
    rect.moveCenter(QPointF(0, -0.5 * GLOBAL_SCALEF));
    return rect;
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

void NodeItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    if (_node->style()->isNone()) {
        QColor c(180,180,200);
        painter->setPen(QPen(c));
        painter->setBrush(QBrush(c));
        painter->drawEllipse(QPointF(0,0), 1,1);

        QPen pen(QColor(180,180,220));
        QVector<qreal> p; p << 1.0 << 2.0;
        pen.setDashPattern(p);
        pen.setWidthF(2.0);
        painter->setPen(pen);
        painter->setBrush(Qt::NoBrush);
        painter->drawPath(shape());
    } else {
        QPen pen(_node->style()->strokeColor());
        pen.setWidth(_node->style()->strokeThickness());
        painter->setPen(pen);
        painter->setBrush(QBrush(_node->style()->fillColor()));
        painter->drawPath(shape());
    }

    bool drawLabel = (_node->label() != "");
    if (scene()) {
        TikzScene *sc = static_cast<TikzScene*>(scene());
        drawLabel = drawLabel && sc->drawNodeLabels();
    }

    if (drawLabel) {
        QRectF rect = labelRect();
        painter->setPen(QPen(Qt::black));
        painter->setFont(Tikzit::LABEL_FONT);
        painter->drawText(rect, Qt::AlignCenter, replaceTexConstants(_node->label()));
    }

    paintSelectionAndOuterLabels(painter);
}

void NodeItem::paintSelectionAndOuterLabels(QPainter *painter)
{
    if (_node->data()->hasProperty("label")) {
        QString label = replaceTexConstants(_node->data()->property("label"));
        label.replace(QRegularExpression("^[^:]*:"), "");
        QRectF rect = outerLabelRect();
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

if (_node->style()->shape() == "rectangle") {
    path = rectanglePathFromProperties(_node, transform, 120, 120, false, false);
} else if (_node->style()->shape() == "triangle") {
        QVector<QPointF> points ({ QPointF(-0.2,  0.2), QPointF( 0.0, -0.1464), QPointF( 0.2,  0.2) });
        path.addPolygon(transform.map(QPolygonF(points)));
        path.closeSubpath();
    } else {
        path.addEllipse(QPointF(0, 0), GLOBAL_SCALEF * 0.2, GLOBAL_SCALEF * 0.2);
    }
    return path;
}
