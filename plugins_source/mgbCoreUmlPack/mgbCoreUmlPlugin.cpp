#include "mgbCoreUmlPlugin.h"

namespace mgb {

QString CoreUmlPlugin::pluginName() const {
    return "MGB Standard UML Pack";
}

QList<PluginElement> CoreUmlPlugin::getElements() const {
    QList<PluginElement> elements;
    
    // 1. Register the Use Case
    PluginElement useCase;
    useCase.name = "UML Use Case";
    useCase.type = "node";
    useCase.category = "UML Basic";
    useCase.tooltip = "A standard UML Use Case oval";
    useCase.properties.insert("shape", "ellipse"); // Tells the factory how to route it
    
    // 2. Register the Class
    PluginElement umlClass;
    umlClass.name = "UML Class";
    umlClass.type = "node";
    umlClass.category = "UML Basic";
    umlClass.tooltip = "A multi-part UML Class node";
    umlClass.properties.insert("shape", "rectangle split"); // Tells the factory how to route it
    
    elements.append(useCase);
    elements.append(umlClass);
    return elements;
}

NodeItem* CoreUmlPlugin::createCustomNode(Node *node) const {
    QString shape = node->style()->shape();
    QString styleName = node->styleName(); // THE FIX: Grab the dropped name
    
    // Check both! If TikZiT loses the shape property, the name will catch it.
    if (shape == "ellipse" || styleName == "UML Use Case") {
        return new ::UseCaseNodeItem(node);
    } 
    else if (shape == "rectangle split" || styleName == "UML Class") {
        return new ::ClassNodeItem(node);
    }
    
    return nullptr; 
}

} // namespace mgb