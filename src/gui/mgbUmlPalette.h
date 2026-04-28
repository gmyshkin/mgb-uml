#ifndef MGB_UMLPALETTE_H
#define MGB_UMLPALETTE_H

#include <QDockWidget>
#include <QListWidget>

namespace mgb {

class UmlPalette : public QDockWidget {
    Q_OBJECT
public:
    explicit UmlPalette(QWidget *parent = nullptr);

private:
    QListWidget *listWidget;
};

} // namespace mgb

#endif // MGB_UMLPALETTE_H