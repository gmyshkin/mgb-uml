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

#include "toolpalette.h"

#include <QVector>
#include <QLayout>
#include <QVBoxLayout>
#include <QDebug>

ToolPalette::ToolPalette(QWidget *parent) :
    QToolBar(parent)
{
    setWindowFlags(Qt::Window
                   | Qt::CustomizeWindowHint
                   | Qt::WindowDoesNotAcceptFocus);
    setOrientation(Qt::Vertical);
    setFocusPolicy(Qt::NoFocus);
    setWindowTitle("Tools");
    setObjectName("toolPalette");
    //setGeometry(100,200,30,195);

    tools  = new QActionGroup(this);

    select = new QAction("Select (s)", this);
    select->setIcon(QIcon(":/images/tikzit-tool-select.svg"));
    
    vertex = new QAction("Add Vertex (v)", this);
    vertex->setIcon(QIcon(":/images/tikzit-tool-node.svg"));
    
    edge   = new QAction("Add Edge (e)", this);
    edge->setIcon(QIcon(":/images/tikzit-tool-edge.svg"));

    // --- MGB-UML NEW TOOLS START ---
    umlUseCase = new QAction("Add Use Case", this);
    umlUseCase->setIcon(QIcon(":/images/tikzit-tool-node.svg")); // TODO: Replace with custom UML icon later

    umlClass = new QAction("Add Class", this);
    umlClass->setIcon(QIcon(":/images/tikzit-tool-node.svg"));   // TODO: Replace with custom UML icon later
    // --- MGB-UML NEW TOOLS END ---


    tools->addAction(select);
    tools->addAction(vertex);
    tools->addAction(edge);
    tools->addAction(umlUseCase); // ADDED
    tools->addAction(umlClass);   // ADDED

    select->setCheckable(true);
    vertex->setCheckable(true);
    edge->setCheckable(true);
    umlUseCase->setCheckable(true); // ADDED
    umlClass->setCheckable(true);   // ADDED
    
    select->setChecked(true);

    addAction(select);
    addAction(vertex);
    addAction(edge);
    
    // Add a visual separator before the UML tools
    addSeparator(); 
    
    addAction(umlUseCase); // ADDED
    addAction(umlClass);   // ADDED
}

ToolPalette::Tool ToolPalette::currentTool() const
{
    QAction *a = tools->checkedAction();
    if (a == vertex) return VERTEX;
    else if (a == edge) return EDGE;
    else if (a == crop) return CROP;
    // --- MGB-UML NEW TOOLS START ---
    else if (a == umlUseCase) return UML_USE_CASE;
    else if (a == umlClass) return UML_CLASS;
    // --- MGB-UML NEW TOOLS END ---
    else return SELECT;
}

void ToolPalette::setCurrentTool(ToolPalette::Tool tool)
{
    switch(tool) {
    case SELECT:
        select->setChecked(true);
        break;
    case VERTEX:
        vertex->setChecked(true);
        break;
    case EDGE:
        edge->setChecked(true);
        break;
    case CROP:
        /* crop->setChecked(true); */
        break;
    // --- MGB-UML NEW TOOLS START ---
    case UML_USE_CASE:
        umlUseCase->setChecked(true);
        break;
    case UML_CLASS:
        umlClass->setChecked(true);
        break;
    // --- MGB-UML NEW TOOLS END ---
    }
}