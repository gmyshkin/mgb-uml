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

#include "mainmenu.h"

#include <QAction>
#include <QDialog>
#include <QDialogButtonBox>
#include <QMenu>
#include <QTextBrowser>
#include <QVBoxLayout>

namespace {

void showBeginnerGuide(QWidget *parent)
{
    QDialog dialog(parent);
    dialog.setWindowTitle(QObject::tr("Beginner Guide"));
    dialog.resize(860, 680);

    QTextBrowser *guide = new QTextBrowser(&dialog);
    guide->setOpenExternalLinks(true);
    guide->setHtml(QObject::tr(R"(
        <h1>MGB-UML Beginner Guide</h1>

        <h2>What you build in this app</h2>
        <p>
        MGB-UML helps you draw UML diagrams and export them as TikZ/LaTeX.
        The canvas view is for arranging the diagram visually. The exported
        <code>.tikz</code> and <code>.tikzstyles</code> files are what LaTeX
        uses when you compile the final document.
        </p>

        <h2>Start a diagram</h2>
        <table border="1" cellspacing="0" cellpadding="10" width="100%">
          <tr>
            <td width="25%" align="center"><b>Palette</b><br/>Drag UML elements from here</td>
            <td width="50%" align="center"><b>Canvas</b><br/>Arrange actors, systems, classes, and use cases</td>
            <td width="25%" align="center"><b>Menus</b><br/>Save, export, reorder, and compile</td>
          </tr>
        </table>
        <p>
        The palette is where you choose UML pieces, the canvas is where you
        build the diagram, and the menus contain file, edit, reorder, and help
        actions.
        </p>
        <ol>
          <li>Create or open a TikZ diagram file.</li>
          <li>Use the UML palette to drag elements onto the canvas.</li>
          <li>Common starting elements are <b>UML System</b>, <b>UML Actor</b>,
              <b>UML Use Case</b>, and <b>UML Class</b>.</li>
          <li>Move elements by dragging them. Select an element first when you
              want to edit, delete, or reorder it.</li>
        </ol>

        <h2>Use case diagrams</h2>
        <p>
        Use case diagrams show who uses a system and what they can do with it.
        A typical use case diagram has:
        </p>
        <ul>
          <li><b>UML System</b>: the large boundary box for the application or service.</li>
          <li><b>UML Actor</b>: a person or outside system that interacts with it.</li>
          <li><b>UML Use Case</b>: an oval describing a goal, such as
              <i>Log in</i>, <i>Create Account</i>, or <i>Export Diagram</i>.</li>
          <li><b>Association</b>: a line connecting an actor to the use case they perform.</li>
        </ul>
        <p>
        Put the system box behind the use cases, then place actors outside the
        boundary and use cases inside or near the boundary depending on the
        diagram style you want.
        </p>

        <h2>Class diagrams</h2>
        <p>
        Class diagrams describe objects in your system. A UML class usually has
        three sections:
        </p>
        <ul>
          <li><b>Class name</b>: the top section.</li>
          <li><b>Attributes</b>: data stored by the class, such as
              <code>+ name</code> or <code>- id</code>.</li>
          <li><b>Methods</b>: behavior, such as <code>+ save()</code> or
              <code>- validate()</code>.</li>
        </ul>
        <p>
        The symbols <code>+</code>, <code>-</code>, and <code>#</code> usually
        mean public, private, and protected.
        </p>

        <h2>Edit labels and details</h2>
        <p>
        Double-click a UML element to edit its text or properties. For class
        diagrams, add attributes and methods on separate lines so the class box
        can grow cleanly. For long use case or actor names, keep the wording
        readable; the app and LaTeX export will size supported UML elements to
        keep text inside their shapes.
        </p>

        <h2>Layering: bring to front and send to back</h2>
        <p>
        If shapes overlap, select one or more elements and use:
        </p>
        <ul>
          <li><b>Edit &gt; Reorder &gt; Bring to Front</b> or <code>Ctrl+]</code>.</li>
          <li><b>Edit &gt; Reorder &gt; Send to Back</b> or <code>Ctrl+[</code>.</li>
        </ul>
        <p>
        A common use is sending a UML System box to the back, then bringing
        actors, classes, and use cases to the front.
        </p>

        <h2>Export and compile with LaTeX</h2>
        <ol>
          <li>Save the diagram. This writes the <code>.tikz</code> file.</li>
          <li>Make sure the matching <code>.tikzstyles</code> file is saved in
              the same folder. It contains the UML/plugin styles that LaTeX needs.</li>
          <li>In your LaTeX document, include the styles and diagram:
              <pre>\\input{mydiagram.tikzstyles}
\\begin{document}
\\input{mydiagram.tikz}
\\end{document}</pre>
          </li>
          <li>Compile with a normal LaTeX engine such as <code>pdflatex</code>,
              <code>xelatex</code>, or your editor's build button.</li>
        </ol>

        <h2>If the LaTeX result looks different</h2>
        <p>
        LaTeX is not drawing a screenshot of the app. It rebuilds the diagram
        from TikZ commands, fonts, sizes, and plugin style definitions. If the
        compiled version looks wrong, check these first:
        </p>
        <ul>
          <li>The <code>.tikzstyles</code> file is in the same folder and up to date.</li>
          <li>Your LaTeX document inputs the correct styles file before the diagram.</li>
          <li>The plugin that created the element is installed in MGB-UML and its
              exported style/preamble is present in the styles file.</li>
          <li>Save the diagram again after editing text or adding UML elements.</li>
        </ul>

        <h2>Good beginner habits</h2>
        <ul>
          <li>Start with the biggest container, such as the UML System box.</li>
          <li>Name actors and use cases with short, readable phrases.</li>
          <li>Use alignment and spacing to make relationships easy to follow.</li>
          <li>Save and compile early, then keep checking the PDF as the diagram grows.</li>
        </ul>
    )"));

    QDialogButtonBox *buttons = new QDialogButtonBox(QDialogButtonBox::Close, &dialog);
    QObject::connect(buttons, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

    QVBoxLayout *layout = new QVBoxLayout(&dialog);
    layout->addWidget(guide);
    layout->addWidget(buttons);

    dialog.exec();
}

void showChangelog(QWidget *parent)
{
    QDialog dialog(parent);
    dialog.setWindowTitle(QObject::tr("Changelog"));
    dialog.resize(860, 680);

    QTextBrowser *changelog = new QTextBrowser(&dialog);
    changelog->setOpenExternalLinks(true);
    changelog->setHtml(QObject::tr(R"(
        <h1>MGB-UML Changelog</h1>

        <p>
        This changelog tracks MGB-UML releases. TikZiT is the upstream project
        that MGB-UML is based on, so TikZiT itself is treated as the baseline
        rather than an MGB-UML release.
        </p>

        <h2>v1.1.0 - Plugin architecture, UML plugins, and release polish</h2>

        <h3>Added</h3>
        <ul>
          <li>Compiled plugin architecture for custom MGB-UML elements.</li>
          <li>Plugins menu with plugin installation and plugin management actions.</li>
          <li>Built-in UML plugins for UML Actor, UML Class, UML System, and UML Use Case.</li>
          <li>Plugin-backed palette metadata, icons, tooltips, categories, and default TikZ style properties.</li>
          <li>Custom Qt rendering for UML plugin nodes so the editor view matches UML shapes more closely.</li>
          <li>Custom TikZ export hooks for plugin nodes so LaTeX output can match editor rendering.</li>
          <li>Plugin geometry hints for edge attachment around custom node shapes.</li>
          <li>MGB-UML Plugin SDK with headers, documentation, geometry guidance, and a basic node plugin template.</li>
          <li>Plugin SDK GitHub Actions workflow.</li>
          <li>Beginner Guide entry under Help.</li>
          <li>Developer and upstream TikZiT attribution in the About dialog.</li>
          <li>Direct keyboard handling for Cut, Copy, and Paste in the diagram scene.</li>
          <li>Regression test for parser-friendly editable TikZ serialization of UML clipboard content.</li>
          <li>Changelog entry to Help tab.</li>
        </ul>

        <h3>Changed</h3>
        <ul>
          <li>UML element creation now uses drag-and-drop style metadata instead of hard-coded palette tool cases.</li>
          <li>Graph export can use plugin custom TikZ, while clipboard copy can request editable parser-friendly TikZ nodes.</li>
          <li>Improved edge routing and anchor calculation for rectangle, ellipse, actor, system, class, and plugin-defined shapes.</li>
          <li>Improved UML Class, UML System, UML Actor, and UML Use Case sizing and label handling in the editor and TikZ export.</li>
          <li>Windows, macOS, and Linux release workflows now build plugins as part of the release process.</li>
          <li>Windows workflow artifacts now extract directly to the app folder instead of containing another zip file.</li>
          <li>The release publish job creates the Windows release zip only at the GitHub Release publishing step.</li>
          <li>Shared core API symbols needed by Windows plugins are exported, including <code>floatToString</code>.</li>
          <li>Automatic update checker URL points to MGB-UML instead of TikZiT.</li>
        </ul>

        <h3>Fixed</h3>
        <ul>
          <li>Copy, Cut, and Paste for UML elements by copying editable TikZ node data instead of plugin-rendered drawing commands.</li>
          <li>Cut places copied content on the clipboard before selected items are removed.</li>
          <li>Pasted path data is included when inserting copied graph fragments.</li>
          <li>Pasted UML selections are visibly reselected after paste.</li>
          <li>Windows release plugin linking for plugins that use shared utility functions from the core app.</li>
          <li>Packaged Windows builds include custom plugins and remove compiler scratch files from the plugin folder.</li>
        </ul>

        <h2>v1.0.0 - Initial MGB-UML release</h2>

        <h3>Added</h3>
        <ul>
          <li>Forked TikZiT into MGB-UML as a UML-focused PGF/TikZ diagram editor.</li>
          <li>Rebranded the application name, executable, icons, update links, and release metadata for MGB-UML.</li>
          <li>MGB-UML palette for UML-oriented diagram creation.</li>
          <li>Initial UML element support, including UML classes and use cases.</li>
          <li>UML-oriented styling support through injected TikZ styles.</li>
          <li>UML edge styling support, including generalization, aggregation, and composition-style edge behavior.</li>
          <li>MGB-UML-specific file management helpers.</li>
          <li>Bring-to-front and send-to-back ordering for overlapping diagram elements.</li>
          <li>Release automation for Windows, macOS, and Linux builds.</li>
          <li>Generated API documentation and documentation publishing workflow.</li>
          <li>Docker-related development and deployment files.</li>
          <li>Project version metadata.</li>
        </ul>

        <h3>Changed</h3>
        <ul>
          <li>Updated the build from the upstream TikZiT baseline for the MGB-UML target.</li>
          <li>Updated editor behavior and palette flow around UML diagram workflows.</li>
          <li>Updated PDF/TikZ handling and style generation for MGB-UML diagrams.</li>
          <li>Updated menu text, app metadata, and repository links for the MGB-UML project.</li>
        </ul>

        <h3>Fixed</h3>
        <ul>
          <li>Early MGB-UML packaging paths for the renamed executable.</li>
          <li>Release scripts package MGB-UML assets instead of the original TikZiT executable naming.</li>
        </ul>
    )"));

    QDialogButtonBox *buttons = new QDialogButtonBox(QDialogButtonBox::Close, &dialog);
    QObject::connect(buttons, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

    QVBoxLayout *layout = new QVBoxLayout(&dialog);
    layout->addWidget(changelog);
    layout->addWidget(buttons);

    dialog.exec();
}

} // namespace
#include "preferencedialog.h"
#include "tikzit.h"
#include "mgbPluginDialog.h" // NEW: Required for the Plugin Manager UI

#include <QDebug>
#include <QSettings>
#include <QMessageBox>
#include <QFileDialog>
#include <QCoreApplication>
#include <QDir>

MainMenu::MainMenu()
{
    QSettings settings("tikzit", "tikzit");
    ui.setupUi(this);

    if (!settings.value("check-for-updates").isNull()) {
        ui.actionCheck_for_updates_automatically->blockSignals(true);
        ui.actionCheck_for_updates_automatically->setChecked(settings.value("check-for-updates").toBool());
        ui.actionCheck_for_updates_automatically->blockSignals(false);
    }

    updateRecentFiles();

    // =====================================================================
    // MGB-UML: DYNAMIC PLUGIN MENU INSTALLER & MANAGER
    // =====================================================================
    QMenu *pluginMenu = new QMenu(tr("&Plugins"), this);
    QAction *loadPluginAct = new QAction(tr("Install New Plugin..."), this);
    QAction *managePluginAct = new QAction(tr("Manage Plugins..."), this);

    pluginMenu->addAction(loadPluginAct);
    pluginMenu->addAction(managePluginAct);
    
    // Try to insert it right before the "Help" menu for a native look
    QAction *helpAction = nullptr;
    foreach(QAction *a, this->actions()) {
        if (a->text().contains("Help")) helpAction = a;
    }
    if (helpAction) this->insertMenu(helpAction, pluginMenu);
    else this->addMenu(pluginMenu); // Fallback to end of the bar

    // 1. The Install Action Lambda
    connect(loadPluginAct, &QAction::triggered, this, [this]() {
        QString filter = "MGB Plugins (*.dll *.so *.dylib *.json)";
        QString filePath = QFileDialog::getOpenFileName(this, tr("Select Plugin File"), "", filter);
        
        if (filePath.isEmpty()) return;

        // Find the app's secret plugin directory
        QString pluginDir = QCoreApplication::applicationDirPath() + "/plugins";
        QDir dir;
        if (!dir.exists(pluginDir)) dir.mkpath(pluginDir);

        QFileInfo fi(filePath);
        QString destPath = pluginDir + "/" + fi.fileName();

        // Copy the main plugin file over
        if (QFile::exists(destPath)) QFile::remove(destPath);
        if (!QFile::copy(filePath, destPath)) {
            QMessageBox::critical(this, "Error", "Could not copy plugin file. Check permissions.");
            return;
        }

        // If it's a JSON plugin, prompt them for their custom icon!
        if (fi.suffix().toLower() == "json") {
            if (QMessageBox::question(this, "Upload Icon", "Would you like to upload a custom SVG/PNG icon for this plugin?") == QMessageBox::Yes) {
                QString iconPath = QFileDialog::getOpenFileName(this, tr("Select Icon"), "", "Images (*.svg *.png)");
                if (!iconPath.isEmpty()) {
                    QFileInfo iconFi(iconPath);
                    QString iconDest = pluginDir + "/" + iconFi.fileName();
                    if (QFile::exists(iconDest)) QFile::remove(iconDest);
                    QFile::copy(iconPath, iconDest);
                }
            }
        }

        // NEW CUSTOM SUCCESS DIALOG (Fixes the screenshot bug!)
        QMessageBox msgBox(this);
        msgBox.setWindowTitle("Installation Complete");
        msgBox.setText("<h2>Plugin Successfully Added!</h2><p><b>" + fi.fileName() + "</b> has been installed.</p><p>You must restart MGB-UML to see it in your palette.</p>");
        msgBox.setIcon(QMessageBox::Information);
        msgBox.exec();
    });

    // 2. The Manage Action Lambda
    connect(managePluginAct, &QAction::triggered, this, [this]() {
        mgbPluginDialog dlg(this);
        dlg.exec();
    });
    // =====================================================================
}

void MainMenu::addDocks(QMenu *m)
{
    ui.menuView->addSeparator();
    foreach (QAction *a, m->actions()) {
        if (!a->isSeparator()) ui.menuView->addAction(a);
    }
}

QAction *MainMenu::updatesAction()
{
    return ui.actionCheck_for_updates_automatically;
}

void MainMenu::updateRecentFiles()
{
    QSettings settings("tikzit", "tikzit");
    ui.menuOpen_Recent->clear();

    QStringList recentFiles = settings.value("recent-files").toStringList();

    QAction *action;
    foreach (QString f, recentFiles) {
        QFileInfo fi(f);
        action = new QAction(fi.fileName(), ui.menuOpen_Recent);
        action->setData(f);
        ui.menuOpen_Recent->addAction(action);
        connect(action, SIGNAL(triggered()),
                this, SLOT(openRecent()));
    }

    ui.menuOpen_Recent->addSeparator();
    action = new QAction("Clear List", ui.menuOpen_Recent);
    connect(action, SIGNAL(triggered()),
            tikzit, SLOT(clearRecentFiles()));
    ui.menuOpen_Recent->addAction(action);
}

// File
void MainMenu::on_actionNew_triggered()
{
    tikzit->newDoc();
}

void MainMenu::on_actionOpen_triggered()
{
    tikzit->open();
}

void MainMenu::on_actionClose_triggered()
{
    if (tikzit->dialogStatus()) {
        tikzit->previewWindow()->close();
    } else if (tikzit->activeWindow() != 0) {
        tikzit->activeWindow()->close();
    }
}

void MainMenu::on_actionSave_triggered()
{
    if (tikzit->activeWindow() != 0)
        tikzit->activeWindow()->tikzDocument()->save();
}

void MainMenu::on_actionSave_As_triggered()
{
    if (tikzit->activeWindow() != 0)
        tikzit->activeWindow()->tikzDocument()->saveAs();
}

void MainMenu::on_actionExit_triggered()
{
    tikzit->quit();
}

void MainMenu::openRecent()
{
    if (sender() != nullptr) {
        if (QAction *action = dynamic_cast<QAction*>(sender())) {
            tikzit->open(action->data().toString());
        }
    }
}


// Edit
void MainMenu::on_actionUndo_triggered()
{
    if (tikzit->activeWindow() != 0)
        tikzit->activeWindow()->tikzDocument()->undoStack()->undo();
}

void MainMenu::on_actionRedo_triggered()
{
    if (tikzit->activeWindow() != 0)
        tikzit->activeWindow()->tikzDocument()->undoStack()->redo();
}

void MainMenu::on_actionCut_triggered()
{
    if (tikzit->activeWindow() != 0)
        tikzit->activeWindow()->tikzScene()->cutToClipboard();
}

void MainMenu::on_actionCopy_triggered()
{
    if (tikzit->activeWindow() != 0)
        tikzit->activeWindow()->tikzScene()->copyToClipboard();
}

void MainMenu::on_actionPaste_triggered()
{
    if (tikzit->activeWindow() != 0)
        tikzit->activeWindow()->tikzScene()->pasteFromClipboard();
}

void MainMenu::on_actionDelete_triggered()
{
    if (tikzit->activeWindow() != 0)
        tikzit->activeWindow()->tikzScene()->deleteSelectedItems();
}

void MainMenu::on_actionSelect_All_triggered()
{
    if (tikzit->activeWindow() != 0)
        tikzit->activeWindow()->tikzScene()->selectAllNodes();
}

void MainMenu::on_actionDeselect_All_triggered()
{
    if (tikzit->activeWindow() != 0)
        tikzit->activeWindow()->tikzScene()->deselectAll();
}

void MainMenu::on_actionReflectHorizontal_triggered()
{
    if (tikzit->activeWindow() != 0)
        tikzit->activeWindow()->tikzScene()->reflectNodes(true);
}

void MainMenu::on_actionReflectVertical_triggered()
{
    if (tikzit->activeWindow() != 0)
        tikzit->activeWindow()->tikzScene()->reflectNodes(false);
}

void MainMenu::on_actionRotateCW_triggered() {
    if (tikzit->activeWindow() != 0)
        tikzit->activeWindow()->tikzScene()->rotateNodes(true);
}

void MainMenu::on_actionRotateCCW_triggered() {
    if (tikzit->activeWindow() != 0)
        tikzit->activeWindow()->tikzScene()->rotateNodes(false);
}

void MainMenu::on_actionBring_to_Front_triggered()
{
    if (tikzit->activeWindow() != 0)
        tikzit->activeWindow()->tikzScene()->reorderSelection(true);
}

void MainMenu::on_actionSend_to_Back_triggered()
{
    if (tikzit->activeWindow() != 0)
        tikzit->activeWindow()->tikzScene()->reorderSelection(false);
}

void MainMenu::on_actionExtendUp_triggered()
{
    if (tikzit->activeWindow() != 0)
        tikzit->activeWindow()->tikzScene()->extendSelectionUp();
}

void MainMenu::on_actionExtendDown_triggered()
{
    if (tikzit->activeWindow() != 0)
        tikzit->activeWindow()->tikzScene()->extendSelectionDown();
}

void MainMenu::on_actionExtendLeft_triggered()
{
    if (tikzit->activeWindow() != 0)
        tikzit->activeWindow()->tikzScene()->extendSelectionLeft();
}

void MainMenu::on_actionExtendRight_triggered()
{
    if (tikzit->activeWindow() != 0)
        tikzit->activeWindow()->tikzScene()->extendSelectionRight();
}

void MainMenu::on_actionReverse_Edge_Direction_triggered()
{
    if (tikzit->activeWindow() != 0)
        tikzit->activeWindow()->tikzScene()->reverseSelectedEdges();
}

void MainMenu::on_actionMerge_Nodes_triggered()
{
    if (tikzit->activeWindow() != 0)
        tikzit->activeWindow()->tikzScene()->mergeNodes();
}

void MainMenu::on_actionMake_Path_triggered()
{
    if (tikzit->activeWindow() != 0)
        tikzit->activeWindow()->tikzScene()->makePath(false);
}

void MainMenu::on_actionMake_Path_as_Background_triggered()
{
    if (tikzit->activeWindow() != 0)
        tikzit->activeWindow()->tikzScene()->makePath(true);
}

void MainMenu::on_actionSplit_Path_triggered()
{
    if (tikzit->activeWindow() != 0)
        tikzit->activeWindow()->tikzScene()->splitPath();
}


// Tikz
void MainMenu::on_actionParse_triggered()
{
    MainWindow *win = tikzit->activeWindow();
    if (win != 0) {
        if (win->tikzScene()->parseTikz(win->tikzSource())) {
            QList<int> sz = win->splitter()->sizes();
            sz[0] = sz[0] + sz[1];
            sz[1] = 0;
            win->splitter()->setSizes(sz);
        }
    }
}

void MainMenu::on_actionRevert_triggered()
{
    MainWindow *win = tikzit->activeWindow();
    if (win != 0) {
        win->tikzDocument()->refreshTikz();
        win->tikzScene()->setEnabled(true);
    }
}

void MainMenu::on_actionJump_to_Selection_triggered()
{
    MainWindow *win = tikzit->activeWindow();
    if (win != 0) {
        QList<int> sz = win->splitter()->sizes();
        if (sz[1] == 0) {
            sz[1] = 200;
            win->splitter()->setSizes(sz);
        }
        win->setSourceLine(win->tikzScene()->lineNumberForSelection());
    }
}

void MainMenu::on_actionRun_LaTeX_triggered()
{
    tikzit->makePreview();
}

void MainMenu::on_actionPrevious_Node_Style_triggered()
{
    tikzit->activeWindow()->stylePalette()->previousNodeStyle();
}

void MainMenu::on_actionNext_Node_Style_triggered()
{
    tikzit->activeWindow()->stylePalette()->nextNodeStyle();
}

void MainMenu::on_actionClear_Node_Style_triggered()
{
    tikzit->activeWindow()->stylePalette()->toggleClearNodeStyle();
}

void MainMenu::on_actionPrevious_Edge_Style_triggered()
{
    tikzit->activeWindow()->stylePalette()->previousEdgeStyle();
}

void MainMenu::on_actionNext_Edge_Style_triggered()
{
    tikzit->activeWindow()->stylePalette()->nextEdgeStyle();
}

void MainMenu::on_actionClear_Edge_Style_triggered()
{
    tikzit->activeWindow()->stylePalette()->toggleClearEdgeStyle();
}

void MainMenu::on_actionPreferences_triggered()
{
    PreferenceDialog *d = new PreferenceDialog(this);
    d->exec();
    d->deleteLater();
}


// View
void MainMenu::on_actionZoom_In_triggered()
{
    if (tikzit->activeWindow() != 0) tikzit->activeWindow()->tikzView()->zoomIn();
}

void MainMenu::on_actionZoom_Out_triggered()
{
    if (tikzit->activeWindow() != 0) tikzit->activeWindow()->tikzView()->zoomOut();
}

void MainMenu::on_actionShow_Node_Labels_triggered()
{
    if (tikzit->activeWindow() != 0) {
        tikzit->activeWindow()->tikzScene()->setDrawNodeLabels(ui.actionShow_Node_Labels->isChecked());
        tikzit->activeWindow()->tikzScene()->invalidate();
    }
}

void MainMenu::on_actionBeginner_Guide_triggered()
{
    showBeginnerGuide(this);
}

void MainMenu::on_actionChangelog_triggered()
{
    showChangelog(this);
}

void MainMenu::on_actionAbout_triggered()
{
    QMessageBox::about(this,
                       "MGB-UML",
                       "<h2><b>MGB-UML</b></h2>"
                       "<p><i>Version: " GIT_VERSION "</i></p>"
                       "<p>MGB-UML is a GUI diagram editor for PGF/TikZ. It is licensed under the "
                       "<a href=\"https://www.gnu.org/licenses/gpl-3.0.en.html\">GNU General "
                       "Public License, version 3.0</a>.</p>"
                       "<p>MGB-UML was created by Gleb Myshkin, Bowen Jiang, and Matthew Smith "
                       "from Stevens Institute of Technology.</p>"
                       "<p>MGB-UML is based on the open-source TikZiT project by Aleks Kissinger "
                       "and contributors. Original TikZiT copyright and GPL license notices are "
                       "preserved in this distribution.</p>"
                       "<p>For more info and updates, visit: "
                       "<a href=\"https://github.com/gmyshkin/mgb-uml\">MGB-UML</a></p>");
}

void MainMenu::on_actionCheck_for_updates_automatically_triggered()
{
    tikzit->setCheckForUpdates(ui.actionCheck_for_updates_automatically->isChecked());
}

void MainMenu::on_actionCheck_now_triggered()
{
    tikzit->checkForUpdates(true);
}
