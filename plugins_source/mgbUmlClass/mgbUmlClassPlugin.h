#ifndef MGB_UML_CLASS_PLUGIN_H
#define MGB_UML_CLASS_PLUGIN_H

#include <QObject>
#include "../../src/data/mgbPluginInterface.h"
#include "mgbUmlClassItem.h"

namespace mgb {
class UmlClassPlugin : public QObject, public ElementPluginInterface {
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "com.mgb.Uml.ElementPluginInterface" FILE "mgbUmlClass.json")
    Q_INTERFACES(mgb::ElementPluginInterface)

public:
    QString pluginName() const override;
    QList<PluginElement> getElements() const override;
    NodeItem* createCustomNode(Node *node) const override;
    bool writeTikzNode(QTextStream &code, Node *node, int *emittedLines) const override;
    QIcon pluginIcon() const override;
};
}
#endif
