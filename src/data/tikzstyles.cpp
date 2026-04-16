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
#include "mgbPluginManager.h" 

#include <QDebug>
#include <QColorDialog>
#include <QFile>
#include <QFileInfo>
#include <QCoreApplication>

TikzStyles::TikzStyles(QObject *parent) : QObject(parent)
{
    _nodeStyles = new StyleList(false, this);
    _edgeStyles = new StyleList(true, this);

    // The PluginManager now auto-loads itself, so we just call the injector
    injectHardcodedStyles();
}

// =================================================================
// MGB-UML: STYLE INJECTOR (Hardcoded Edges + Dynamic Plugins)
// =================================================================
void TikzStyles::injectHardcodedStyles()
{
    // --- 1. STABLE HARDCODED EDGES (Lines) ---
    // CRITICAL FIX: We explicitly add to _edgeStyles so the internal 
    // sorter doesn't accidentally misclassify custom diamonds as nodes!
    
    if (_edgeStyles->style("Association") == nullptr) {
        GraphElementData *data = new GraphElementData();
        data->setProperty("-", ""); 
        data->setProperty("draw", "black");
        data->setProperty("tikzit category", "UML Edges");
        _edgeStyles->addStyle(new Style("Association", data));
    }
    
    if (_edgeStyles->style("Generalization") == nullptr) {
        GraphElementData *data = new GraphElementData();
        data->setProperty("->", ""); 
        data->setProperty("draw", "black");
        data->setProperty("tikzit category", "UML Edges");
        _edgeStyles->addStyle(new Style("Generalization", data));
    }
    
    if (_edgeStyles->style("Aggregation") == nullptr) {
        GraphElementData *data = new GraphElementData();
        data->setProperty("-{Diamond[open]}", ""); 
        data->setProperty("draw", "black");
        data->setProperty("tikzit category", "UML Edges");
        _edgeStyles->addStyle(new Style("Aggregation", data));
    }
    
    if (_edgeStyles->style("Composition") == nullptr) {
        GraphElementData *data = new GraphElementData();
        data->setProperty("-{Diamond[fill]}", ""); 
        data->setProperty("draw", "black");
        data->setProperty("tikzit category", "UML Edges");
        _edgeStyles->addStyle(new Style("Composition", data));
    }

    // --- 2. INJECT UNIFIED PLUGINS ---
    QList<mgb::PluginElement> plugins = mgb::PluginManager::instance().getLoadedPlugins();
    for (const mgb::PluginElement& p : plugins) {
        
        GraphElementData *data = new GraphElementData();
        QMapIterator<QString, QString> i(p.properties);
        while (i.hasNext()) {
            i.next();
            // SECRET EXCLUSION: Do not put raw LaTeX math into the visual UI property list
            if (i.key() == "tikz_libraries" || i.key() == "latex_preamble") {
                continue; 
            }
            data->setProperty(i.key(), i.value());
        }
        data->setProperty("tikzit category", p.category);

        if (p.type == "edge") {
            if (_edgeStyles->style(p.name) == nullptr) {
                _edgeStyles->addStyle(new Style(p.name, data));
            }
        } else {
            if (_nodeStyles->style(p.name) == nullptr) {
                _nodeStyles->addStyle(new Style(p.name, data));
            }
        }
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

        QString cleanTikz;
        QTextStream stream(&styleTikz);
        QList<mgb::PluginElement> plugins = mgb::PluginManager::instance().getLoadedPlugins();

        while (!stream.atEnd()) {
            QString line = stream.readLine();
            
            if (line.startsWith("\\usetikzlibrary") || 
                line.startsWith("\\pgfkeys") || 
                line.startsWith("\\pgfdeclarelayer") || 
                line.startsWith("\\pgfsetlayers") ||
                line.startsWith("%") ) { 
                continue; 
            }
            
            // Auto-Heal: Protect Hardcoded Edge Styles
            if (line.startsWith("\\tikzstyle{Association}=") || 
                line.startsWith("\\tikzstyle{Generalization}=") ||
                line.startsWith("\\tikzstyle{Aggregation}=") ||
                line.startsWith("\\tikzstyle{Composition}=")) {
                continue; 
            }

            // Auto-Heal: Dynamically protect loaded Plugins
            bool isPluginStyle = false;
            for (const mgb::PluginElement& p : plugins) {
                if (line.startsWith("\\tikzstyle{" + p.name + "}=")) {
                    isPluginStyle = true;
                    break;
                }
            }
            if (isPluginStyle) continue;

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
    // Handled in stylelist.cpp
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
    QStringList list;
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
    code << "% Do NOT modify the properties of core or plugin elements directly.\n";
    code << "% The app will automatically reset them to default to protect the palette.\n";
    code << "% \n";
    code << "% If you want a custom color or style, COPY the line and RENAME it.\n";
    code << "% =========================================================\n\n";

    // --- MGB-UML: DYNAMIC LIBRARY AND PREAMBLE EXTRACTOR ---
    QStringList libraries = {"shapes.multipart", "positioning", "shapes.geometric", "arrows.meta"};
    QString customPreambles = "";

    QList<mgb::PluginElement> plugins = mgb::PluginManager::instance().getLoadedPlugins();
    for (const mgb::PluginElement& p : plugins) {
        
        // 1. Extract custom libraries
        if (p.properties.contains("tikz_libraries")) {
            QStringList libs = p.properties.value("tikz_libraries").split(",");
            for (QString l : libs) {
                if (!libraries.contains(l.trimmed())) {
                    libraries << l.trimmed();
                }
            }
        }
        
        // 2. Extract custom \pgfdeclareshape macros
        if (p.properties.contains("latex_preamble")) {
            customPreambles += "% Preamble definitions for " + p.name + "\n";
            customPreambles += p.properties.value("latex_preamble") + "\n\n";
        }
    }

    code << "% Required TikZ Libraries\n";
    code << "\\usetikzlibrary{" << libraries.join(", ") << "}\n\n";

    if (!customPreambles.isEmpty()) {
        code << "% Custom Plugin Shapes & Preambles\n";
        code << customPreambles;
    }
    // --------------------------------------------------------

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