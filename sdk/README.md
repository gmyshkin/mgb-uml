# MGB-UML Plugin SDK

This SDK is for building third-party MGB-UML plugins.

Plugins are Qt plugins that implement `mgb::ElementPluginInterface`. A plugin can provide:

- Palette metadata, such as name, category, icon, tooltip, and TikZ style properties.
- A custom Qt node item for app-side drawing.
- Optional custom TikZ export so the compiled LaTeX output matches the app rendering.

## SDK Layout

Packaged SDK archives contain:

- `include/` - MGB-UML headers needed to compile plugins.
- `templates/basic-node-plugin/` - a small working plugin template.
- `docs/PLUGIN_DEVELOPMENT.md` - step-by-step plugin development guide.

## Requirements

- Qt 6 development tools, including `qmake6`, `moc`, and C++ compiler support.
- A copy of MGB-UML built for the same OS and Qt major version.

Plugins are binary artifacts. Build Linux plugins on Linux, Windows plugins on Windows, and macOS plugins on macOS.

## Quick Start

1. Copy `templates/basic-node-plugin` somewhere outside the SDK.
2. Set `MGB_UML_SDK` to the SDK root.
3. Run:

```sh
qmake6
make
```

On Windows, use the Qt developer command prompt and run the equivalent `qmake6` and `nmake`/`jom`/`mingw32-make` flow for your Qt kit.

4. Copy the generated plugin library into the MGB-UML `plugins` directory next to the app executable.
5. Restart MGB-UML.

## Important Interface

Every compiled plugin implements:

```cpp
class MyPlugin : public QObject, public mgb::ElementPluginInterface
```

The most important methods are:

- `getElements()` - returns palette/style metadata.
- `createCustomNode(Node *node)` - returns a custom `NodeItem` for app rendering, or `nullptr`.
- `writeTikzNode(QTextStream &code, Node *node, int *emittedLines)` - optionally writes custom TikZ for export.

Use `writeTikzNode` whenever your Qt rendering is more complex than a standard TikZ node. This prevents the app and LaTeX compiler from drifting apart.
