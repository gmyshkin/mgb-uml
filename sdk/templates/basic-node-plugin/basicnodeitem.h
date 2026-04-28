#ifndef BASICNODEITEM_H
#define BASICNODEITEM_H

#include "src/gui/nodeitem.h"

class BasicNodeItem : public NodeItem {
public:
    explicit BasicNodeItem(Node *node) : NodeItem(node) {}

    QRectF labelRect() const override;
    QPainterPath shape() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
};

#endif
