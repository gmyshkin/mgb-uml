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
    
    e.properties.insert("shape", "rectangle");
    e.properties.insert("draw", "black");
    e.properties.insert("fill", "white");
    e.properties.insert("minimum width", "5cm");
    e.properties.insert("minimum height", "7cm");
    e.properties.insert("align", "center");
    
    // Standard shape, no extra libraries needed
    e.properties.insert("tikz_libraries", "");
    
    return {e};
}

QIcon UmlSystemPlugin::pluginIcon() const {
    return QIcon(":/icons/system_icon.png");
}

NodeItem* UmlSystemPlugin::createCustomNode(Node *node) const {
    if (node->style()->shape() == "uml system" || node->styleName() == "UML System") {
        return new ::SystemNodeItem(node);
    }
    return nullptr; 
}

} // namespace mgb