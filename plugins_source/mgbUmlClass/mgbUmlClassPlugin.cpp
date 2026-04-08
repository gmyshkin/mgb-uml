#include "mgbUmlClassPlugin.h"

namespace mgb {

QString UmlClassPlugin::pluginName() const { 
    return "UML Class Element"; 
}

QList<PluginElement> UmlClassPlugin::getElements() const {
    QList<PluginElement> elements;
    PluginElement e;
    e.name = "UML Class";
    e.type = "node";
    e.category = "UML Basics";
    e.tooltip = "A multi-part UML Class node";
    e.properties.insert("shape", "rectangle split");
    elements.append(e);
    return elements;
}

NodeItem* UmlClassPlugin::createCustomNode(Node *node) const {
    if (node->style()->shape() == "rectangle split" || node->styleName() == "UML Class") {
        return new ::ClassNodeItem(node);
    }
    return nullptr; 
}

} // namespace mgb