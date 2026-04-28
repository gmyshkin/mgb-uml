#ifndef MGB_UML_SYSTEM_ITEM_H
#define MGB_UML_SYSTEM_ITEM_H

#include "../../src/gui/nodeitem.h"

class SystemNodeItem : public NodeItem {
public:
    SystemNodeItem(Node *node) : NodeItem(node) {}
    QRectF labelRect() const override;
    QPainterPath shape() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
};

#endif