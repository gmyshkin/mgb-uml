#include "mgbUmlSystemPlugin.h"
#include "../../src/data/node.h"
#include "../../src/data/style.h"
#include "../../src/util.h"

#include <QTextStream>

namespace {

QString propertyWithStyleDefault(Node *node, const QString &key, const QString &fallback)
{
    QString value = node->data()->property(key);
    if (value.isEmpty() && node->style() && node->style()->data()) {
        value = node->style()->data()->property(key);
    }
    return value.isEmpty() ? fallback : value;
}

QString nodeDrawOptions(Node *node)
{
    QStringList options;
    options << "draw=" + propertyWithStyleDefault(node, "draw", "black");
    options << "fill=" + propertyWithStyleDefault(node, "fill", "white");
    options << "line width=" + propertyWithStyleDefault(node, "line width", "0.6pt");
    options << "minimum width=" + propertyWithStyleDefault(node, "minimum width", "4cm");
    options << "minimum height=" + propertyWithStyleDefault(node, "minimum height", "6cm");
    options << "inner sep=0pt";
    options << "outer sep=0pt";
    options << "shape=rectangle";
    return "[" + options.join(", ") + "]";
}

}

namespace mgb {

QString UmlSystemPlugin::pluginName() const { 
    return "UML System Element"; 
}

QList<PluginElement> UmlSystemPlugin::getElements() const {
    PluginElement e;
    e.name = "UML System";
    e.type = "node";
    e.category = "UML Elements";
    e.tooltip = "Drag to add a System boundary";
    e.iconPath = ":/icons/system_icon.png";
    
    e.properties.insert("shape", "uml system");
    e.properties.insert("tikzit edge shape", "rectangle");
    e.properties.insert("tikzit edge width", "5.0cm");
    e.properties.insert("tikzit edge height", "6.0cm");
    e.properties.insert("draw", "black");
    e.properties.insert("fill", "white");
    e.properties.insert("minimum width", "4cm");
    e.properties.insert("minimum height", "6cm");
    e.properties.insert("align", "left");
    e.properties.insert("font", "\\bfseries\\sffamily\\fontsize{10pt}{12pt}\\selectfont");
    e.properties.insert("line width", "0.6pt");
    e.properties.insert("inner sep", "0pt");
    e.properties.insert("outer sep", "0pt");

    e.properties.insert("tikz_libraries", "");
    e.properties.insert("latex_preamble",
        "\\makeatletter\n"
        "\\pgfdeclareshape{uml system}{\n"
        "  \\inheritsavedanchors[from=rectangle]\n"
        "  \\inheritanchorborder[from=rectangle]\n"
        "  \\inheritanchor[from=rectangle]{center}\n"
        "  \\inheritanchor[from=rectangle]{north}\n"
        "  \\inheritanchor[from=rectangle]{north east}\n"
        "  \\inheritanchor[from=rectangle]{east}\n"
        "  \\inheritanchor[from=rectangle]{south east}\n"
        "  \\inheritanchor[from=rectangle]{south}\n"
        "  \\inheritanchor[from=rectangle]{south west}\n"
        "  \\inheritanchor[from=rectangle]{west}\n"
        "  \\inheritanchor[from=rectangle]{north west}\n"
        "  \\inheritbackgroundpath[from=rectangle]\n"
        "  \\anchor{text}{\n"
        "    \\pgf@process{\\northwest}\n"
        "    \\advance\\pgf@x by 0.25cm\n"
        "    \\advance\\pgf@y by -0.15cm\n"
        "  }\n"
        "}\n"
        "\\makeatother");
    
    return {e};
}

QIcon UmlSystemPlugin::pluginIcon() const {
    return QIcon(":/icons/system_icon.png");
}

NodeItem* UmlSystemPlugin::createCustomNode(Node *node) const {
    if (node->style()->shape() == "rectangle" ||
        node->style()->shape() == "uml system" ||
        node->styleName() == "UML System") {
        return new ::SystemNodeItem(node);
    }
    return nullptr; 
}

bool UmlSystemPlugin::writeTikzNode(QTextStream &code, Node *node, int *emittedLines) const {
    if (!(node->styleName() == "UML System" || node->style()->shape() == "uml system")) {
        if (emittedLines) *emittedLines = 0;
        return false;
    }

    QString x = floatToString(node->point().x());
    QString y = floatToString(node->point().y());
    QString font = propertyWithStyleDefault(node, "font", "\\bfseries\\sffamily\\fontsize{10pt}{12pt}\\selectfont");

    code << "\t\t\\node " << nodeDrawOptions(node)
         << " (" << node->name() << ") at (" << x << ", " << y << ") {};\n";
    code << "\t\t\\node [anchor=north west, align=left, inner sep=0pt, outer sep=0pt, font={"
         << font << "}] at ([xshift=0.25cm,yshift=-0.15cm]" << node->name()
         << ".north west) {" << node->label() << "};\n";

    if (emittedLines) *emittedLines = 2;
    return true;
}

} // namespace mgb
