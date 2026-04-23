#include "mgbUmlActorPlugin.h"

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
    e.properties.insert("font", "\\sffamily\\fontsize{10pt}{12pt}\\selectfont");
    e.properties.insert("line width", "0.6pt");
    
    // The Trojan Horse: Teaching LaTeX
    e.properties.insert("tikz_libraries", "arrows.meta");
    e.properties.insert("latex_preamble", 
        "\\pgfdeclareshape{uml_actor}{\n"
        "  \\savedanchor\\centerpoint{\\pgfpointorigin}\n"
        "  \\anchor{center}{\\centerpoint}\n"
        "  \\anchor{text}{\\pgfpoint{-1.5cm}{-1.2cm}}\n" // NEW: Anchors text below the feet
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
    if (node->style()->shape() == "uml actor" || node->styleName() == "UML Actor") {
        return new ::ActorNodeItem(node);
    }
    return nullptr; 
}

} // namespace mgb
