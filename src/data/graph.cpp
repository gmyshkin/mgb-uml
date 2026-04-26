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

#include "graph.h"
#include "util.h"

#include <QTextStream>
#include <QSet>
#include <QtAlgorithms>
#include <QDebug>
#include <algorithm>

namespace {

QString nodePropertyWithStyleDefault(Node *node, const QString &key, const QString &fallback)
{
    QString value = node->data()->property(key);
    if (value.isEmpty() && node->style() && node->style()->data()) {
        value = node->style()->data()->property(key);
    }
    return value.isEmpty() ? fallback : value;
}

QString nodeDrawOptions(Node *node, const QString &shape, const QString &fallbackWidth, const QString &fallbackHeight)
{
    QStringList options;
    options << "draw=" + nodePropertyWithStyleDefault(node, "draw", "black");
    options << "fill=" + nodePropertyWithStyleDefault(node, "fill", "white");
    options << "line width=" + nodePropertyWithStyleDefault(node, "line width", "0.6pt");
    options << "minimum width=" + nodePropertyWithStyleDefault(node, "minimum width", fallbackWidth);
    options << "minimum height=" + nodePropertyWithStyleDefault(node, "minimum height", fallbackHeight);
    options << "inner sep=0pt";
    options << "outer sep=0pt";
    options << "shape=" + shape;
    return "[" + options.join(", ") + "]";
}

QString nodeFontOption(Node *node, const QString &fallback)
{
    return "font={" + nodePropertyWithStyleDefault(node, "font", fallback) + "}";
}

QStringList umlClassParts(const QString &label)
{
    QStringList parts;
    QString part1 = label;
    QString part2;
    QString part3;

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

    parts << part1 << part2 << part3;
    return parts;
}

void writeUmlSystemNode(QTextStream &code, Node *node)
{
    code << "\t\t\\node " << nodeDrawOptions(node, "rectangle", "4cm", "6cm")
         << " (" << node->name() << ") at ("
         << floatToString(node->point().x()) << ", "
         << floatToString(node->point().y()) << ") {};\n";

    code << "\t\t\\node [anchor=north west, align=left, inner sep=0pt, outer sep=0pt, "
         << nodeFontOption(node, "\\bfseries\\sffamily\\fontsize{10pt}{12pt}\\selectfont")
         << "] at ([xshift=0.25cm,yshift=-0.15cm]" << node->name() << ".north west) {"
         << node->label() << "};\n";
}

void writeUmlClassNode(QTextStream &code, Node *node)
{
    QStringList parts = umlClassParts(node->label());
    QString stroke = nodePropertyWithStyleDefault(node, "draw", "black");
    QString lineWidth = nodePropertyWithStyleDefault(node, "line width", "0.6pt");
    QString font = nodePropertyWithStyleDefault(node, "font", "\\sffamily\\fontsize{10pt}{12pt}\\selectfont");

    code << "\t\t\\node " << nodeDrawOptions(node, "rectangle", "3cm", "2cm")
         << " (" << node->name() << ") at ("
         << floatToString(node->point().x()) << ", "
         << floatToString(node->point().y()) << ") {};\n";

    code << "\t\t\\draw [draw=" << stroke << ", line width=" << lineWidth << "] "
         << "([yshift=-0.60cm]" << node->name() << ".north west) -- "
         << "([yshift=-0.60cm]" << node->name() << ".north east);\n";

    code << "\t\t\\draw [draw=" << stroke << ", line width=" << lineWidth << "] "
         << "([yshift=-1.10cm]" << node->name() << ".north west) -- "
         << "([yshift=-1.10cm]" << node->name() << ".north east);\n";

    code << "\t\t\\node [anchor=north, align=center, inner sep=0pt, outer sep=0pt, "
         << "font={\\bfseries" << font << "}] at ([yshift=-0.18cm]" << node->name()
         << ".north) {" << parts.value(0) << "};\n";

    code << "\t\t\\node [anchor=north west, align=left, inner sep=0pt, outer sep=0pt, "
         << "font={" << font << "}] at ([xshift=0.12cm,yshift=-0.72cm]" << node->name()
         << ".north west) {" << parts.value(1) << "};\n";

    code << "\t\t\\node [anchor=north west, align=left, inner sep=0pt, outer sep=0pt, "
         << "font={" << font << "}] at ([xshift=0.12cm,yshift=-1.22cm]" << node->name()
         << ".north west) {" << parts.value(2) << "};\n";
}

int writeCustomUmlNode(QTextStream &code, Node *node)
{
    if (!node->style()) return 0;

    if (node->styleName() == "UML System" || node->style()->shape() == "uml system") {
        writeUmlSystemNode(code, node);
        return 2;
    }

    if (node->styleName() == "UML Class") {
        writeUmlClassNode(code, node);
        return 6;
    }

    return 0;
}

}


