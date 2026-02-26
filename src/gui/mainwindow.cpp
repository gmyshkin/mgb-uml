#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "mainmenu.h"
#include "tikzassembler.h"
#include "toolpalette.h"
#include "tikzit.h"
#include "mgbumlpalette.h" // MGB-UML

#include <QDebug>
#include <QFile>
#include <QList>
#include <QSettings>
#include <QMessageBox>
#include <QFileDialog>
#include <QTextEdit>
#include <QTextBlock>
#include <QIcon>
#include <QPushButton>

int MainWindow::_numWindows = 0;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    QSettings settings("mgb-uml", "mgb-uml");
    _windowId = _numWindows;
    _numWindows++;
    ui->setupUi(this);

    setWindowIcon(QIcon(":/images/tikzit.png"));

    setAttribute(Qt::WA_DeleteOnClose, true);
    _tikzDocument = new TikzDocument(this);

    _toolPalette = new ToolPalette(this);
    addToolBar(_toolPalette);

    _stylePalette = new StylePalette(this);
    _stylePalette->setObjectName("stylePaletteDock");

    _umlPalette = new mgb::UmlPalette(this); 
    _umlPalette->setObjectName("umlPaletteDock");

    _tikzScene = new TikzScene(_tikzDocument, _toolPalette, _stylePalette, this);
    ui->tikzView->setScene(_tikzScene);

    _menu = new MainMenu();
    _menu->setParent(this);
    setMenuBar(_menu);

    QList<int> sz = ui->splitter->sizes();
    sz[0] = sz[0] + sz[1];
    sz[1] = 0;
    ui->splitter->setSizes(sz);

    _tikzDocument->refreshTikz();

    connect(_tikzDocument->undoStack(), SIGNAL(cleanChanged(bool)), this, SLOT(updateFileName()));
    _menu->addDocks(createPopupMenu());

    setFont();

    // MGB-UML: Force both docks to the right side of the screen
    addDockWidget(Qt::RightDockWidgetArea, _stylePalette);
    addDockWidget(Qt::RightDockWidgetArea, _umlPalette); 

    QVariant state = settings.value(QString("windowState-main-qt") + qVersion());
    if (state.isValid()) {
        restoreState(state.toByteArray(), 2);
    } 
    resizeDocks({_stylePalette, _umlPalette}, {130, 130}, Qt::Horizontal);
}

MainWindow::~MainWindow()
{
    tikzit->removeWindow(this);
    delete ui;
}

void MainWindow::setFont()
{
    QSettings settings("mgb-uml", "mgb-uml");
#if (QT_VERSION >= QT_VERSION_CHECK(5, 10, 0))
    ui->tikzSource->setTabStopDistance(20.0);
#else
    ui->tikzSource->setTabStopWidth(20);
#endif

    QFont font("Courier New", 12);
    if (settings.contains("source-font")) {
        font.fromString(settings.value("source-font").toString());
    }
    ui->tikzSource->setFont(font);
}

void MainWindow::restorePosition()
{
    QSettings settings("mgb-uml", "mgb-uml");
    QVariant geom = settings.value(QString("geometry-main-qt") + qVersion());

    if (geom.isValid()) {
        restoreGeometry(geom.toByteArray());
    }
}

void MainWindow::open(QString fileName)
{
    _tikzDocument->open(fileName);

    if (_tikzDocument->parseSuccess()) {
        statusBar()->showMessage("TiKZ parsed successfully", 2000);
        _tikzScene->setTikzDocument(_tikzDocument);
        updateFileName();
    } else {
        statusBar()->showMessage("Cannot read TiKZ source");
    }

}

QSplitter *MainWindow::splitter() const {
    return ui->splitter;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    QSettings settings("mgb-uml", "mgb-uml");
    settings.setValue(QString("geometry-main-qt") + qVersion(), saveGeometry());
    settings.setValue(QString("windowState-main-qt") + qVersion(), saveState(2));

    if (!_tikzDocument->isClean()) {
        QString nm = _tikzDocument->shortName();
        if (nm.isEmpty()) nm = "untitled";
        QMessageBox::StandardButton resBtn = QMessageBox::question(
                    this, "Save Changes",
                    "Do you wish to save changes to " + nm + "?",
                    QMessageBox::Cancel | QMessageBox::No | QMessageBox::Yes,
                    QMessageBox::Yes);

        if (resBtn == QMessageBox::Yes && _tikzDocument->save()) {
            event->accept();
        } else if (resBtn == QMessageBox::No) {
            event->accept();
        } else {
            event->ignore();
        }
    } else {
        event->accept();
    }
}

void MainWindow::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::ActivationChange && isActiveWindow()) {
        tikzit->setActiveWindow(this);
        tikzit->setDialogStatus(false);
    }
    QMainWindow::changeEvent(event);
}

MainMenu *MainWindow::menu() const
{
    return _menu;
}

StylePalette *MainWindow::stylePalette() const
{
    return _stylePalette;
}

QString MainWindow::tikzSource()
{
    return ui->tikzSource->toPlainText();
}

void MainWindow::setSourceLine(int line)
{
    QTextCursor cursor(ui->tikzSource->document()->findBlockByLineNumber(line));
    cursor.movePosition(QTextCursor::EndOfLine);
    ui->tikzSource->setTextCursor(cursor);
    ui->tikzSource->setFocus();
}

void MainWindow::updateFileName()
{
    QString nm = _tikzDocument->shortName();
    if (nm.isEmpty()) nm = "untitled";
    if (!_tikzDocument->isClean()) nm += "*";
    setWindowTitle(nm + " - MGB-UML");
}

void MainWindow::refreshTikz()
{
    ui->tikzSource->blockSignals(true);
    ui->tikzSource->setText(_tikzDocument->tikz());
    ui->tikzSource->blockSignals(false);
}

ToolPalette *MainWindow::toolPalette() const
{
    return _toolPalette;
}

TikzDocument *MainWindow::tikzDocument() const
{
    return _tikzDocument;
}

TikzScene *MainWindow::tikzScene() const
{
    return _tikzScene;
}

int MainWindow::windowId() const
{
    return _windowId;
}

TikzView *MainWindow::tikzView() const
{
    return ui->tikzView;
}

void MainWindow::on_tikzSource_textChanged()
{
    if (_tikzScene->enabled()) _tikzScene->setEnabled(false);
}