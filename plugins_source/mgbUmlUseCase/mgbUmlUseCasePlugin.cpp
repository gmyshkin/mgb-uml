#include "mgbUmlUseCasePlugin.h"

namespace mgb {

QString UmlUseCasePlugin::pluginName() const { 
    return "UML Use Case Element"; 
}

QList<PluginElement> UmlUseCasePlugin::getElements() const {
    QList<PluginElement> elements;
    PluginElement e;
    e.name = "UML Use Case";
    e.type = "node";
    e.category = "UML Basics";
    e.tooltip = "A standard UML Use Case oval";
    e.properties.insert("shape", "ellipse");
    elements.append(e);
    return elements;
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

} // namespace mgb