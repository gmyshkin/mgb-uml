/*
    TikZiT - a GUI diagram editor for TikZ
    Copyright (C) 2018 Aleks Kissinger
*/

#include "tikzview.h"
#include "tikzit.h"
#include "tikzscene.h"
#include "undocommands.h"
#include "node.h"

#include <QDebug>
#include <QScrollBar>
#include <QSettings>
#include <QDataStream>

TikzView::TikzView(QWidget *parent) : QGraphicsView(parent)
{
    setRenderHint(QPainter::Antialiasing);
    setResizeAnchor(QGraphicsView::AnchorViewCenter);
    setBackgroundBrush(QBrush(Qt::white));

    setViewportUpdateMode(QGraphicsView::FullViewportUpdate);

    // === THE FAST CURSOR FIX ===
    // Offload CPU calculations so the FullViewportUpdate doesn't drop fast mouse clicks
    setOptimizationFlag(QGraphicsView::DontSavePainterState);
    setOptimizationFlag(QGraphicsView::DontAdjustForAntialiasing);
    
    // --- MGB-UML: Allow things to be dropped here ---
    setAcceptDrops(true);

    _scale = 2.5f;
    scale(2.5, 2.5);
}

void TikzView::zoomIn()
{
    _scale *= 1.6f;
    scale(1.6,1.6);
}

void TikzView::zoomOut()
{
    _scale *= 0.625f;
    scale(0.625,0.625);
}

void TikzView::setScene(QGraphicsScene *scene)
{
    QGraphicsView::setScene(scene);
    centerOn(QPointF(0.0,0.0));
}

void TikzView::drawBackground(QPainter *painter, const QRectF &rect)
{
    QSettings settings("tikzit", "tikzit");
    QGraphicsView::drawBackground(painter, rect);
    TikzScene *sc = static_cast<TikzScene*>(scene());
    if (!sc->enabled()) painter->fillRect(rect, QBrush(QColor(240,240,240)));

    QPen pen1;
    pen1.setCosmetic(true);
    pen1.setColor(settings.value("grid-color-minor", QColor(250,250,255)).value<QColor>());

    QPen pen2 = pen1;
    pen2.setColor(settings.value("grid-color-major", QColor(240,240,250)).value<QColor>());

    QPen pen3 = pen1;
    pen3.setColor(settings.value("grid-color-axes", QColor(220,220,240)).value<QColor>());

    painter->setPen(pen1);

    if (_scale > 0.2f) {
        for (int x = -GRID_SEP; x > rect.left(); x -= GRID_SEP) {
            if (x % (GRID_SEP * GRID_N) != 0) {
                qreal xf = (qreal)x;
                painter->drawLine(xf, rect.top(), xf, rect.bottom());
            }
        }

        for (int x = GRID_SEP; x < rect.right(); x += GRID_SEP) {
            if (x % (GRID_SEP * GRID_N) != 0) {
                qreal xf = (qreal)x;
                painter->drawLine(xf, rect.top(), xf, rect.bottom());
            }
        }

        for (int y = -GRID_SEP; y > rect.top(); y -= GRID_SEP) {
            if (y % (GRID_SEP * GRID_N) != 0) {
                qreal yf = (qreal)y;
                painter->drawLine(rect.left(), yf, rect.right(), yf);
            }
        }

        for (int y = GRID_SEP; y < rect.bottom(); y += GRID_SEP) {
            if (y % (GRID_SEP * GRID_N) != 0) {
                qreal yf = (qreal)y;
                painter->drawLine(rect.left(), yf, rect.right(), yf);
            }
        }
    }

    painter->setPen(pen2);

    for (int x = -GRID_SEP*GRID_N; x > rect.left(); x -= GRID_SEP*GRID_N) {
        qreal xf = (qreal)x;
        painter->drawLine(xf, rect.top(), xf, rect.bottom());
    }

    for (int x = GRID_SEP*GRID_N; x < rect.right(); x += GRID_SEP*GRID_N) {
        qreal xf = (qreal)x;
        painter->drawLine(xf, rect.top(), xf, rect.bottom());
    }

    for (int y = -GRID_SEP*GRID_N; y > rect.top(); y -= GRID_SEP*GRID_N) {
        qreal yf = (qreal)y;
        painter->drawLine(rect.left(), yf, rect.right(), yf);
    }

    for (int y = GRID_SEP*GRID_N; y < rect.bottom(); y += GRID_SEP*GRID_N) {
        qreal yf = (qreal)y;
        painter->drawLine(rect.left(), yf, rect.right(), yf);
    }

    painter->setPen(pen3);
    painter->drawLine(rect.left(), 0, rect.right(), 0);
    painter->drawLine(0, rect.top(), 0, rect.bottom());
}

void TikzView::wheelEvent(QWheelEvent *event)
{
    QSettings settings("tikzit", "tikzit");
    bool shiftScroll = settings.value("shift-to-scroll", false).toBool();
    if ((!shiftScroll && event->modifiers() == Qt::NoModifier) ||
        (shiftScroll && (event->modifiers() == Qt::ShiftModifier)))
    {
        event->setModifiers(Qt::NoModifier);
        QGraphicsView::wheelEvent(event);
    }

    if (event->modifiers() & Qt::ControlModifier) {
        if (event->angleDelta().y() > 0) {
            zoomIn();
        } else if (event->angleDelta().y() < 0) {
            zoomOut();
        }
    } else if (event->modifiers() & Qt::ShiftModifier) {
        horizontalScrollBar()->setValue(horizontalScrollBar()->value() - event->angleDelta().y());
    }
}

// =========================================================================
// MGB-UML: Drag and Drop Handoff
// =========================================================================

void TikzView::dragEnterEvent(QDragEnterEvent *event)
{
    // Pass the event directly to the base class so it securely routes to TikzScene
    QGraphicsView::dragEnterEvent(event);
}

void TikzView::dragMoveEvent(QDragMoveEvent *event) 
{
    QGraphicsView::dragMoveEvent(event);
}

void TikzView::dropEvent(QDropEvent *event)
{
    QGraphicsView::dropEvent(event);
    
    // Automatically steal window focus back so the user doesn't have to click multiple times
    this->setFocus();
}