Graph::Graph(QObject *parent) : QObject(parent)
{
    _data = new GraphElementData(this);
    _bbox = QRectF(0,0,0,0);
}

Graph::~Graph()
{
}

// add a node. The graph claims ownership.
void Graph::addNode(Node *n) {
    n->setParent(this);
    _nodes << n;
}

void Graph::addNode(Node *n, int index)
{
    n->setParent(this);
    _nodes.insert(index, n);
}

void Graph::removeNode(Node *n) {
    // the node itself is not deleted, as it may still be referenced in an undo command. It will
    // be deleted when graph is, via QObject memory management.
    _nodes.removeOne(n);
}


void Graph::addEdge(Edge *e)
{
    e->setParent(this);
    _edges << e;
}

void Graph::addEdge(Edge *e, int index)
{
    e->setParent(this);
    _edges.insert(index, e);
}

void Graph::removeEdge(Edge *e)
{
    // the edge itself is not deleted, as it may still be referenced in an undo command. It will
    // be deleted when graph is, via QObject memory management.
    _edges.removeOne(e);
}

void Graph::addPath(Path *p)
{
    p->setParent(this);
    _paths << p;
}

void Graph::removePath(Path *p)
{
    _paths.removeOne(p);
}

int Graph::maxIntName()
{
    int max = -1;
    int i;
    bool ok;
    foreach (Node *n, _nodes) {
        i = n->name().toInt(&ok);
        if (ok && i > max) max = i;
    }
    return max;
}

void Graph::reorderNodes(const QVector<Node *> &newOrder)
{
    _nodes = newOrder;
}

void Graph::reorderEdges(const QVector<Edge *> &newOrder)
{
    _edges = newOrder;
}

QRectF Graph::realBbox()
{
    //float maxX = 0.0f;
    QRectF rect = bbox();
    foreach (Node *n, _nodes) {
        rect = rect.united(QRectF(n->point().x()-0.5f,
                                  n->point().y()-0.5f,
                                  1.0f, 1.0f));
    }

    return rect;
}

QRectF Graph::boundsForNodes(QSet<Node*>nds) {
	QPointF p;
	QPointF tl;
	QPointF br;
	bool hasPoints = false;
	foreach (Node *n, nds) {
		p = n->point();
		if (!hasPoints) {
			hasPoints = true;
			tl = p;
			br = p;
		} else {
			if (p.x() < tl.x()) tl.setX(p.x());
			if (p.y() > tl.y()) tl.setY(p.y());
			if (p.x() > br.x()) br.setX(p.x());
			if (p.y() < br.y()) br.setY(p.y());
		}
	}

	QRectF rect(tl, br);
	return rect;
}

QString Graph::freshNodeName()
{
    return QString::number(maxIntName() + 1);
}

void Graph::renameApart(Graph *graph)
{
    int i = graph->maxIntName() + 1;
    foreach (Node *n, _nodes) {
        n->setName(QString::number(i));
        i++;
    }
}

GraphElementData *Graph::data() const
{
    return _data;
}

void Graph::setData(GraphElementData *data)
{
    GraphElementData *oldData = _data;
    _data = data;
    oldData->deleteLater();
}

const QVector<Node*> &Graph::nodes()
{
    return _nodes;
}

const QVector<Edge*> &Graph::edges()
{
    return _edges;
}

const QVector<Path *> &Graph::paths()
{
    return _paths;
}

QRectF Graph::bbox() const
{
    return _bbox;
}

bool Graph::hasBbox() {
    return !(_bbox == QRectF(0,0,0,0));
}

void Graph::clearBbox() {
    _bbox = QRectF(0,0,0,0);
}

