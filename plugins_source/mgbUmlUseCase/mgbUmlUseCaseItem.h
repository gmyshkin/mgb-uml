#ifndef MGB_UML_USECASE_ITEM_H
#define MGB_UML_USECASE_ITEM_H

#include "../../src/gui/nodeitem.h"

class UseCaseNodeItem : public NodeItem {
public:
    UseCaseNodeItem(Node *node) : NodeItem(node) {}
    QRectF labelRect() const override;
    QPainterPath shape() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
};

#endif