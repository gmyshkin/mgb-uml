#ifndef MGB_UML_ACTOR_ITEM_H
#define MGB_UML_ACTOR_ITEM_H

#include "../../src/gui/nodeitem.h"

class ActorNodeItem : public NodeItem {
public:
    ActorNodeItem(Node *node) : NodeItem(node) {}
    QRectF labelRect() const override;
    QPainterPath shape() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
};

#endif