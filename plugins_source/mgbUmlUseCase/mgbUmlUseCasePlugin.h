#ifndef MGB_UML_USECASE_PLUGIN_H
#define MGB_UML_USECASE_PLUGIN_H

#include <QObject>
#include "../../src/data/mgbPluginInterface.h"
#include "mgbUmlUseCaseItem.h"

namespace mgb {
class UmlUseCasePlugin : public QObject, public ElementPluginInterface {
    Q_OBJECT
    Q_PLUGIN_METADATA(IID ElementPluginInterface_iid)
    Q_INTERFACES(mgb::ElementPluginInterface)

public:
    QString pluginName() const override;
    QList<PluginElement> getElements() const override;
    NodeItem* createCustomNode(Node *node) const override;
};
}
#endif