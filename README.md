# MGB-UML

MGB-UML is a desktop diagram editor for creating UML-style diagrams that export
to PGF/TikZ. It is based on TikZiT, but focuses the editor, palette, styling,
and release packaging around UML workflows.

The app keeps TikZiT's editable `.tikz` document model while adding UML-focused
tools such as classes, systems, actors, use cases, and UML edge styles. Diagrams
can be edited visually, copied and pasted as editable graph fragments, and
exported for LaTeX documents.

## Features

- Visual editing for TikZ-backed diagrams.
- UML palette entries for common UML elements.
- UML edge styles including generalization, aggregation, and composition.
- Plugin support for custom diagram elements.
- Plugin-aware TikZ export so app rendering and LaTeX output stay aligned.
- Plugin SDK with headers, documentation, and a basic node plugin template.
- Cross-platform Qt application packaging for Linux, Windows, and macOS.

## Repository Layout

- `src/` - application source code.
- `plugins_source/` - built-in plugin source code.
- `plugins/` - runtime plugin directory used by local builds.
- `sdk/` - plugin SDK, documentation, and templates.
- `share/` - packaged shared resources.
- `tests/` - test and deployment test documentation.
- `images/` - application icons and image assets.

## Requirements

MGB-UML is a Qt Widgets application. For local development you need:

- C++ compiler with C++17 support.
- Qt with Core, Gui, Widgets, Network, Pdf, and PdfWidgets modules.
- flex and bison.
- Poppler development libraries where PDF preview/export support requires them.
- make or another build tool supported by your Qt installation.

The qmake project file is currently the primary build entry point.

## Building on Linux

On Ubuntu-like systems, install the usual compiler and Qt development packages,
plus flex, bison, and Poppler:

```sh
sudo apt install build-essential qtbase5-dev qtpdf5-dev flex bison \
  libpoppler-dev libpoppler-cpp-dev libgl1-mesa-dev
```

Then build the app:

```sh
qmake -r tikzit.pro
make
```

The local executable is named `mgb-uml`.

To create a portable Linux package, run:

```sh
./deploy-linux.sh
```

## Building on Windows

Install Qt for Windows with a compatible compiler, then install flex and bison.
WinFlexBison is the usual option on Windows; make sure the tools are available
on your `Path`.

From a Qt command prompt:

```bat
qmake -r tikzit.pro
mingw32-make
```

To create a portable Windows folder, run:

```bat
deploy-win.bat
```

## Building on macOS

Install Qt, developer tools, flex, bison, and Poppler. With Homebrew, the exact
package names depend on the Qt version you are using, but a typical setup is:

```sh
brew install qt flex bison poppler
```

Make sure Qt's `bin` directory is on your `PATH`, then build:

```sh
qmake -r tikzit.pro
make
```

To bundle the app, run:

```sh
./deploy-osx.sh
```

## Tests

The qmake project includes a test configuration:

```sh
qmake -config test tikzit.pro
make
./UnitTests
```

## Plugins and SDK

MGB-UML supports compiled Qt plugins for custom diagram elements. Plugins can
add palette entries, custom editor rendering, geometry hints for edge attachment,
and custom TikZ output.

Start with the SDK docs:

- `sdk/README.md`
- `sdk/docs/PLUGIN_DEVELOPMENT.md`
- `sdk/PLUGIN_GEOMETRY_HINTS.md`
- `sdk/templates/basic-node-plugin/`

Compiled plugins should be copied into the `plugins` directory next to the app
executable, then the app should be restarted.

## Attribution

MGB-UML is based on TikZiT, a graphical tool for rapidly creating graphs and
string diagrams using PGF/TikZ. The original TikZiT project remains the upstream
foundation for the editor and TikZ document model.

## License

This project follows the license included in `COPYING`.
