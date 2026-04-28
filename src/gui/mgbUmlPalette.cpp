#include "mgbUmlPalette.h"
#include <QVBoxLayout>
#include <QFile>
#include <QMimeData>
#include <QDebug>
#include <QIcon>
#include "../data/mgbPluginManager.h"
#include "../data/mgbPluginInterface.h"

namespace mgb {

// =================================================================
// MGB-UML: CUSTOM DRAG-AND-DROP LIST
// Forces Qt to pass clean, uncorrupted plain text to the Canvas
// =================================================================
class PaletteListWidget : public QListWidget {
public:
    PaletteListWidget(QWidget *parent = nullptr) : QListWidget(parent) {}
protected:
QMimeData* mimeData(const QList<QListWidgetItem *> &items) const override {
    QMimeData *mime = QListWidget::mimeData(items);

    if (!items.isEmpty()) {
        QListWidgetItem *item = items.first();

        QString styleName = item->data(Qt::UserRole).toString();
        QString defaultLabel = item->data(Qt::UserRole + 1).toString();

        mime->setData("application/x-mgb-style-name", styleName.toUtf8());
        mime->setData("application/x-mgb-default-label", defaultLabel.toUtf8());

        // optional fallback for debugging
        mime->setText(styleName);
    }

    return mime;
}
};
// =================================================================

UmlPalette::UmlPalette(QWidget *parent) : QDockWidget("UML Elements", parent) {
    listWidget = new PaletteListWidget(this);
    
    listWidget->setViewMode(QListView::IconMode);
    listWidget->setIconSize(QSize(64, 64)); // Slightly larger icons look better in dynamic menus
    listWidget->setMovement(QListView::Static);
    listWidget->setResizeMode(QListView::Adjust);
    listWidget->setSpacing(10);
    listWidget->setFocusPolicy(Qt::NoFocus); // Prevents drag-delay bugs
    
    listWidget->setDragEnabled(true); 
    listWidget->setDropIndicatorShown(false);

    // =================================================================
    // MGB-UML: DYNAMIC UI BUILDER
    // Loops through all successfully loaded C++ plugins and extracts 
    // their icon and name directly from the plugin interface.
    // =================================================================
    QList<ElementPluginInterface*> loadedPlugins = PluginManager::instance().getCompiledInterfaces();
    
    for (ElementPluginInterface* plugin : loadedPlugins) {
        
        // Ask the plugin for the specific elements it provides
        QList<PluginElement> elements = plugin->getElements();
        if (elements.isEmpty()) continue;
        
        // Grab the actual element data (e.g., "UML Actor", "UML Class")
        PluginElement elementData = elements.first();
        QIcon pluginIcon = plugin->pluginIcon();
        
        // If the plugin forgot to provide an icon, provide a safe fallback
        if (pluginIcon.isNull()) {
            pluginIcon = QIcon(":/images/tikzit-tool-node.svg"); 
        }
        
        // CRITICAL FIX: We MUST use elementData.name here! 
        // If we use the Plugin Name, the canvas won't know what to draw.
QListWidgetItem *item = new QListWidgetItem(pluginIcon, elementData.name);
item->setToolTip(QString("Drag to add a %1").arg(elementData.name));

// store machine-readable drag payload
item->setData(Qt::UserRole, elementData.name);
item->setData(Qt::UserRole + 1,
              elementData.properties.value("default_label", elementData.name));

listWidget->addItem(item);
    }

    setWidget(listWidget);
}

} // namespace mgb
