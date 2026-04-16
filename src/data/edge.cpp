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

#include "edge.h"
#include "tikzit.h"
#include "util.h"

#include <QDebug>
#include <QPointF>
#include <QFontMetrics>
#include <QRegularExpression>
#include <algorithm>
#include <limits>
namespace {

static int umlLengthToPixelsForEdge(QString raw, int fallbackPx)
{
    QString s = raw.trimmed();
    if (s.isEmpty()) return fallbackPx;

    bool ok = false;
    if (s.endsWith("cm")) {
        double cm = s.left(s.size() - 2).toDouble(&ok);
        if (ok) return static_cast<int>(cm * GLOBAL_SCALEF);
    } else if (s.endsWith("pt")) {
        double pt = s.left(s.size() - 2).toDouble(&ok);
        if (ok) return static_cast<int>(pt * (GLOBAL_SCALEF / 28.3465));
    } else {
        double v = s.toDouble(&ok);
        if (ok) return static_cast<int>(v * GLOBAL_SCALEF);
    }

    return fallbackPx;
}
static QSizeF rectangleHalfExtentsForEdge(const Node *node)
{
    QString widthProp = node->data()->property("minimum width");
    if (widthProp.isEmpty() && node->style() && node->style()->data()) {
        widthProp = node->style()->data()->property("minimum width");
    }

    QString heightProp = node->data()->property("minimum height");
    if (heightProp.isEmpty() && node->style() && node->style()->data()) {
        heightProp = node->style()->data()->property("minimum height");
    }

    int w = umlLengthToPixelsForEdge(widthProp, 120);
    int h = umlLengthToPixelsForEdge(heightProp, 120);

    // Make rectangle split match the class renderer better
    if (node->style() && node->style()->shape() == "rectangle split") {
        QString label = node->label();
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

        QFont bodyFont("Helvetica", 10);
        QFont titleFont = bodyFont;
        titleFont.setBold(true);

        QFontMetrics fm(bodyFont);
        QFontMetrics fmTitle(titleFont);

        int padding = 10;

        QRect b1 = fmTitle.boundingRect(QRect(0,0,1000,1000), Qt::AlignCenter, printPart1);
        QRect b2 = fm.boundingRect(QRect(0,0,1000,1000), Qt::AlignLeft, printPart2);
        QRect b3 = fm.boundingRect(QRect(0,0,1000,1000), Qt::AlignLeft, printPart3);

        w = std::max(w, std::max({b1.width(), b2.width(), b3.width(), 60}) + padding * 2);
        h = std::max(h, std::max(b1.height() + b2.height() + b3.height() + padding * 3, 40));
    }

    return QSizeF((w / 2.0) / GLOBAL_SCALEF, (h / 2.0) / GLOBAL_SCALEF);
}

static QPointF anchorPointTowardForEdge(const Node *node, qreal angleR)
{
    QPointF c = node->point();
    QString shape = node->style() ? node->style()->shape() : "";

    // Generic rule for rectangle-like shapes
    if (shape == "rectangle" || shape == "rectangle split") {
        QSizeF half = rectangleHalfExtentsForEdge(node);

        qreal dx = std::cos(angleR);
        qreal dy = std::sin(angleR);

        qreal tx = std::numeric_limits<qreal>::infinity();
        qreal ty = std::numeric_limits<qreal>::infinity();

        if (!almostZero(dx)) tx = half.width() / std::abs(dx);
        if (!almostZero(dy)) ty = half.height() / std::abs(dy);

        qreal t = std::min(tx, ty);
        return QPointF(c.x() + dx * t, c.y() + dy * t);
    }

    // Fallback for everything else
    if (node->style() && node->style()->isNone()) {
        return c;
    }

    return QPointF(c.x() + std::cos(angleR) * 0.2,
                   c.y() + std::sin(angleR) * 0.2);
}

static QPointF nodeHalfExtents(const Node *node)
{
    QString shapeName = node->style()->shape();

    if (shapeName == "rectangle split") {
        QString label = node->label();
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
        QFont boldFont = Tikzit::LABEL_FONT;
        boldFont.setBold(true);
        QFontMetrics fmBold(boldFont);

        int padding = 10;
        QRect b1 = fmBold.boundingRect(QRect(0,0,1000,1000), Qt::AlignCenter, printPart1);
        QRect b2 = fm.boundingRect(QRect(0,0,1000,1000), Qt::AlignLeft, printPart2);
        QRect b3 = fm.boundingRect(QRect(0,0,1000,1000), Qt::AlignLeft, printPart3);

        int w = std::max({b1.width(), b2.width(), b3.width()}) + padding * 2;
        int h = b1.height() + b2.height() + b3.height() + padding * 3;

        if (w < 80) w = 80;
        if (h < 60) h = 60;

        return QPointF((w / 2.0) / GLOBAL_SCALEF, (h / 2.0) / GLOBAL_SCALEF);
    }

    if (shapeName == "ellipse") {
        QString label = replaceTexConstants(node->label());
        QFontMetrics fm(Tikzit::LABEL_FONT);
        QRect b = fm.boundingRect(label);
        qreal hw = (b.width() / 2.0 + 15) / GLOBAL_SCALEF;
        qreal hh = (b.height() / 2.0 + 10) / GLOBAL_SCALEF;
        if (hw < 0.5) hw = 0.5;
        if (hh < 0.25) hh = 0.25;
        return QPointF(hw, hh);
    }

    if (shapeName == "uml actor") {
        return QPointF(0.22, 0.57);
    }

    if (shapeName == "uml system") {
        QString label = replaceTexConstants(node->label());
        QFont titleFont = Tikzit::LABEL_FONT;
        titleFont.setBold(true);
        QFontMetrics fm(titleFont);
        QRect b = fm.boundingRect(label);

        QString widthProp = node->data()->property("minimum width");
        if (widthProp.isEmpty()) widthProp = node->style()->data()->property("minimum width");

        QString heightProp = node->data()->property("minimum height");
        if (heightProp.isEmpty()) heightProp = node->style()->data()->property("minimum height");

        int minW = umlLengthToPixelsForEdge(widthProp, 240);
        int minH = umlLengthToPixelsForEdge(heightProp, 160);
        int w = std::max(b.width() + 40, minW);
        int h = std::max(b.height() + 40, minH);

        return QPointF((w / 2.0) / GLOBAL_SCALEF, (h / 2.0) / GLOBAL_SCALEF);
    }

    if (shapeName == "rectangle") return QPointF(0.2, 0.2);
    if (shapeName == "triangle") return QPointF(0.2, 0.2);

    return QPointF(0.2, 0.2);
}

static qreal tipPadding(Style::ArrowTipStyle tip)
{
    switch (tip) {
    case Style::Pointer:
        return 0.10;
    case Style::Flat:
        return 0.08;
    case Style::OpenTriangle:
        return 0.14;
    case Style::Diamond:
    case Style::FilledDiamond:
        return 0.16;
    case Style::NoTip:
        return 0.0;
    }
    return 0.0;
}

static QPointF pointOnNodeBoundary(const Node *node, qreal angleR)
{
    if (node->style()->isNone()) return node->point();

    QPointF half = nodeHalfExtents(node);
    qreal dx = std::cos(angleR);
    qreal dy = std::sin(angleR);
    qreal hw = std::max<qreal>(half.x(), 0.0001);
    qreal hh = std::max<qreal>(half.y(), 0.0001);
    QPointF c = node->point();

    if (node->style()->shape() == "ellipse") {
        qreal denom = std::sqrt((dx * dx) / (hw * hw) + (dy * dy) / (hh * hh));
        if (denom > 0.0) return QPointF(c.x() + dx / denom, c.y() + dy / denom);
    } else {
        qreal tx = almostZero(dx) ? std::numeric_limits<qreal>::infinity() : std::abs(hw / dx);
        qreal ty = almostZero(dy) ? std::numeric_limits<qreal>::infinity() : std::abs(hh / dy);
        qreal t = std::min(tx, ty);
        if (std::isfinite(t)) return QPointF(c.x() + dx * t, c.y() + dy * t);
    }

    return QPointF(c.x() + dx * 0.2, c.y() + dy * 0.2);
}

} // namespace

