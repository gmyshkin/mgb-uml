#include "mgbUmlClassPlugin.h"

namespace mgb {

QString UmlClassPlugin::pluginName() const { 
    return "UML Class Element"; 
}

QList<PluginElement> UmlClassPlugin::getElements() const {
    PluginElement e;
    e.name = "UML Class";
    e.type = "node";
    e.category = "UML Elements";
    e.tooltip = "Drag to add a Class";
    e.iconPath = ":/icons/class_icon.png";
    
    e.properties.insert("shape", "rectangle split");
    e.properties.insert("rectangle split parts", "3");
    e.properties.insert("draw", "black");
    e.properties.insert("fill", "white");
    e.properties.insert("align", "left");
    e.properties.insert("default_label",
    "Class \\nodepart{two} + attr \\nodepart{three} + method()");
    e.properties.insert("minimum width", "3cm");
    e.properties.insert("minimum height", "2cm");
    
    // Guarantee that LaTeX includes the multipart library
    e.properties.insert("tikz_libraries", "shapes.multipart");
    
    return {e};
}

QIcon UmlClassPlugin::pluginIcon() const {
    return QIcon(":/icons/class_icon.png");
}

NodeItem* UmlClassPlugin::createCustomNode(Node *node) const {
    if (node->style()->shape() == "rectangle split" || node->styleName() == "UML Class") {
        return new ::ClassNodeItem(node);
    }
    return nullptr; 
}

} // namespace mgb
