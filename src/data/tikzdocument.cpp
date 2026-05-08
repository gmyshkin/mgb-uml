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

#include <QFile>
#include <QFileInfo>
#include <QSettings>
#include <QTextStream>
#include <QMessageBox>
#include <QFileDialog>
#include <QRegularExpression>
#include <QStringList>

#include "tikzit.h"
#include "tikzdocument.h"
#include "tikzassembler.h"
#include "mainwindow.h"

namespace {

const QString EDITABLE_BEGIN = "% MGB-UML editable TikZ begin";
const QString EDITABLE_END = "% MGB-UML editable TikZ end";
const QString EDITABLE_PREFIX = "% MGB-UML editable TikZ: ";

QString editableSnapshotFromTikz(const QString &tikz)
{
    QStringList editableLines;
    bool inEditableBlock = false;

    const QStringList lines = tikz.split('\n');
    for (const QString &line : lines) {
        const QString trimmed = line.trimmed();
        if (trimmed == EDITABLE_BEGIN) {
            editableLines.clear();
            inEditableBlock = true;
            continue;
        }

        if (trimmed == EDITABLE_END) {
            return editableLines.join("\n") + "\n";
        }

        if (inEditableBlock && line.startsWith(EDITABLE_PREFIX)) {
            editableLines << line.mid(EDITABLE_PREFIX.length());
        }
    }

    return QString();
}

QString tikzWithEditableSnapshot(const QString &compiledTikz, const QString &editableTikz)
{
    QString out;
    QTextStream stream(&out);

    stream << EDITABLE_BEGIN << "\n";
    const QStringList lines = editableTikz.split('\n');
    for (const QString &line : lines) {
        stream << EDITABLE_PREFIX << line << "\n";
    }
    stream << EDITABLE_END << "\n\n";
    stream << compiledTikz;
    stream.flush();

    return out;
}

QString tikzNodeLabelFromLine(const QString &line)
{
    int start = line.indexOf(") {");
    if (start < 0) return QString();

    QString label = line.mid(start + 3).trimmed();
    if (label.endsWith("};")) label.chop(2);
    return label;
}

QString recoveredNodeLine(const QString &style, const QString &name,
                          const QString &x, const QString &y,
                          const QString &label)
{
    return "\t\t\\node [style=" + style + "] (" + name + ") at (" +
           x + ", " + y + ") {" + label + "};";
}

QString recoverRenderedPluginTikz(const QString &tikz)
{
    struct RenderedNode {
        QString name;
        QString x;
        QString y;
        QString style;
        QString label;
        int line = -1;
    };

    QString beginLine = "\\begin{tikzpicture}";
    QStringList bboxLines;
    QStringList simpleNodeLines;
    QStringList edgeStatements;
    QList<RenderedNode> recoveredNodes;
    QSet<QString> recoveredNames;

    const QStringList lines = tikz.split('\n');
    QRegularExpression beginRe("^\\s*\\\\begin\\{tikzpicture\\}.*$");
    QRegularExpression renderedNodeRe("^\\s*\\\\node\\s+\\[([^\\]]*)\\]\\s+\\(([^\\)]+)\\)\\s+at\\s+\\(([-+0-9.]+)\\s*,\\s*([-+0-9.]+)\\)\\s+\\{\\}\\s*;\\s*$");
    QRegularExpression simpleNodeRe("^\\s*\\\\node\\s+(\\[[^\\]]*\\]\\s+)?\\(([^\\)]+)\\)\\s+at\\s+\\(([-+0-9.]+)\\s*,\\s*([-+0-9.]+)\\)\\s+\\{.*\\}\\s*;\\s*$");

    for (int i = 0; i < lines.size(); ++i) {
        const QString line = lines[i];
        if (beginRe.match(line).hasMatch()) {
            beginLine = line.trimmed();
        }

        if (line.contains("use as bounding box")) {
            bboxLines << line;
        }

        QRegularExpressionMatch renderedMatch = renderedNodeRe.match(line);
        if (renderedMatch.hasMatch()) {
            const QString options = renderedMatch.captured(1);
            const QString name = renderedMatch.captured(2);
            const QString x = renderedMatch.captured(3);
            const QString y = renderedMatch.captured(4);

            int end = i + 1;
            while (end < lines.size() &&
                   !renderedNodeRe.match(lines[end]).hasMatch() &&
                   !lines[end].contains("\\end{pgfonlayer}")) {
                ++end;
            }

            QString style;
            QString label;
            QString classTitle;
            QStringList classBodies;

            for (int j = i + 1; j < end; ++j) {
                const QString blockLine = lines[j];
                if (blockLine.contains("circle [radius=0.12cm]")) {
                    style = "UML Actor";
                }
                if (blockLine.contains(" ellipse [x radius=")) {
                    style = "UML Use Case";
                }
                if (blockLine.contains(name + ".north)")) {
                    classTitle = tikzNodeLabelFromLine(blockLine);
                } else if (blockLine.contains(name + ".north west)")) {
                    classBodies << tikzNodeLabelFromLine(blockLine);
                } else if (blockLine.contains("[yshift=-0.80cm]" + name + ")")) {
                    label = tikzNodeLabelFromLine(blockLine);
                } else if (style == "UML Use Case" && blockLine.contains("\\node") && blockLine.contains(" at (" + x + ", " + y + ")")) {
                    label = tikzNodeLabelFromLine(blockLine);
                }
            }

            if (!classTitle.isEmpty()) {
                style = "UML Class";
                label = classTitle + " \\nodepart{two} " + classBodies.value(0) +
                        " \\nodepart{three} " + classBodies.value(1);
            } else if (style.isEmpty() && classBodies.size() == 1) {
                style = "UML System";
                label = classBodies[0];
            } else if (style.isEmpty() && options.contains("shape=ellipse")) {
                style = "UML Use Case";
            }

            if (!style.isEmpty()) {
                RenderedNode node;
                node.name = name;
                node.x = x;
                node.y = y;
                node.style = style;
                node.label = label;
                node.line = i;
                recoveredNodes << node;
                recoveredNames.insert(name);
                i = end - 1;
            }
            continue;
        }

        QRegularExpressionMatch simpleMatch = simpleNodeRe.match(line);
        if (simpleMatch.hasMatch() && !line.contains("draw=none")) {
            simpleNodeLines << line;
        }
    }

    for (int i = 0; i < lines.size(); ++i) {
        QString statement = lines[i];
        if (!statement.trimmed().startsWith("\\draw")) continue;

        while (!statement.contains(";") && i + 1 < lines.size()) {
            ++i;
            statement += "\n" + lines[i];
        }

        if (statement.contains(" to ")) {
            edgeStatements << statement;
        }
    }

    if (recoveredNodes.isEmpty() && simpleNodeLines.isEmpty() && edgeStatements.isEmpty()) {
        return QString();
    }

    QString recovered;
    QTextStream stream(&recovered);
    stream << beginLine << "\n";
    for (const QString &bbox : bboxLines) stream << bbox << "\n";

    if (!recoveredNodes.isEmpty() || !simpleNodeLines.isEmpty()) {
        stream << "\t\\begin{pgfonlayer}{nodelayer}\n";
        for (const RenderedNode &node : recoveredNodes) {
            stream << recoveredNodeLine(node.style, node.name, node.x, node.y, node.label) << "\n";
        }
        for (const QString &line : simpleNodeLines) {
            QRegularExpressionMatch simpleMatch = simpleNodeRe.match(line);
            if (!simpleMatch.hasMatch() || !recoveredNames.contains(simpleMatch.captured(2))) {
                stream << line << "\n";
            }
        }
        stream << "\t\\end{pgfonlayer}\n";
    }

    if (!edgeStatements.isEmpty()) {
        stream << "\t\\begin{pgfonlayer}{edgelayer}\n";
        for (const QString &edge : edgeStatements) stream << edge << "\n";
        stream << "\t\\end{pgfonlayer}\n";
    }

    stream << "\\end{tikzpicture}\n";
    stream.flush();
    return recovered;
}

bool parseIntoGraph(Graph *graph, const QString &tikz)
{
    TikzAssembler ass(graph);
    return ass.parse(tikz);
}

} // namespace