Edge::Edge(Node *s, Node *t, QObject *parent) :
    QObject(parent), _source(s), _target(t)
{
    _data = new GraphElementData(this);
    _edgeNode = nullptr;
    _path = nullptr;
    _dirty = true;

    if (s != t) {
        _basicBendMode = true;
        _bend = 0;
        _inAngle = 0;
        _outAngle = 0;
        _weight = 0.4;
    } else {
        _basicBendMode = false;
        _bend = 0;
        _inAngle = 135;
        _outAngle = 45;
        _weight = 1.0;
    }
	_style = noneEdgeStyle;
    updateControls();
}

/*!
 * @brief Edge::copy makes a deep copy of an edge.
 * @param nodeTable is an optional pointer to a table mapping the old source/target
 * node pointers to their new, copied versions. This is used when making a copy of
 * an entire (sub)graph.
 * @return a copy of the edge
 */
Edge *Edge::copy(QMap<Node*,Node*> *nodeTable)
{
    Edge *e;
    if (nodeTable == nullptr) {
        e = new Edge(_source, _target);
    } else {
        Node *s = nodeTable->value(_source);
        Node *t = nodeTable->value(_target);
        e = new Edge((s != nullptr) ? s : _source,
                     (t != nullptr) ? t : _target);
    }
    e->setData(_data->copy());
    e->setBasicBendMode(_basicBendMode);
    e->setBend(_bend);
    e->setInAngle(_inAngle);
    e->setOutAngle(_outAngle);
    e->setWeight(_weight);
	e->attachStyle();
    e->updateControls();
    return e;
}

