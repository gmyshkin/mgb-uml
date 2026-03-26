#include "mgbUmlPalette.h"
#include <QVBoxLayout>
#include <QFile>
#include <QMimeData>
#include "../data/mgbPluginManager.h"

namespace mgb {

// =================================================================
// MGB-UML: CUSTOM DRAG-AND-DROP LIST
// Forces Qt to pass clean, uncorrupted plain text to the Canvas
// =================================================================
class PaletteListWidget : public QListWidget {
public:
    PaletteListWidget(QWidget *parent = nullptr) : QListWidget(parent) {}
protected:
    // ADDED THE MISSING '&' HERE:
    QMimeData* mimeData(const QList<QListWidgetItem *> &items) const override {
        QMimeData *mime = QListWidget::mimeData(items);
        if (!items.isEmpty()) {
            // Force the exact item name into the drag payload
            mime->setText(items.first()->text()); 
        }
        return mime;
    }
};
// =================================================================


UmlPalette::UmlPalette(QWidget *parent) : QDockWidget("UML Elements", parent) {
    // USE OUR NEW CUSTOM LIST WIDGET INSTEAD OF THE DEFAULT ONE
    listWidget = new PaletteListWidget(this);
    
    listWidget->setViewMode(QListView::IconMode);
    listWidget->setIconSize(QSize(48, 48));
    listWidget->setMovement(QListView::Static);
    listWidget->setResizeMode(QListView::Adjust);
    listWidget->setSpacing(10);
    
    listWidget->setDragEnabled(true); 
    listWidget->setDropIndicatorShown(false);

    // --- 1. ADD BASELINE HARDCODED ITEMS ---
    QListWidgetItem *useCaseItem = new QListWidgetItem(QIcon(":/images/tikzit-tool-node.svg"), "UML Use Case");
    useCaseItem->setToolTip("Drag to add a Use Case");
    listWidget->addItem(useCaseItem);

    QListWidgetItem *classItem = new QListWidgetItem(QIcon(":/images/tikzit-tool-node.svg"), "UML Class");
    classItem->setToolTip("Drag to add a Class");
    listWidget->addItem(classItem);

    // --- 2. ADD USER PLUGINS (Both JSON and C++) ---
    QList<PluginElement> plugins = PluginManager::instance().getLoadedPlugins();
    for (const PluginElement& p : plugins) {
        if (p.type == "node") {
            QIcon pluginIcon;
            if (QFile::exists(p.iconPath)) {
                pluginIcon = QIcon(p.iconPath);
            } else {
                pluginIcon = QIcon(":/images/tikzit-tool-node.svg"); 
            }
            
            QListWidgetItem *item = new QListWidgetItem(pluginIcon, p.name);
            item->setToolTip(p.tooltip);
            listWidget->addItem(item);
        }
    }

    setWidget(listWidget);
}

} // namespace mgb