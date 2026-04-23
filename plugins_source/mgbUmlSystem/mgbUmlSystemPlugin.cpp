#include "mgbUmlSystemPlugin.h"

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
    e.properties.insert("draw", "black");
    e.properties.insert("fill", "white");
    e.properties.insert("minimum width", "2cm");
    e.properties.insert("minimum height", "2cm");
    e.properties.insert("font", "\\sffamily\\fontsize{10pt}{12pt}\\selectfont");
    e.properties.insert("line width", "0.6pt");

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
        "    \\advance\\pgf@x by 10pt\n"
        "    \\advance\\pgf@y by -16pt\n"
        "  }\n"
        "}\n"
        "\\makeatother");
    
    return {e};
}

QIcon UmlSystemPlugin::pluginIcon() const {
    return QIcon(":/icons/system_icon.png");
}

NodeItem* UmlSystemPlugin::createCustomNode(Node *node) const {
    if (node->style()->shape() == "rectangle" || node->styleName() == "UML System") {
        return new ::SystemNodeItem(node);
    }
    return nullptr; 
}

} // namespace mgb