Node *Edge::source() const
{
    return _source;
}

Node *Edge::target() const
{
    return _target;
}

bool Edge::isSelfLoop()
{
    return (_source == _target);
}

bool Edge::isStraight()
{
    return (_basicBendMode && _bend == 0);
}

GraphElementData *Edge::data() const
{
    return _data;
}

void Edge::setData(GraphElementData *data)
{
    GraphElementData *oldData = _data;
    _data = data;
    oldData->deleteLater();
    setAttributesFromData();
}

QString Edge::styleName() const
{
	QString nm = _data->property("style");
	if (nm.isNull()) return "none";
	else return nm;
}

void Edge::setStyleName(const QString &styleName)
{
	if (!styleName.isNull() && styleName != "none") _data->setProperty("style", styleName);
	else _data->unsetProperty("style");
}

QString Edge::sourceAnchor() const
{
    return _sourceAnchor;
}

void Edge::setSourceAnchor(const QString &sourceAnchor)
{
    _sourceAnchor = sourceAnchor;
}

QString Edge::targetAnchor() const
{
    return _targetAnchor;
}

void Edge::setTargetAnchor(const QString &targetAnchor)
{
    _targetAnchor = targetAnchor;
}

Node *Edge::edgeNode() const
{
    return _edgeNode;
}

void Edge::setEdgeNode(Node *edgeNode)
{
    Node *oldEdgeNode = _edgeNode;
    _edgeNode = edgeNode;
    if (oldEdgeNode != nullptr) oldEdgeNode->deleteLater();
}

bool Edge::hasEdgeNode()
{
    return _edgeNode != nullptr;
}

void Edge::updateControls() {
    //if (_dirty) {
    QPointF src = _source->point();
    QPointF targ = _target->point();

    qreal dx = (targ.x() - src.x());
    qreal dy = (targ.y() - src.y());

    qreal outAngleR = 0.0;
    qreal inAngleR = 0.0;
	

_tail = anchorPointTowardForEdge(_source, outAngleR);
_head = anchorPointTowardForEdge(_target, inAngleR);

    // TODO: calculate head and tail properly, not just for circles
QPointF tailDir(std::cos(outAngleR), std::sin(outAngleR));
QPointF headDir(std::cos(inAngleR), std::sin(inAngleR));

_tail = pointOnNodeBoundary(_source, outAngleR) + tailDir * tipPadding(_style->arrowTail());
_head = pointOnNodeBoundary(_target, inAngleR) + headDir * tipPadding(_style->arrowHead());
    // give a default distance for self-loops
    _cpDist = (almostZero(dx) && almostZero(dy)) ? _weight : std::sqrt(dx*dx + dy*dy) * _weight;

    _cp1 = QPointF(src.x() + (_cpDist * std::cos(outAngleR)),
                   src.y() + (_cpDist * std::sin(outAngleR)));

    _cp2 = QPointF(targ.x() + (_cpDist * std::cos(inAngleR)),
                   targ.y() + (_cpDist * std::sin(inAngleR)));

    _mid = bezierInterpolateFull (0.5, _tail, _cp1, _cp2, _head);
    _tailTangent = bezierTangent(0.0, 0.1);
    _headTangent = bezierTangent(1.0, 0.9);
}

void Edge::setAttributesFromData()
{
    _basicBendMode = true;
    bool ok = true;

    if (_data->atom("bend left")) {
        _bend = -30;
    } else if (_data->atom("bend right")) {
        _bend = 30;
    } else if (_data->property("bend left") != nullptr) {
        _bend = -_data->property("bend left").toInt(&ok);
        if (!ok) _bend = -30;
    } else if (_data->property("bend right") != nullptr) {
        _bend = _data->property("bend right").toInt(&ok);
        if (!ok) _bend = 30;
    } else {
        _bend = 0;

        if (_data->property("in") != nullptr && _data->property("out") != nullptr) {
            _basicBendMode = false;
            _inAngle = _data->property("in").toInt(&ok);
            if (!ok) _inAngle = 0;
            _outAngle = _data->property("out").toInt(&ok);
            if (!ok) _outAngle = 180;
        }
    }

    if (!_data->property("looseness").isNull()) {
        _weight = _data->property("looseness").toDouble(&ok) / 2.5;
        if (!ok) _weight = 0.4;
    } else {
        _weight = (isSelfLoop()) ? 1.0 : 0.4;
    }

    //qDebug() << "bend: " << _bend << " in: " << _inAngle << " out: " << _outAngle;
    _dirty = true;
}

