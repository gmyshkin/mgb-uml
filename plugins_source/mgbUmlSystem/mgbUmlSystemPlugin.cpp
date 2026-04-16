#include "mgbUmlSystemPlugin.h"

namespace mgb {

QString UmlSystemPlugin::pluginName() const { 
    return "UML System Element"; 
}

QList<PluginElement> UmlSystemPlugin::getElements() const {
    QList<PluginElement> elements;
    PluginElement e;
    e.name = "UML System";
    e.type = "node";
    e.category = "UML Basics";
    e.tooltip = "A UML System Boundary box";
    e.properties.insert("shape", "uml system");
    elements.append(e);
    return elements;
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