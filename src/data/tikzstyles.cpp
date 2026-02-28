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

#include "tikzstyles.h"
#include "tikzassembler.h"

#include <QDebug>
#include <QColorDialog>
#include <QFile>
#include <QFileInfo>

TikzStyles::TikzStyles(QObject *parent) : QObject(parent)
{
    _nodeStyles = new StyleList(false, this);
    _edgeStyles = new StyleList(true, this);

    // Run the injector on app startup
    injectHardcodedStyles();
}

// =================================================================
// MGB-UML: SMART STYLE INJECTOR
// =================================================================
void TikzStyles::injectHardcodedStyles()
{
    // 1. UML Use Case
    Style *useCaseStyle = _nodeStyles->style("UML Use Case");
    if (useCaseStyle == nullptr) {
        // Doesn't exist in the loaded file? Create it from scratch.
        GraphElementData *data = new GraphElementData();
        data->setProperty("shape", "ellipse");
        data->setProperty("draw", "black");
        data->setProperty("fill", "white");
        data->setProperty("minimum width", "3cm");
        data->setProperty("minimum height", "1.5cm");
        data->setProperty("tikzit category", "MGB_UML_HIDDEN");
        addStyle("UML Use Case", data);
    } else {
        // Exists in the file! Respect their colors/settings, but force it hidden in UI.
        useCaseStyle->setProperty("tikzit category", "MGB_UML_HIDDEN");
    }

    // 2. UML Class
    Style *classStyle = _nodeStyles->style("UML Class");
    if (classStyle == nullptr) {
        GraphElementData *data = new GraphElementData();
        data->setProperty("shape", "rectangle split");
        data->setProperty("rectangle split parts", "3");
        data->setProperty("draw", "black");
        data->setProperty("fill", "white");
        data->setProperty("align", "left");
        data->setProperty("tikzit category", "MGB_UML_HIDDEN");
        addStyle("UML Class", data);
    } else {
        classStyle->setProperty("tikzit category", "MGB_UML_HIDDEN");
    }
}
// =================================================================

Style *TikzStyles::nodeStyle(QString name) const
{
    Style *s = _nodeStyles->style(name);
    return (s == nullptr) ? unknownStyle : s;
}

Style *TikzStyles::edgeStyle(QString name) const
{
    Style *s = _edgeStyles->style(name);
    return (s == nullptr) ? noneEdgeStyle : s;
}

void TikzStyles::clear()
{
    _nodeStyles->clear();
    _edgeStyles->clear();
}

bool TikzStyles::loadStyles(QString fileName)
{
    QFile file(fileName);
    if (file.open(QIODevice::ReadOnly)) {
        QTextStream in(&file);
        QString styleTikz = in.readAll();
        file.close();

        clear();
        TikzAssembler ass(this);
        bool success = ass.parse(styleTikz);
        
        // MGB-UML: After clearing and loading the user's file, ensure our hardcoded styles survive!
        injectHardcodedStyles();
        
        return success;
    } else {
        // Even if loading fails, restore the hardcoded styles so the app doesn't break
        injectHardcodedStyles();
        return false;
    }
}

bool TikzStyles::saveStyles(QString fileName)
{
    QFile file(fileName);
    if (file.open(QIODevice::WriteOnly)) {
        QTextStream stream(&file);
        stream << tikz();
        file.close();
        return true;
    }
    return false;
}

void TikzStyles::refreshModels(QStandardItemModel *nodeModel, QStandardItemModel *edgeModel, QString category, bool includeNone)
{
    nodeModel->clear();
    edgeModel->clear();

    QStandardItem *it;

    if (includeNone) {
        it = new QStandardItem(noneStyle->icon(), noneStyle->name());
        it->setEditable(false);
        it->setData(noneStyle->name());
        nodeModel->appendRow(it);
        it->setTextAlignment(Qt::AlignCenter);
    }

    Style *s;
    for (int i = 0; i < _nodeStyles->length(); ++i) {
        s = _nodeStyles->style(i);
        // Only add it if it's NOT in our hidden category
        if (s->propertyWithDefault("tikzit category", "", false) != "MGB_UML_HIDDEN") {
            if (category == "" || category == s->propertyWithDefault("tikzit category", "", false))
            {
                it = new QStandardItem(s->icon(), s->name());
                it->setEditable(false);
                it->setData(s->name());
                nodeModel->appendRow(it);
            }
        }
    }

    if (includeNone) {
        it = new QStandardItem(noneEdgeStyle->icon(), noneEdgeStyle->name());
        it->setEditable(false);
        it->setData(noneEdgeStyle->name());
        edgeModel->appendRow(it);
    }

    for (int i = 0; i < _edgeStyles->length(); ++i) {
        s = _edgeStyles->style(i);
        it = new QStandardItem(s->icon(), s->name());
        it->setEditable(false);
        it->setData(s->name());
        edgeModel->appendRow(it);
    }
}

StyleList *TikzStyles::nodeStyles() const
{
    return _nodeStyles;
}

StyleList *TikzStyles::edgeStyles() const
{
    return _edgeStyles;
}

QStringList TikzStyles::categories() const
{
    QMap<QString,bool> cats;
    cats.insert("", true);
    Style *ns;
    for (int i = 0; i < _nodeStyles->length(); ++i) {
        ns = _nodeStyles->style(i);
        QString cat = ns->propertyWithDefault("tikzit category", "", false);
        if (cat != "MGB_UML_HIDDEN") {
            cats.insert(cat, true);
        }
    }
    return QStringList(cats.keys());
}

QString TikzStyles::tikz() const
{
    QString str;
    QTextStream code(&str);

    code << "% TiKZ style file generated by MGB-UML. You may edit this file manually,\n";
    code << "% but some things (e.g. comments) may be overwritten. To be readable in\n";
    code << "% MGB-UML, the only non-comment lines must be of the form:\n";
    code << "% \\tikzstyle{NAME}=[PROPERTY LIST]\n\n";

    // =========================================================
    // MGB-UML: AUTOMATICALLY INJECT LAYER DEFINITIONS
    // =========================================================
    code << "% Required TikZ Libraries\n";
    code << "\\usetikzlibrary{shapes.multipart, positioning, shapes.geometric}\n\n";

    code << "% Ignore UI-specific keys\n";
    code << "\\pgfkeys{/tikz/tikzit category/.initial=}\n\n";

    code << "% Layer definitions\n";
    code << "\\pgfdeclarelayer{edgelayer}\n";
    code << "\\pgfdeclarelayer{nodelayer}\n";
    code << "\\pgfsetlayers{edgelayer,nodelayer,main}\n\n";
    // =========================================================

    code << "% Node styles\n";
    code << _nodeStyles->tikz();

    code << "\n% Edge styles\n";
    code << _edgeStyles->tikz();

    code.flush();
    return str;
}

void TikzStyles::addStyle(QString name, GraphElementData *data)
{
    Style *s = new Style(name, data);
    if (s->isEdgeStyle()) _edgeStyles->addStyle(s);
    else _nodeStyles->addStyle(s);
}