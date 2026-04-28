# Basic Node Plugin Template

This is a minimal compiled plugin for MGB-UML. It demonstrates:

- Palette metadata via `PluginElement`.
- Custom Qt drawing via `NodeItem`.
- Matching custom TikZ export via `writeTikzNode`.

## Build

Set `MGB_UML_SDK` to the SDK root, then run:

```sh
qmake6
make
```

The generated library can be copied into the MGB-UML `plugins` directory.
