#include "mgbUmlClassPlugin.h"
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
    options << "minimum width=" + propertyWithStyleDefault(node, "minimum width", "3cm");
    options << "minimum height=" + propertyWithStyleDefault(node, "minimum height", "2cm");
    options << "inner sep=0pt";
    options << "outer sep=0pt";
    options << "shape=rectangle";
    return "[" + options.join(", ") + "]";
}

QStringList umlClassParts(const QString &label)
{
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

    return {part1, part2, part3};
}

}

namespace mgb {

QString UmlClassPlugin::pluginName() const { 
    return "UML Class Element"; 
}

QList<PluginElement> UmlClassPlugin::getElements() const {
    PluginElement e;
    e.name = "UML Class";
    e.type = "node";
    e.category = "UML Elements";
    e.tooltip = "Drag to add a Class";
    e.iconPath = ":/icons/class_icon.png";
    
    e.properties.insert("shape", "rectangle split");
    e.properties.insert("rectangle split parts", "3");
    e.properties.insert("draw", "black");
    e.properties.insert("fill", "white");
    e.properties.insert("align", "left");
    e.properties.insert("font", "\\sffamily\\fontsize{10pt}{12pt}\\selectfont");
    e.properties.insert("line width", "0.6pt");
    e.properties.insert("inner sep", "0pt");
    e.properties.insert("outer sep", "0pt");
    e.properties.insert("rectangle split part align", "{center,left,left}");
    e.properties.insert("default_label",
    "Class \\nodepart{two} + attr \\nodepart{three} + method()");
    e.properties.insert("minimum width", "3cm");
    e.properties.insert("minimum height", "2cm");
    
    // Guarantee that LaTeX includes the multipart library
    e.properties.insert("tikz_libraries", "shapes.multipart");
    
    return {e};
}

QIcon UmlClassPlugin::pluginIcon() const {
    return QIcon(":/icons/class_icon.png");
}

NodeItem* UmlClassPlugin::createCustomNode(Node *node) const {
    if (node->style()->shape() == "rectangle split" || node->styleName() == "UML Class") {
        return new ::ClassNodeItem(node);
    }
    return nullptr;
}

bool UmlClassPlugin::writeTikzNode(QTextStream &code, Node *node, int *emittedLines) const {
    if (node->styleName() != "UML Class") {
        if (emittedLines) *emittedLines = 0;
        return false;
    }

    QStringList parts = umlClassParts(node->label());
    QString x = floatToString(node->point().x());
    QString y = floatToString(node->point().y());
    QString stroke = propertyWithStyleDefault(node, "draw", "black");
    QString lineWidth = propertyWithStyleDefault(node, "line width", "0.6pt");
    QString font = propertyWithStyleDefault(node, "font", "\\sffamily\\fontsize{10pt}{12pt}\\selectfont");

    code << "\t\t\\node " << nodeDrawOptions(node)
         << " (" << node->name() << ") at (" << x << ", " << y << ") {};\n";
    code << "\t\t\\draw [draw=" << stroke << ", line width=" << lineWidth << "] "
         << "([yshift=-0.60cm]" << node->name() << ".north west) -- "
         << "([yshift=-0.60cm]" << node->name() << ".north east);\n";
    code << "\t\t\\draw [draw=" << stroke << ", line width=" << lineWidth << "] "
         << "([yshift=-1.10cm]" << node->name() << ".north west) -- "
         << "([yshift=-1.10cm]" << node->name() << ".north east);\n";
    code << "\t\t\\node [anchor=north, align=center, inner sep=0pt, outer sep=0pt, font={\\bfseries"
         << font << "}] at ([yshift=-0.18cm]" << node->name()
         << ".north) {" << parts.value(0) << "};\n";
    code << "\t\t\\node [anchor=north west, align=left, inner sep=0pt, outer sep=0pt, font={"
         << font << "}] at ([xshift=0.12cm,yshift=-0.72cm]" << node->name()
         << ".north west) {" << parts.value(1) << "};\n";
    code << "\t\t\\node [anchor=north west, align=left, inner sep=0pt, outer sep=0pt, font={"
         << font << "}] at ([xshift=0.12cm,yshift=-1.22cm]" << node->name()
         << ".north west) {" << parts.value(2) << "};\n";

    if (emittedLines) *emittedLines = 6;
    return true;
}

} // namespace mgb