TikzDocument::TikzDocument(QObject *parent) : QObject(parent)
{
    _graph = new Graph(this);
    _parseSuccess = true;
    _fileName = "";
    _shortName = "";
    _undoStack = new QUndoStack(this);
    _undoStack->setClean();
}

QUndoStack *TikzDocument::undoStack() const
{
    return _undoStack;
}

Graph *TikzDocument::graph() const
{
    return _graph;
}

QString TikzDocument::tikz() const
{
    return _tikz;
}

void TikzDocument::open(QString fileName)
{
    _fileName = fileName;
    QFile file(fileName);
    QFileInfo fi(file);
    _shortName = fi.fileName();
    QSettings settings("tikzit", "tikzit");
    settings.setValue("previous-file-path", fi.absolutePath());

    // if the file does not exist, only set the file name. The file will be written on
    // the first save.
    if (!file.exists()) {
        refreshTikz();
        _undoStack->resetClean();
        _parseSuccess = true;
        return;
    }

    if (!file.open(QIODevice::ReadOnly)) {
       // QMessageBox::critical(NULL, tr("Error"),
       //         tr("Could not open file"));
        _parseSuccess = false;
        return;
    }

    addToRecentFiles();

    QTextStream in(&file);
    _tikz = in.readAll();
    file.close();

    // Load the associated .tikzstyles file
    QString stylesFile = fi.absolutePath() + "/" + fi.baseName() + ".tikzstyles";
    if (QFile(stylesFile).exists()) {
        tikzit->loadStyles(stylesFile, true);
    }

    QString parseTikz = editableSnapshotFromTikz(_tikz);
    if (parseTikz.isEmpty()) parseTikz = _tikz;

    Graph *oldGraph = _graph;
    Graph *newGraph = new Graph(this);
    bool parsed = parseIntoGraph(newGraph, parseTikz);

    if (!parsed && parseTikz != _tikz) {
        newGraph->deleteLater();
        newGraph = new Graph(this);
        parsed = parseIntoGraph(newGraph, _tikz);
    }

    if (!parsed) {
        QString recoveredTikz = recoverRenderedPluginTikz(_tikz);
        if (!recoveredTikz.isEmpty()) {
            newGraph->deleteLater();
            newGraph = new Graph(this);
            parsed = parseIntoGraph(newGraph, recoveredTikz);
        }
    }

    if (parsed) {
        _graph = newGraph;
        oldGraph->deleteLater();
        foreach (Node *n, _graph->nodes()) n->attachStyle();
        foreach (Edge *e, _graph->edges()) {
            e->attachStyle();
            e->updateControls();
        }
        _parseSuccess = true;
        refreshTikz();
        setClean();
    } else {
       // QMessageBox::critical(NULL, tr("Error"),
       //         tr("Could not parse tikz file."));
        newGraph->deleteLater();
        _parseSuccess = false;
    }
}

