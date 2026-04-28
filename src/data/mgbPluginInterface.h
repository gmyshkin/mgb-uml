#ifndef MGBPLUGININTERFACE_H
#define MGBPLUGININTERFACE_H

#include <QtPlugin>
#include "mgbPluginManager.h" // Gets the PluginElement struct

class Node;
class NodeItem;
class QTextStream;

namespace mgb {

class ElementPluginInterface {
public:
    virtual ~ElementPluginInterface() = default;

    // The name of the compiled plugin pack
    virtual QString pluginName() const = 0;

    // The compiled plugin hands the app a list of its elements for the menu
    virtual QList<PluginElement> getElements() const = 0;

    // THE MAGIC: Let the C++ plugin return a custom drawing class!
    // If the plugin doesn't want to draw this specific node, it returns nullptr.
    virtual NodeItem* createCustomNode(Node *node) const = 0;

    // Let plugins emit TikZ that matches their custom Qt drawing. Return true
    // when the plugin handled the node; emittedLines should include every line
    // written so source line tracking remains useful.
    virtual bool writeTikzNode(QTextStream &, Node *, int *emittedLines) const {
        if (emittedLines) *emittedLines = 0;
        return false;
    }

    virtual QIcon pluginIcon() const = 0;
};

} // namespace mgb

QT_BEGIN_NAMESPACE
#define ElementPluginInterface_iid "com.mgb.Uml.ElementPluginInterface"
Q_DECLARE_INTERFACE(mgb::ElementPluginInterface, ElementPluginInterface_iid)
QT_END_NAMESPACE

#endif // MGBPLUGININTERFACE_H
