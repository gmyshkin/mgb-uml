#ifndef MGB_UML_SYSTEM_PLUGIN_H
#define MGB_UML_SYSTEM_PLUGIN_H

#include <QObject>
#include "../../src/data/mgbPluginInterface.h"
#include "mgbUmlSystemItem.h"

namespace mgb {
class UmlSystemPlugin : public QObject, public ElementPluginInterface {
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "com.mgb.Uml.ElementPluginInterface" FILE "mgbUmlSystem.json")
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
