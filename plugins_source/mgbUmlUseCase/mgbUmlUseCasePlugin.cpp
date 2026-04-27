#include "mgbUmlUseCasePlugin.h"
#include "../../src/data/node.h"
#include "../../src/data/style.h"
#include "../../src/util.h"

#include <QFont>
#include <QFontMetrics>
#include <QTextStream>
#include <algorithm>

namespace {

static constexpr qreal UML_SCALEF = 40.0;
const QFont MGB_LABEL_FONT("Helvetica", 10);

QString propertyWithStyleDefault(Node *node, const QString &key, const QString &fallback)
{
    QString value = node->data()->property(key);
    if (value.isEmpty() && node->style() && node->style()->data()) {
        value = node->style()->data()->property(key);
    }
    return value.isEmpty() ? fallback : value;
}

int umlLengthToPixels(QString raw, int fallbackPx)
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

QString cm(qreal value)
{
    return floatToString(value) + "cm";
}

}

namespace mgb {

QString UmlUseCasePlugin::pluginName() const { 
    return "UML Use Case Element"; 
}

QList<PluginElement> UmlUseCasePlugin::getElements() const {
    PluginElement e;
    e.name = "UML Use Case";
    e.type = "node";
    e.category = "UML Elements";
    e.tooltip = "Drag to add a Use Case";
    e.iconPath = ":/icons/use_case_icon.png";
    
    e.properties.insert("shape", "ellipse");
    e.properties.insert("draw", "black");
    e.properties.insert("fill", "white");
    e.properties.insert("font", "\\sffamily\\fontsize{10pt}{12pt}\\selectfont");
    e.properties.insert("line width", "0.6pt");
    e.properties.insert("inner sep", "0pt");
    e.properties.insert("outer sep", "0pt");
    e.properties.insert("align", "center");
    e.properties.insert("minimum width", "3cm");
    e.properties.insert("minimum height", "1.5cm");
    
    // Guarantee that LaTeX includes the geometric library
    e.properties.insert("tikz_libraries", "shapes.geometric");
    
    return {e};
}

QIcon UmlUseCasePlugin::pluginIcon() const {
    return QIcon(":/icons/use_case_icon.png");
}

NodeItem* UmlUseCasePlugin::createCustomNode(Node *node) const {
    if (node->style()->shape() == "ellipse" || node->styleName() == "UML Use Case") {
        return new ::UseCaseNodeItem(node);
    }
    return nullptr; 
}

bool UmlUseCasePlugin::writeTikzNode(QTextStream &code, Node *node, int *emittedLines) const {
    if (!(node->styleName() == "UML Use Case")) {
        if (emittedLines) *emittedLines = 0;
        return false;
    }

    QString label = replaceTexConstants(node->label());
    QFontMetrics fm(MGB_LABEL_FONT);
    QRect b = fm.boundingRect(label);

    int minW = umlLengthToPixels(propertyWithStyleDefault(node, "minimum width", "3cm"), 120);
    int minH = umlLengthToPixels(propertyWithStyleDefault(node, "minimum height", "1.5cm"), 60);

    qreal xRadius = std::max(std::max(minW / 2.0, b.width() / 2.0 + 15.0) / UML_SCALEF, 0.5);
    qreal yRadius = std::max(std::max(minH / 2.0, b.height() / 2.0 + 10.0) / UML_SCALEF, 0.25);
    qreal width = xRadius * 2.0;
    qreal height = yRadius * 2.0;

    QString x = floatToString(node->point().x());
    QString y = floatToString(node->point().y());
    QString draw = propertyWithStyleDefault(node, "draw", "black");
    QString fill = propertyWithStyleDefault(node, "fill", "white");
    QString lineWidth = propertyWithStyleDefault(node, "line width", "0.6pt");
    QString font = propertyWithStyleDefault(node, "font", "\\sffamily\\fontsize{10pt}{12pt}\\selectfont");

    code << "\t\t\\node [draw=none, fill=none, shape=ellipse, minimum width=" << cm(width)
         << ", minimum height=" << cm(height)
         << ", inner sep=0pt, outer sep=0pt] (" << node->name()
         << ") at (" << x << ", " << y << ") {};\n";
    code << "\t\t\\path [draw=" << draw << ", fill=" << fill
         << ", line width=" << lineWidth << "] (" << x << ", " << y
         << ") ellipse [x radius=" << cm(xRadius)
         << ", y radius=" << cm(yRadius) << "];\n";
    code << "\t\t\\node [align=center, inner sep=0pt, outer sep=0pt, font={" << font
         << "}] at (" << x << ", " << y << ") {" << node->label() << "};\n";

    if (emittedLines) *emittedLines = 3;
    return true;
}

} // namespace mgb
