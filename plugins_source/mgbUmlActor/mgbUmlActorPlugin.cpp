#include "mgbUmlActorPlugin.h"

namespace mgb {

QString UmlActorPlugin::pluginName() const { 
    return "UML Actor Element"; 
}

QList<PluginElement> UmlActorPlugin::getElements() const {
    QList<PluginElement> elements;
    PluginElement e;
    e.name = "UML Actor";
    e.type = "node";
    e.category = "UML Basics";
    e.tooltip = "A standard UML stick figure actor";
    e.properties.insert("shape", "uml actor");
    elements.append(e);
    return elements;
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