QString Graph::tikz()
{
    QString str;
    QTextStream code(&str);
    int line = 0;

    code << "\\begin{tikzpicture}";
    if (!_data->isEmpty()) {
        QString dataStr = _data->tikz();
        code << "[" << dataStr.mid(1, dataStr.length() - 2) << "]";
    }
    code << "\n";
    if (hasBbox()) {
        code << "\t\\path [use as bounding box] ("
             << floatToString(_bbox.topLeft().x()) << "," << floatToString(_bbox.topLeft().y())
             << ") rectangle ("
             << floatToString(_bbox.bottomRight().x()) << "," << floatToString(_bbox.bottomRight().y())
             << ");\n";
        line++;
    }

    if (!_nodes.isEmpty()) {
        code << "\t\\begin{pgfonlayer}{nodelayer}\n";
        line++;
    }

    Node *n;
    foreach (n, _nodes) {
        n->setTikzLine(line);
        int customLines = writeCustomUmlNode(code, n);
        if (customLines > 0) {
            line += customLines;
            continue;
        }

        code << "\t\t\\node ";

        if (!n->data()->isEmpty())
            code << n->data()->tikz() << " ";

        code << "(" << n->name() << ") at ("
             << floatToString(n->point().x())
             << ", "
             << floatToString(n->point().y())
             << ") {" << n->label() << "};\n";
        line++;
    }

    if (!_nodes.isEmpty()) {
        code << "\t\\end{pgfonlayer}\n";
        line++;
    }

    if (!_edges.isEmpty()) {
        code << "\t\\begin{pgfonlayer}{edgelayer}\n";
        line++;
    }


    Edge *e;
    Path *p;
    foreach (e, _edges) {
        p = e->path();
        if (p) { // if edge is part of a path
            if (p->edges().first() == e) { // only add tikz code once per path
                e->setTikzLine(line);
                e->updateData();
                code << "\t\t\\draw ";

                GraphElementData *npd = e->data()->nonPathData();
                if (!npd->isEmpty())
                    code << npd->tikz() << " ";
                delete npd;

                code << "(" << e->source()->name();
                if (e->sourceAnchor() != "") {
                    code << "." << e->sourceAnchor();
                } else if (p->isCycle()) {
                    code << ".center";
                }
                code << ")";

                foreach (Edge *e1, p->edges()) {
                    code << "\n\t\t\t to ";
                    line++;
                    e1->setTikzLine(line);
                    e1->updateData();

                    GraphElementData *pd = e1->data()->pathData();
                    if (!pd->isEmpty())
                        code << pd->tikz() << " ";
                    delete pd;

                    if (e1->hasEdgeNode()) {
                        code << "node ";
                        if (!e1->edgeNode()->data()->isEmpty())
                            code << e1->edgeNode()->data()->tikz() << " ";
                        code << "{" << e1->edgeNode()->label() << "} ";
                    }

                    if (e->source() == e1->target()) {
                        code << "cycle";
                    } else {
                        code << "(" << e1->target()->name();
                        if (e1->targetAnchor() != "") {
                            code << "." << e1->targetAnchor();
                        } else if (e1 != p->edges().last()) {
                            code << ".center";
                        }
                        code << ")";
                    }
                }
                code << ";\n";
                line++;
            }
        } else { // edge is not part of a path
            e->setTikzLine(line);
            e->updateData();
            code << "\t\t\\draw ";

            if (!e->data()->isEmpty())
                code << e->data()->tikz() << " ";

            code << "(" << e->source()->name();
            if (e->sourceAnchor() != "")
                code << "." << e->sourceAnchor();
            code << ") to ";

            if (e->hasEdgeNode()) {
                code << "node ";
                if (!e->edgeNode()->data()->isEmpty())
                    code << e->edgeNode()->data()->tikz() << " ";
                code << "{" << e->edgeNode()->label() << "} ";
            }

            if (e->source() == e->target()) {
                code << "()";
            } else {
                code << "(" << e->target()->name();
                if (e->targetAnchor() != "")
                    code << "." << e->targetAnchor();
                code << ")";
            }

            code << ";\n";
            line++;
        }
    }

    if (!_edges.isEmpty()) {
        code << "\t\\end{pgfonlayer}\n";
        line++;
    }

    code << "\\end{tikzpicture}\n";
    line++;

    code.flush();
    return str;
}