void Edge::updateData()
{
    _data->unsetAtom("loop");
    _data->unsetProperty("in");
    _data->unsetProperty("out");
    _data->unsetAtom("bend left");
    _data->unsetAtom("bend right");
    _data->unsetProperty("bend left");
    _data->unsetProperty("bend right");
    _data->unsetProperty("looseness");

    if (_basicBendMode) {
        if (_bend != 0) {
            QString bendKey;
            int b;
            if (_bend < 0) {
                bendKey = "bend left";
                b = -_bend;
            } else {
                bendKey = "bend right";
                b = _bend;
            }

            if (b == 30) {
                _data->setAtom(bendKey);
            } else {
                _data->setProperty(bendKey, QString::number(b));
            }
        }
    } else {
        _data->setProperty("in", QString::number(_inAngle));
        _data->setProperty("out", QString::number(_outAngle));
    }

    if (_source == _target) _data->setAtom("loop");
    if (!isSelfLoop() && !isStraight() && !almostEqual(_weight, 0.4))
        _data->setProperty("looseness", QString::number(_weight*2.5, 'f', 2));
    if (_source->isBlankNode()) _sourceAnchor = "center";
    else _sourceAnchor = "";
    if (_target->isBlankNode()) _targetAnchor = "center";
    else _targetAnchor = "";

}


QPointF Edge::head() const
{
    return _head;
}

QPointF Edge::tail() const
{
    return _tail;
}

QPointF Edge::cp1() const
{
    return _cp1;
}

QPointF Edge::cp2() const
{
    return _cp2;
}

int Edge::bend() const
{
    return _bend;
}

int Edge::inAngle() const
{
    return _inAngle;
}

int Edge::outAngle() const
{
    return _outAngle;
}

qreal Edge::weight() const
{
    return _weight;
}

bool Edge::basicBendMode() const
{
    return _basicBendMode;
}

qreal Edge::cpDist() const
{
    return _cpDist;
}

void Edge::setBasicBendMode(bool mode)
{
    _basicBendMode = mode;
}

void Edge::setBend(int bend)
{
    _bend = bend;
}

void Edge::setInAngle(int inAngle)
{
    _inAngle = inAngle;
}

void Edge::setOutAngle(int outAngle)
{
    _outAngle = outAngle;
}

void Edge::setWeight(qreal weight)
{
    _weight = weight;
}

void Edge::reverse()
{
    Node *n = _source;
    _source = _target;
    _target = n;
    int a = _inAngle;
    _inAngle = _outAngle;
    _outAngle = a;
    _bend = -_bend;
    updateData();
}

int Edge::tikzLine() const
{
    return _tikzLine;
}

void Edge::setTikzLine(int tikzLine)
{
    _tikzLine = tikzLine;
}

QPointF Edge::mid() const
{
    return _mid;
}

QPointF Edge::headTangent() const
{
	return _headTangent;
}

QPointF Edge::tailTangent() const
{
	return _tailTangent;
}

void Edge::attachStyle()
{
	QString nm = styleName();
	if (nm.isNull()) _style = noneEdgeStyle;
	else _style = tikzit->styles()->edgeStyle(nm);
}

Style *Edge::style() const
{
	return _style;
}

QPointF Edge::bezierTangent(qreal start, qreal end) const
{
	qreal dx = bezierInterpolate(end, _tail.x(), _cp1.x(), _cp2.x(), _head.x()) -
		bezierInterpolate(start, _tail.x(), _cp1.x(), _cp2.x(), _head.x());
	qreal dy = bezierInterpolate(end, _tail.y(), _cp1.y(), _cp2.y(), _head.y()) -
		bezierInterpolate(start, _tail.y(), _cp1.y(), _cp2.y(), _head.y());

	// normalise
	qreal len = sqrt(dx*dx + dy*dy);
	if (!almostZero(len)) {
		dx = (dx / len) * 0.1;
		dy = (dy / len) * 0.1;
	}

	return QPointF(dx, dy);
}

Path *Edge::path() const
{
    return _path;
}

void Edge::setPath(Path *path)
{
    _path = path;
}
