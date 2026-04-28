#ifndef UMLNODEITEMS_H
#define UMLNODEITEMS_H

#include "nodeitem.h"

// =================================================
// USE CASE SUBCLASS
// =================================================
class UseCaseNodeItem : public NodeItem {
public:
    UseCaseNodeItem(Node *node) : NodeItem(node) {}
    
    QRectF labelRect() const override;
    QPainterPath shape() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
};

// =================================================
// CLASS SUBCLASS
// =================================================
class ClassNodeItem : public NodeItem {
public:
    ClassNodeItem(Node *node) : NodeItem(node) {}
    
    QRectF labelRect() const override;
    QPainterPath shape() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
};

#endif // UMLNODEITEMS_H