Graph *Graph::copyOfSubgraphWithNodes(QSet<Node *> nds)
{
    Graph *g = new Graph();
    g->setData(_data->copy());
    QMap<Node*,Node*> nodeTable;
    foreach (Node *n, nodes()) {
        if (nds.contains(n)) {
            Node *n1 = n->copy();
            nodeTable.insert(n, n1);
            g->addNode(n1);
        }
    }

    QMap<Edge*,Edge*> edgeTable;
    foreach (Edge *e, edges()) {
        if (nds.contains(e->source()) && nds.contains(e->target())) {
            Edge *e1 = e->copy(&nodeTable);
            g->addEdge(e1);
            edgeTable.insert(e,e1);
        }
    }

    // add a copy of a path to the new graph if all of the edges are there
    foreach (Path *p, paths()) {
        bool allEdges = true;
        Path *p1 = new Path();
        foreach (Edge *e1, p->edges()) {
            if (edgeTable.contains(e1)) {
                p1->addEdge(edgeTable[e1]);
            } else {
                allEdges = false;
                break;
            }
        }
        if (allEdges) {
            g->addPath(p1);
        } else {
            p1->removeEdges();
            delete p1;
        }
    }

    return g;
}

void Graph::insertGraph(Graph *graph)
{
    QMap<Node*,Node*> nodeTable;
    foreach (Node *n, graph->nodes()) addNode(n);
    foreach (Edge *e, graph->edges()) addEdge(e);
}

void Graph::reflectNodes(QSet<Node*> nds, bool horizontal)
{
    QRectF bds = boundsForNodes(nds);
    float ctr;
    if (horizontal) ctr = bds.center().x();
    else ctr = bds.center().y();

    QPointF p;
    foreach(Node *n, nds) {
        p = n->point();
        if (horizontal) p.setX(2 * ctr - p.x());
        else p.setY(2 * ctr - p.y());
        n->setPoint(p);
    }

    foreach (Edge *e, _edges) {
        if (nds.contains(e->source()) && nds.contains(e->target())) {
            if (!e->basicBendMode()) {
                if (horizontal) {
                    if (e->inAngle() < 0) e->setInAngle(-180 - e->inAngle());
                    else e->setInAngle(180 - e->inAngle());

                    if (e->outAngle() < 0) e->setOutAngle(-180 - e->outAngle());
                    else e->setOutAngle(180 - e->outAngle());
                }
                else {
                    e->setInAngle(-e->inAngle());
                    e->setOutAngle(-e->outAngle());
                }
            }
            else {
                e->setBend(-e->bend());
            }
        }
    }
}

void Graph::rotateNodes(QSet<Node*> nds, bool clockwise)
{
    //QRectF bds = boundsForNodes(nds);
    // QPointF ctr = bds.center();
    // ctr.setX((float)floor(ctr.x() * 4.0f) / 4.0f);
    // ctr.setY((float)floor(ctr.y() * 4.0f) / 4.0f);
    float sign = (clockwise) ? 1.0f : -1.0f;

    QPointF p;
    // float dx, dy;
    foreach(Node *n, nds) {
        p = n->point();
        // dx = p.x() - ctr.x();
        // dy = p.y() - ctr.y();
        n->setPoint(QPointF(sign * p.y(), -sign * p.x()));
    }

    int newIn, newOut;
    foreach (Edge *e, _edges) {
        if (nds.contains(e->source()) && nds.contains(e->target())) {
            // update angles if necessary. Note that "basic" bends are computed based
            // on node position, so they don't need to be updated.
            if (!e->basicBendMode()) {
                newIn = e->inAngle() - sign * 90;
                newOut = e->outAngle() - sign * 90;

                // normalise the angle to be within (-180,180]
                if (newIn > 180) newIn -= 360;
                else if (newIn <= -180) newIn += 360;
                if (newOut > 180) newOut -= 360;
                else if (newOut <= -180) newOut += 360;
                e->setInAngle(newIn);
                e->setOutAngle(newOut);
            }
        }
    }
}

void Graph::setBbox(const QRectF &bbox)
{
    _bbox = bbox;
}
