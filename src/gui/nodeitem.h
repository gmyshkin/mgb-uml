/*
    TikZiT - a GUI diagram editor for TikZ
    Copyright (C) 2018 Aleks Kissinger

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

/*!
  * A QGraphicsItem that handles drawing a single node.
  */

#ifndef NODEITEM_H
#define NODEITEM_H

#include <QGraphicsItem>
#include "node.h"

class NodeItem : public QGraphicsItem
{
public:
    NodeItem(Node *node);
    virtual ~NodeItem() = default;

    // The Factory Method
    static NodeItem* createNode(Node *node);

    // Public getters and setters (This fixes your undo command errors!)
    Node *node() const;
    void readPos();
    void writePos();
    void updateBounds();

    QRectF boundingRect() const override;
    QRectF outerLabelRect() const;
    
    // Virtual methods that our subclasses will override
    virtual QRectF labelRect() const;
    virtual QPainterPath shape() const override;
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

protected:
    // Helper function for subclasses
    void paintSelectionAndOuterLabels(QPainter *painter);

    Node *_node;
    QRectF _boundingRect;
};

#endif // NODEITEM_H