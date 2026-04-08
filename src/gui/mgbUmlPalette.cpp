#include "mgbUmlPalette.h"
#include <QVBoxLayout>
#include <QFile>
#include <QMimeData>
#include <QDebug>
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
    listWidget = new PaletteListWidget(this);
    
    listWidget->setViewMode(QListView::IconMode);
    listWidget->setIconSize(QSize(48, 48));
    listWidget->setMovement(QListView::Static);
    listWidget->setResizeMode(QListView::Adjust);
    listWidget->setSpacing(10);
    listWidget->setFocusPolicy(Qt::NoFocus); // Prevents drag-delay bugs
    
    listWidget->setDragEnabled(true); 
    listWidget->setDropIndicatorShown(false);

    // --- ADD USER PLUGINS (Both JSON and C++) ---
    // The hardcoded elements have been completely removed.
    // The Palette relies entirely on the Master Plugin List.
    QList<PluginElement> plugins = PluginManager::instance().getLoadedPlugins();
    for (const PluginElement& p : plugins) {
        
        // The forgiving string check (ignores caps and spaces)
        if (p.type.toLower().trimmed() == "node") {
            QIcon pluginIcon;
            
            // Safe fallback icon check
            if (!p.iconPath.isEmpty() && QFile::exists(p.iconPath)) {
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