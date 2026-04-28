#ifndef MGB_CORE_UML_PLUGIN_H
#define MGB_CORE_UML_PLUGIN_H

#include <QObject>
#include "../../src/data/mgbPluginInterface.h"
#include "mgbUmlNodeItems.h"

namespace mgb {

class CoreUmlPlugin : public QObject, public ElementPluginInterface {
    Q_OBJECT
    Q_PLUGIN_METADATA(IID ElementPluginInterface_iid)
    Q_INTERFACES(mgb::ElementPluginInterface)

public:
    QString pluginName() const override;
    QList<PluginElement> getElements() const override;
    NodeItem* createCustomNode(Node *node) const override;
};

} // namespace mgb

#endif // MGB_CORE_UML_PLUGIN_H