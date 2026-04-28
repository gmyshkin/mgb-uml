#include "basicnodeplugin.h"

#include "src/data/node.h"
#include "src/data/style.h"

#include <QTextStream>

namespace {

QString tikzNumber(qreal value)
{
    return QString::number(value, 'g', 12);
}

QString propertyWithStyleDefault(Node *node, const QString &key, const QString &fallback)
{
    QString value = node->data()->property(key);
    if (value.isEmpty() && node->style() && node->style()->data()) {
        value = node->style()->data()->property(key);
    }
    return value.isEmpty() ? fallback : value;
}

}

namespace mgb {

QString BasicNodePlugin::pluginName() const
{
    return "Basic Node Plugin";
}

QList<PluginElement> BasicNodePlugin::getElements() const
{
    PluginElement e;
    e.name = "Basic SDK Node";
    e.type = "node";
    e.category = "SDK Examples";
    e.tooltip = "Drag to add a basic SDK node";
    e.iconPath = ":/icons/basic_node.svg";

    e.properties.insert("shape", "rectangle");
    e.properties.insert("draw", "black");
    e.properties.insert("fill", "white");
    e.properties.insert("font", "\\sffamily\\fontsize{10pt}{12pt}\\selectfont");
    e.properties.insert("line width", "0.6pt");
    e.properties.insert("inner sep", "0pt");
    e.properties.insert("outer sep", "0pt");
    e.properties.insert("minimum width", "3cm");
    e.properties.insert("minimum height", "1.5cm");

    return {e};
}

NodeItem *BasicNodePlugin::createCustomNode(Node *node) const
{
    if (node->styleName() == "Basic SDK Node") {
        return new ::BasicNodeItem(node);
    }
    return nullptr;
}

bool BasicNodePlugin::writeTikzNode(QTextStream &code, Node *node, int *emittedLines) const
{
    if (node->styleName() != "Basic SDK Node") {
        if (emittedLines) *emittedLines = 0;
        return false;
    }

    QString draw = propertyWithStyleDefault(node, "draw", "black");
    QString fill = propertyWithStyleDefault(node, "fill", "white");
    QString lineWidth = propertyWithStyleDefault(node, "line width", "0.6pt");
    QString font = propertyWithStyleDefault(node, "font", "\\sffamily\\fontsize{10pt}{12pt}\\selectfont");

    code << "\t\t\\node [draw=" << draw
         << ", fill=" << fill
         << ", line width=" << lineWidth
         << ", minimum width=3cm, minimum height=1.5cm"
         << ", inner sep=0pt, outer sep=0pt, shape=rectangle"
         << ", align=center, font={" << font << "}] ("
         << node->name() << ") at ("
         << tikzNumber(node->point().x()) << ", "
         << tikzNumber(node->point().y()) << ") {"
         << node->label() << "};\n";

    if (emittedLines) *emittedLines = 1;
    return true;
}

QIcon BasicNodePlugin::pluginIcon() const
{
    return QIcon(":/icons/basic_node.svg");
}

}
