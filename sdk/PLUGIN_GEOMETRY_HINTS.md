# Plugin Geometry Hints

MGB-UML plugins can describe how edges should attach to custom node shapes by
setting the `tikzit edge shape` property on their plugin element.

This keeps shape-specific edge behavior inside the plugin instead of hardcoding
plugin names in the main application.

## Supported hints

```cpp
e.properties.insert("tikzit edge shape", "rectangle");
e.properties.insert("tikzit edge width", "5.0cm");
e.properties.insert("tikzit edge height", "6.0cm");
```

Use this for box-like elements, including custom rectangular nodes and
multi-section nodes.

This is also useful for custom drawn symbols that should behave as if they have
an outer interaction box for edge attachment, such as stick-figure actors.

```cpp
e.properties.insert("tikzit edge shape", "ellipse");
e.properties.insert("tikzit edge width", "2.8cm");
e.properties.insert("tikzit edge height", "1.1cm");
```

Use this for oval or circular elements.

## Example

```cpp
mgb::PluginElement e;
e.name = "My Custom Node";
e.type = "node";
e.category = "My Plugin";
e.properties.insert("shape", "my custom tikz shape");
e.properties.insert("tikzit edge shape", "rectangle");
```

The `shape` property controls the TikZ/App visual style. The `tikzit edge shape`
property tells MGB-UML which generic boundary math to use when drawing lines and
arrowheads in the editor.

The optional `tikzit edge width` and `tikzit edge height` properties are
editor-only geometry hints. They help the app clip edges to the visible boundary
of custom plugin shapes without changing the LaTeX-rendered size.

Built-in examples:

```cpp
// UML Class and UML System
e.properties.insert("tikzit edge shape", "rectangle");

// UML Use Case
e.properties.insert("tikzit edge shape", "ellipse");

// UML Actor
e.properties.insert("tikzit edge shape", "rectangle");
```

If a plugin does not set this property, MGB-UML falls back to the normal TikZ
shape information when possible.
