#include "mgbUmlPalette.h"
#include <QVBoxLayout>

namespace mgb {

UmlPalette::UmlPalette(QWidget *parent) : QDockWidget("UML Elements", parent) {
    listWidget = new QListWidget(this);
    
    // Set up the visual look of the list
    listWidget->setViewMode(QListView::IconMode);
    listWidget->setIconSize(QSize(48, 48));
    listWidget->setMovement(QListView::Static);
    listWidget->setResizeMode(QListView::Adjust);
    listWidget->setSpacing(10);
    
    // Critical: Enable items to be dragged out of the widget
    listWidget->setDragEnabled(true); 
    listWidget->setDropIndicatorShown(false);

    // Add Use Case
    QListWidgetItem *useCaseItem = new QListWidgetItem(QIcon(":/images/tikzit-tool-node.svg"), "UML Use Case");
    useCaseItem->setToolTip("Drag to add a Use Case");
    listWidget->addItem(useCaseItem);

    // Add Class
    QListWidgetItem *classItem = new QListWidgetItem(QIcon(":/images/tikzit-tool-node.svg"), "UML Class");
    classItem->setToolTip("Drag to add a Class");
    listWidget->addItem(classItem);

    setWidget(listWidget);
}

} // namespace mgb