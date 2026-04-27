#include "mgbUmlActorPlugin.h"
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

}

namespace mgb {

QString UmlActorPlugin::pluginName() const { 
    return "UML Actor Element"; 
}

QList<PluginElement> UmlActorPlugin::getElements() const {
    PluginElement e;
    e.name = "UML Actor";
    e.type = "node";
    e.category = "UML Elements";
    e.tooltip = "Drag to add an Actor";
    e.iconPath = ":/icons/actor_icon.png";
    
    // The standard properties
    e.properties.insert("shape", "uml_actor");
    e.properties.insert("draw", "black");
    e.properties.insert("fill", "none");
    e.properties.insert("align", "center");
    e.properties.insert("font", "\\sffamily\\fontsize{10pt}{12pt}\\selectfont");
    e.properties.insert("line width", "0.6pt");
    e.properties.insert("inner sep", "0pt");
    e.properties.insert("outer sep", "0pt");
    
    // The Trojan Horse: Teaching LaTeX
    e.properties.insert("tikz_libraries", "arrows.meta");
    e.properties.insert("latex_preamble", 
        "\\pgfdeclareshape{uml_actor}{\n"
        "  \\savedanchor\\centerpoint{\\pgfpointorigin}\n"
        "  \\anchor{center}{\\centerpoint}\n"
        "  \\anchor{text}{\\pgfpoint{0cm}{-1.25cm}}\n"
        "  \\backgroundpath{\n"
        "    \\pgfpathcircle{\\pgfpoint{0cm}{0.5cm}}{0.2cm}\n"
        "    \\pgfpathmoveto{\\pgfpoint{0cm}{0.3cm}}\n"
        "    \\pgfpathlineto{\\pgfpoint{0cm}{-0.4cm}}\n"
        "    \\pgfpathmoveto{\\pgfpoint{-0.4cm}{0.1cm}}\n"
        "    \\pgfpathlineto{\\pgfpoint{0.4cm}{0.1cm}}\n"
        "    \\pgfpathmoveto{\\pgfpoint{-0.3cm}{-0.9cm}}\n"
        "    \\pgfpathlineto{\\pgfpoint{0cm}{-0.4cm}}\n"
        "    \\pgfpathlineto{\\pgfpoint{0.3cm}{-0.9cm}}\n"
        "  }\n"
        "}"
    );
    
    return {e};
}

QIcon UmlActorPlugin::pluginIcon() const {
    return QIcon(":/icons/actor_icon.png");
}

NodeItem* UmlActorPlugin::createCustomNode(Node *node) const {
    if (node->style()->shape() == "uml actor" ||
        node->style()->shape() == "uml_actor" ||
        node->styleName() == "UML Actor") {
        return new ::ActorNodeItem(node);
    }
    return nullptr; 
}

bool UmlActorPlugin::writeTikzNode(QTextStream &code, Node *node, int *emittedLines) const {
    if (!(node->styleName() == "UML Actor" || node->style()->shape() == "uml_actor")) {
        if (emittedLines) *emittedLines = 0;
        return false;
    }

    QString x = floatToString(node->point().x());
    QString y = floatToString(node->point().y());
    QString stroke = propertyWithStyleDefault(node, "draw", "black");
    QString lineWidth = propertyWithStyleDefault(node, "line width", "0.6pt");
    QString font = propertyWithStyleDefault(node, "font", "\\sffamily\\fontsize{10pt}{12pt}\\selectfont");
    QString draw = "draw=" + stroke + ", line width=" + lineWidth;

    code << "\t\t\\coordinate (" << node->name() << ") at (" << x << ", " << y << ");\n";
    code << "\t\t\\draw [" << draw << "] (" << node->name()
         << ") ++(0,0.45) circle [radius=0.12cm];\n";
    code << "\t\t\\draw [" << draw << "] (" << node->name()
         << ") ++(0,0.33) -- ++(0,-0.53);\n";
    code << "\t\t\\draw [" << draw << "] (" << node->name()
         << ") ++(-0.22,0.05) -- ++(0.44,0);\n";
    code << "\t\t\\draw [" << draw << "] (" << node->name()
         << ") ++(0,-0.20) -- ++(-0.20,-0.35);\n";
    code << "\t\t\\draw [" << draw << "] (" << node->name()
         << ") ++(0,-0.20) -- ++(0.20,-0.35);\n";
    code << "\t\t\\node [align=center, anchor=north, inner sep=0pt, outer sep=0pt, font={"
         << font << "}] at ([yshift=-0.80cm]" << node->name() << ") {"
         << node->label() << "};\n";

    if (emittedLines) *emittedLines = 7;
    return true;
}

} // namespace mgb
