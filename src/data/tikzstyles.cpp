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
#include <QStringList>

namespace {

QString sanitizeStyleSource(const QString &styleTikz)
{
    QStringList styleLines;
    const QStringList lines = styleTikz.split('\n');

    for (const QString &line : lines) {
        const QString trimmed = line.trimmed();
        if (trimmed.startsWith("\\tikzstyle")) {
            styleLines << trimmed;
        }
    }

    if (!styleLines.isEmpty()) {
        return styleLines.join("\n") + "\n";
    }

    return styleTikz;
}

void applyProtectedStyle(StyleList *list, const QString &name, GraphElementData *defaults)
{
    Style *existing = list->style(name);
    if (existing == nullptr) {
        list->addStyle(new Style(name, defaults));
        return;
    }

    GraphElementData *target = existing->data();
    const QVector<GraphElementProperty> currentProps = target->properties();
    for (const GraphElementProperty &prop : currentProps) {
        if (prop.atom()) target->unsetAtom(prop.key());
        else target->unsetProperty(prop.key());
    }

    const QVector<GraphElementProperty> defaultProps = defaults->properties();
    for (const GraphElementProperty &prop : defaultProps) {
        if (prop.atom()) target->setAtom(prop.key());
        else target->setProperty(prop.key(), prop.value());
    }

    delete defaults;
}

}

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
     {
        GraphElementData *data = new GraphElementData();
        data->setAtom("->");
        data->setProperty("draw", "black");
        data->setProperty("line width", "0.6pt");
        data->setProperty("tikzit category", "UML Edges");
        applyProtectedStyle(_edgeStyles, "Association", data);
    }

    {
        GraphElementData *data = new GraphElementData();
        data->setAtom("uml-generalization");
        data->setProperty("draw", "black");
        data->setProperty("line width", "0.6pt");
        data->setProperty("tikzit category", "UML Edges");
        applyProtectedStyle(_edgeStyles, "Generalization", data);
    }

    {
        GraphElementData *data = new GraphElementData();
        data->setAtom("uml-aggregation");
        data->setProperty("draw", "black");
        data->setProperty("line width", "0.6pt");
        data->setProperty("tikzit category", "UML Edges");
        applyProtectedStyle(_edgeStyles, "Aggregation", data);
    }

    {
        GraphElementData *data = new GraphElementData();
        data->setAtom("uml-composition");
        data->setProperty("draw", "black");
        data->setProperty("line width", "0.6pt");
        data->setProperty("tikzit category", "UML Edges");
        applyProtectedStyle(_edgeStyles, "Composition", data);
    }

    QList<mgb::PluginElement> plugins = mgb::PluginManager::instance().getLoadedPlugins();
    for (const mgb::PluginElement &p : plugins) {
        GraphElementData *data = new GraphElementData();
        QMapIterator<QString, QString> i(p.properties);

        while (i.hasNext()) {
            i.next();
            if (i.key() == "tikz_libraries" || i.key() == "latex_preamble") {
                continue;
            }
            data->setProperty(i.key(), i.value());
        }

        data->setProperty("tikzit category", p.category);

        if (p.type == "edge") {
            applyProtectedStyle(_edgeStyles, p.name, data);
        } else {
            applyProtectedStyle(_nodeStyles, p.name, data);
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
        QString styleTikz = sanitizeStyleSource(in.readAll());
        file.close();

        clear();
        TikzAssembler ass(this);
        bool success = ass.parse(styleTikz);
        
        if (success) {
            injectHardcodedStyles();
            // Persist the canonicalized style file so external LaTeX compilers
            // see the same plugin-backed styles the app is rendering with.
            saveStyles(fileName);
            return true;
        } else {
            injectHardcodedStyles();
            return false;
        }
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

void TikzStyles::refreshModels(QStandardItemModel *nodeModel,
                               QStandardItemModel *edgeModel,
                               QString category,
                               bool includeNone)
{
    (void)nodeModel;
    (void)edgeModel;
    (void)category;
    (void)includeNone;
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
    code << "% Match app font metrics more closely in external LaTeX compilers\n";
    code << "\\IfFileExists{helvet.sty}{\\usepackage[scaled=1.0]{helvet}}{}\n";
    code << "% Match the app's default visible canvas scale more closely\n";
    code << "\\tikzset{every picture/.style={baseline=-0.25em,scale=1,transform shape}}\n\n";

    if (!customPreambles.isEmpty()) {
        code << "% Custom Plugin Shapes & Preambles\n";
        code << customPreambles;
    }
    // --------------------------------------------------------

    code << "% Ignore UI-specific keys\n";
    code << "\\pgfkeys{/tikz/tikzit fill/.initial=}\n";
    code << "\\pgfkeys{/tikz/tikzit draw/.initial=}\n";
    code << "\\pgfkeys{/tikz/tikzit shape/.initial=}\n";
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
