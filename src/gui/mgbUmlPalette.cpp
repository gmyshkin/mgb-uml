#include "mgbUmlPalette.h"

#include <QListView>
#include <QPainter>
#include <QPixmap>
#include <QPen>
#include <QFont>

namespace {

QIcon createUseCasePreviewIcon()
{
    QPixmap px(120, 80);
    px.fill(Qt::transparent);

    QPainter p(&px);
    p.setRenderHint(QPainter::Antialiasing, true);
    p.setPen(QPen(Qt::black, 3));
    p.setBrush(Qt::white);
    QRectF oval(8, 8, 104, 64);
    p.drawEllipse(oval);

    QFont f;
    f.setPointSize(11);
    p.setFont(f);
    p.drawText(oval, Qt::AlignCenter, "Use Case");

    return QIcon(px);
}

QIcon createClassPreviewIcon()
{
    QPixmap px(120, 90);
    px.fill(Qt::transparent);

    QPainter p(&px);
    p.setRenderHint(QPainter::Antialiasing, true);
    p.setPen(QPen(Qt::black, 3));
    p.setBrush(Qt::white);

    QRectF box(8, 8, 104, 74);
    p.drawRect(box);

    qreal y1 = box.top() + 24;
    qreal y2 = box.top() + 49;

    p.drawLine(QPointF(box.left(), y1), QPointF(box.right(), y1));
    p.drawLine(QPointF(box.left(), y2), QPointF(box.right(), y2));

    QFont titleFont;
    titleFont.setPointSize(9);
    titleFont.setBold(true);
    p.setFont(titleFont);
    p.drawText(QRectF(box.left(), box.top(), box.width(), 22), Qt::AlignCenter, "Class");

    QFont bodyFont;
    bodyFont.setPointSize(7);
    bodyFont.setBold(false);
    p.setFont(bodyFont);
    p.drawText(QRectF(box.left() + 6, y1 + 1, box.width() - 12, 18), Qt::AlignLeft | Qt::AlignVCenter, "+ attr");
    p.drawText(QRectF(box.left() + 6, y2 + 1, box.width() - 12, 18), Qt::AlignLeft | Qt::AlignVCenter, "+ method()");

    return QIcon(px);
}

QIcon createActorPreviewIcon()
{
    QPixmap px(120, 100);
    px.fill(Qt::transparent);

    QPainter p(&px);
    p.setRenderHint(QPainter::Antialiasing, true);
    p.setPen(QPen(Qt::black, 3));
    p.setBrush(Qt::NoBrush);

    qreal cx = 60;
    qreal headR = 12;
    qreal headCy = 22;

    p.drawEllipse(QPointF(cx, headCy), headR, headR);
    p.drawLine(QPointF(cx, headCy + headR), QPointF(cx, 58));
    p.drawLine(QPointF(cx - 18, 42), QPointF(cx + 18, 42));
    p.drawLine(QPointF(cx, 58), QPointF(cx - 16, 80));
    p.drawLine(QPointF(cx, 58), QPointF(cx + 16, 80));

    return QIcon(px);
}

QIcon createSystemPreviewIcon()
{
    QPixmap px(120, 90);
    px.fill(Qt::transparent);

    QPainter p(&px);
    p.setRenderHint(QPainter::Antialiasing, true);
    p.setPen(QPen(Qt::black, 3));
    p.setBrush(Qt::white);

    QRectF box(10, 10, 100, 70);
    p.drawRect(box);

    QFont f;
    f.setPointSize(10);
    p.setFont(f);
    p.drawText(QRectF(box.left(), box.top() + 4, box.width(), 20), Qt::AlignCenter, "System");

    return QIcon(px);
}

} // anonymous namespace

namespace mgb {

UmlPalette::UmlPalette(QWidget *parent) : QDockWidget("UML Elements", parent)
{
    listWidget = new QListWidget(this);

    listWidget->setViewMode(QListView::IconMode);
    listWidget->setIconSize(QSize(120, 90));
    listWidget->setMovement(QListView::Static);
    listWidget->setResizeMode(QListView::Adjust);

    listWidget->setFlow(QListView::TopToBottom);
    listWidget->setWrapping(false);
    listWidget->setWordWrap(true);
    listWidget->setGridSize(QSize(160, 130));
    listWidget->setSpacing(6);

    listWidget->setDragEnabled(true);
    listWidget->setDropIndicatorShown(false);

    QListWidgetItem *useCaseItem = new QListWidgetItem(createUseCasePreviewIcon(), "UML Use Case");
    useCaseItem->setToolTip("Drag to add a Use Case");
    listWidget->addItem(useCaseItem);

    QListWidgetItem *classItem = new QListWidgetItem(createClassPreviewIcon(), "UML Class");
    classItem->setToolTip("Drag to add a Class");
    listWidget->addItem(classItem);

    QListWidgetItem *actorItem = new QListWidgetItem(createActorPreviewIcon(), "UML Actor");
    actorItem->setToolTip("Drag to add an Actor");
    listWidget->addItem(actorItem);

    QListWidgetItem *systemItem = new QListWidgetItem(createSystemPreviewIcon(), "UML System");
    systemItem->setToolTip("Drag to add a System boundary");
    listWidget->addItem(systemItem);

    setWidget(listWidget);
}

} // namespace mgb