bool TikzDocument::save() {
    if (_fileName == "") {
        return saveAs();
    } else {
        MainWindow *win = tikzit->activeWindow();
        if (win != nullptr && !win->tikzScene()->enabled()) {
            win->tikzScene()->parseTikz(win->tikzSource());
            if (!win->tikzScene()->enabled()) {
                auto resp = QMessageBox::question(nullptr,
                  tr("Tikz failed to parse"),
                  tr("Cannot save file with invalid TiKZ source. Revert changes and save?"));
                if (resp == QMessageBox::Yes) win->tikzScene()->setEnabled(true);
                else return false; // ABORT the save
            }
        }

        refreshTikz();
        QFile file(_fileName);
        QFileInfo fi(file);
        _shortName = fi.fileName();
        QSettings settings("tikzit", "tikzit");
        settings.setValue("previous-file-path", fi.absolutePath());

        if (file.open(QIODevice::WriteOnly)) {
            QTextStream stream(&file);
            stream << tikzWithEditableSnapshot(_tikz, _graph->tikz(false));
            file.close();
            setClean();
            addToRecentFiles();
            return true;
        } else {
            QMessageBox::warning(nullptr,
                "Save Failed", "Could not open file: '" + _fileName + "' for writing.");
        }
    }

    return false;
}

bool TikzDocument::isClean() const
{
    return _undoStack->isClean();
}

void TikzDocument::setClean()
{
    _undoStack->setClean();
}

QString TikzDocument::fileName() const
{
    return _fileName;
}

bool TikzDocument::isEmpty()
{
    return _graph->nodes().isEmpty();
}

void TikzDocument::addToRecentFiles()
{
    QSettings settings("tikzit", "tikzit");
    if (!_fileName.isEmpty()) {
        QStringList recentFiles = settings.value("recent-files").toStringList();

        // if the file is in the list already, shift it to the top. Otherwise, add it.
        recentFiles.removeAll(_fileName);
        recentFiles.prepend(_fileName);

        // keep max 10 files
        while (recentFiles.size() > 10) recentFiles.removeLast();

        settings.setValue("recent-files", recentFiles);
        tikzit->updateRecentFiles();
    }
}

void TikzDocument::setGraph(Graph *graph)
{
    _graph = graph;
    refreshTikz();
}

bool TikzDocument::saveAs() {
    MainWindow *win = tikzit->activeWindow();
    if (win != nullptr && !win->tikzScene()->enabled()) {
        win->tikzScene()->parseTikz(win->tikzSource());
        if (!win->tikzScene()->enabled()) {
            auto resp = QMessageBox::question(nullptr,
              tr("Tikz failed to parse"),
              tr("Cannot save file with invalid TiKZ source. Revert changes and save?"));
            if (resp == QMessageBox::Yes) win->tikzScene()->setEnabled(true);
            else return false; // ABORT the save
        }
    }

    QSettings settings("tikzit", "tikzit");

    QFileDialog dialog;
    dialog.setDefaultSuffix("tikz");
    dialog.setWindowTitle(tr("Save File As"));
    dialog.setAcceptMode(QFileDialog::AcceptSave);
    dialog.setNameFilter(tr("TiKZ Files (*.tikz)"));
    dialog.setFileMode(QFileDialog::AnyFile);
    dialog.setDirectory(settings.value("previous-file-path").toString());
    dialog.setOption(QFileDialog::DontUseNativeDialog);

    if (dialog.exec() && !dialog.selectedFiles().isEmpty()) {
        QString fileName = dialog.selectedFiles()[0];
        _fileName = fileName;
        if (save()) {
            // clean state might not change, so update title bar manually
            tikzit->activeWindow()->updateFileName();
            return true;
        }
    }

    return false;
}

QString TikzDocument::shortName() const
{
    return _shortName;
}

bool TikzDocument::parseSuccess() const
{
    return _parseSuccess;
}

void TikzDocument::refreshTikz()
{
    _tikz = _graph->tikz();
    if (MainWindow *w = dynamic_cast<MainWindow*>(parent()))
        w->refreshTikz();
}
