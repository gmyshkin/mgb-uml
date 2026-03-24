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

    injectHardcodedStyles();
}

// =================================================================
// MGB-UML: PRISTINE STYLE INJECTOR
// =================================================================
void TikzStyles::injectHardcodedStyles()
{
    // 1. UML Use Case
    Style *useCaseStyle = _nodeStyles->style("UML Use Case");
    if (useCaseStyle == nullptr) {
        GraphElementData *data = new GraphElementData();
        data->setProperty("shape", "ellipse");
        data->setProperty("draw", "black");
        data->setProperty("fill", "white");
        data->setProperty("minimum width", "3cm");
        data->setProperty("minimum height", "1.5cm");
        data->setProperty("tikzit category", "UML Elements"); 
        addStyle("UML Use Case", data);
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
        data->setProperty("tikzit category", "UML Elements");
        addStyle("UML Class", data);
    }
}
// 3. UML Actor
Style *actorStyle = _nodeStyles->style("UML Actor");
if (actorStyle == nullptr) {
    GraphElementData *data = new GraphElementData();
    data->setProperty("shape", "ellipse"); // placeholder for editor/export safety
    data->setProperty("draw", "black");
    data->setProperty("fill", "white");
    data->setProperty("tikzit category", "UML Elements");
    addStyle("UML Actor", data);
}

// 4. UML System
Style *systemStyle = _nodeStyles->style("UML System");
if (systemStyle == nullptr) {
    GraphElementData *data = new GraphElementData();
    data->setProperty("shape", "rectangle");
    data->setProperty("draw", "black");
    data->setProperty("fill", "white");
    data->setProperty("minimum width", "6cm");
    data->setProperty("minimum height", "4cm");
    data->setProperty("align", "center");
    data->setProperty("tikzit category", "UML Elements");
    addStyle("UML System", data);
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

        QString cleanTikz;
        QTextStream stream(&styleTikz);
        while (!stream.atEnd()) {
            QString line = stream.readLine();
            
            if (line.startsWith("\\usetikzlibrary") || 
                line.startsWith("\\pgfkeys") || 
                line.startsWith("\\pgfdeclarelayer") || 
                line.startsWith("\\pgfsetlayers")) {
                continue; 
            }
            
            // MGB-UML: AUTO-HEAL INTERCEPTOR
            if (line.startsWith("\\tikzstyle{UML Class}=") || 
    line.startsWith("\\tikzstyle{UML Use Case}=") ||
    line.startsWith("\\tikzstyle{UML Actor}=") ||
    line.startsWith("\\tikzstyle{UML System}=")) {
    continue; 
}

            cleanTikz += line + "\n";
        }

        clear();
        TikzAssembler ass(this);
        bool success = ass.parse(cleanTikz); 
        
        injectHardcodedStyles();
        saveStyles(fileName);
        
        return success;
    } else {
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
    // DEAD CODE: The TikZiT UI no longer uses this function!
}

StyleList *TikzStyles::nodeStyles() const
{
    return _nodeStyles;
}

StyleList *TikzStyles::edgeStyles() const
{
    return _edgeStyles;
}

// =================================================================
// MGB-UML: PERFECTED CATEGORY LIST
// =================================================================
QStringList TikzStyles::categories() const
{
    QStringList list;
    
    // Safely send the word "All" to the GUI. The GUI will translate it back to "" internally.
    list << "All"; 
    
    for (int i = 0; i < _nodeStyles->length(); ++i) {
        Style *ns = _nodeStyles->style(i);
        QString cat = ns->propertyWithDefault("tikzit category", "", false).trimmed();
        if (cat != "" && !list.contains(cat)) {
            list << cat;
        }
    }
    
    for (int i = 0; i < _edgeStyles->length(); ++i) {
        Style *es = _edgeStyles->style(i);
        QString cat = es->propertyWithDefault("tikzit category", "", false).trimmed();
        if (cat != "" && !list.contains(cat)) {
            list << cat;
        }
    }
    
    return list;
}

QString TikzStyles::tikz() const
{
    QString str;
    QTextStream code(&str);

    code << "% =========================================================\n";
    code << "% MGB-UML: PROTECTED STYLES WARNING\n";
    code << "% Do NOT modify the properties of 'UML Class' or 'UML Use Case' directly.\n";
    code << "% The app will automatically reset them to default to protect the palette.\n";
    code << "% \n";
    code << "% If you want a custom color or style, COPY the line and RENAME it.\n";
    code << "% (Example: \\tikzstyle{UML Class Red}=[... fill=red ...])\n";
    code << "% =========================================================\n\n";

    code << "% Required TikZ Libraries\n";
    code << "\\usetikzlibrary{shapes.multipart, positioning, shapes.geometric}\n\n";

    code << "% Ignore UI-specific keys\n";
    code << "\\pgfkeys{/tikz/tikzit category/.initial=}\n\n";

    code << "% Layer definitions\n";
    code << "\\pgfdeclarelayer{edgelayer}\n";
    code << "\\pgfdeclarelayer{nodelayer}\n";
    code << "\\pgfsetlayers{edgelayer,nodelayer,main}\n\n";

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
