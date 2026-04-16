#include "mgbUmlUseCasePlugin.h"

namespace mgb {

QString UmlUseCasePlugin::pluginName() const { 
    return "UML Use Case Element"; 
}

QList<PluginElement> UmlUseCasePlugin::getElements() const {
    PluginElement e;
    e.name = "UML Use Case";
    e.type = "node";
    e.category = "UML Elements";
    e.tooltip = "Drag to add a Use Case";
    e.iconPath = ":/icons/use_case_icon.png";
    
    e.properties.insert("shape", "ellipse");
    e.properties.insert("draw", "black");
    e.properties.insert("fill", "white");
    e.properties.insert("minimum width", "3cm");
    e.properties.insert("minimum height", "1.5cm");
    
    // Guarantee that LaTeX includes the geometric library
    e.properties.insert("tikz_libraries", "shapes.geometric");
    
    return {e};
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