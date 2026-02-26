/*
    TikZiT - a GUI diagram editor for TikZ
    Copyright (C) 2018 Aleks Kissinger
    ...
*/

#ifndef TIKZVIEW_H
#define TIKZVIEW_H

#include <QObject>
#include <QWidget>
#include <QGraphicsView>
#include <QPainter>
#include <QGraphicsItem>
#include <QStyleOptionGraphicsItem>
#include <QRectF>
#include <QMouseEvent>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMimeData>

class TikzView : public QGraphicsView
{
    Q_OBJECT
public:
    explicit TikzView(QWidget *parent = 0);

public slots:
    void zoomIn();
    void zoomOut();
    void setScene(QGraphicsScene *scene);
protected:
    void drawBackground(QPainter *painter, const QRectF &rect) override;
    void wheelEvent(QWheelEvent *event) override;
    
    // --- MGB-UML: Enabling Drag and Drop ---
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dragMoveEvent(QDragMoveEvent *event) override;
    void dropEvent(QDropEvent *event) override;
    
private:
    float _scale;
};

#endif // TIKZVIEW_H