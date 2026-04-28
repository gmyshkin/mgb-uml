# Plugin Development Guide

## Concept

MGB-UML has two renderers:

- The app renderer, which uses Qt graphics items.
- The LaTeX renderer, which uses exported TikZ code.

If a plugin only defines TikZ style properties, MGB-UML can export it as a normal `\node`. That is enough for simple rectangles, circles, and labels.

If a plugin draws a custom Qt item, it should usually also implement `writeTikzNode(...)`. Otherwise the app and LaTeX compiler may disagree about font metrics, anchors, text wrapping, shape size, or internal layout.

## Minimal Plugin Responsibilities

Your plugin must:

1. Include `mgbPluginInterface.h`.
2. Inherit from `QObject` and `mgb::ElementPluginInterface`.
3. Use `Q_PLUGIN_METADATA`.
4. Use `Q_INTERFACES(mgb::ElementPluginInterface)`.
5. Return at least one `mgb::PluginElement` from `getElements()`.

## PluginElement Metadata

`PluginElement` describes what appears in the palette and what style is written to `.tikzstyles`.

Common fields:

```cpp
mgb::PluginElement e;
e.name = "Example Box";
e.type = "node";
e.category = "Examples";
e.tooltip = "Drag to add an example box";
e.iconPath = ":/icons/example.svg";
```

Common TikZ properties:

```cpp
e.properties.insert("shape", "rectangle");
e.properties.insert("draw", "black");
e.properties.insert("fill", "white");
e.properties.insert("minimum width", "3cm");
e.properties.insert("minimum height", "1.5cm");
e.properties.insert("font", "\\sffamily\\fontsize{10pt}{12pt}\\selectfont");
e.properties.insert("line width", "0.6pt");
e.properties.insert("inner sep", "0pt");
e.properties.insert("outer sep", "0pt");
```

Special properties:

- `tikz_libraries` - comma-separated TikZ libraries needed by the style.
- `latex_preamble` - custom LaTeX/TikZ definitions injected into `.tikzstyles`.

## Custom App Rendering

Return a custom `NodeItem` from `createCustomNode` when you need app-side drawing beyond the default renderer.

```cpp
NodeItem *MyPlugin::createCustomNode(Node *node) const
{
    if (node->styleName() == "Example Box") {
        return new ExampleBoxItem(node);
    }
    return nullptr;
}
```

## Custom TikZ Export

Implement `writeTikzNode` when your Qt item has custom sizing or layout.

```cpp
bool MyPlugin::writeTikzNode(QTextStream &code, Node *node, int *emittedLines) const
{
    if (node->styleName() != "Example Box") {
        if (emittedLines) *emittedLines = 0;
        return false;
    }

    code << "\\t\\t\\node [draw, rectangle] ("
         << node->name() << ") at ("
         << node->point().x() << ", "
         << node->point().y() << ") {"
         << node->label() << "};\\n";

    if (emittedLines) *emittedLines = 1;
    return true;
}
```

`emittedLines` must match how many TikZ lines you wrote. MGB-UML uses this to keep source-line selection useful.

## Hitboxes

Qt uses `NodeItem::shape()` for picking, selection, and dragging. If your visual drawing is thin, such as a stick figure, use a wider invisible path in `shape()`, but do not draw that path in `paint()`.

Pattern:

```cpp
QPainterPath MyItem::shape() const
{
    QPainterPath visual = visualPath();
    QPainterPathStroker stroker;
    stroker.setWidth(12);
    return stroker.createStroke(visual).united(visual);
}

void MyItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    painter->drawPath(visualPath());
}
```

## Installing A Plugin

Copy the compiled plugin library into the MGB-UML `plugins` directory next to the app executable:

- Linux: `plugins/libYourPlugin.so`
- Windows: `plugins/YourPlugin.dll`
- macOS: `plugins/libYourPlugin.dylib`

Restart MGB-UML after installing or replacing plugins.
