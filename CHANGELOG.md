# Changelog

This changelog tracks MGB-UML releases. TikZiT is the upstream project that
MGB-UML is based on, so TikZiT itself is treated as the baseline rather than an
MGB-UML release.

## v1.1.1 - Release SDK and updater fixes

### Changed

- Updated the Manual Build & Release workflow so publishing a release also
  packages the Linux, Windows, and macOS Plugin SDK archives and uploads them
  to the same GitHub Release.
- Improved the automatic update checker to normalize version tags with or
  without a leading `v` before comparing versions.
- Updated update-checker wording from TikZiT to MGB-UML.

### Fixed

- Fixed SDK archives not being attached when a GitHub Release is created by the
  manual release workflow.
- Fixed false update notifications when the installed app version is tagged with
  a leading `v`, such as `v1.1.0`, and GitHub reports `1.1.0` after
  normalization.

## v1.1.0 - Plugin architecture, UML plugins, and release polish

### Added

- Added a compiled plugin architecture for custom MGB-UML elements.
- Added the Plugins menu with plugin installation and plugin management actions.
- Added built-in UML plugins for:
  - UML Actor
  - UML Class
  - UML System
  - UML Use Case
- Added plugin-backed palette metadata, icons, tooltips, categories, and default
  TikZ style properties.
- Added custom Qt rendering for UML plugin nodes so the editor view matches UML
  shapes more closely.
- Added custom TikZ export hooks for plugin nodes so LaTeX output can match the
  editor rendering.
- Added plugin geometry hints for edge attachment around custom node shapes.
- Added the MGB-UML Plugin SDK, including headers, documentation, geometry
  guidance, and a basic node plugin template.
- Added the plugin SDK GitHub Actions workflow.
- Added a Beginner Guide entry under Help.
- Added developer and upstream TikZiT attribution in the About dialog.
- Added direct keyboard handling for Cut, Copy, and Paste in the diagram scene.
- Added a regression test for parser-friendly editable TikZ serialization of
  UML clipboard content.
- Added changelog entry to Help tab.

### Changed

- Changed UML element creation to use drag-and-drop style metadata instead of
  hard-coded palette tool cases.
- Changed graph export so normal file/export output can use plugin custom TikZ,
  while clipboard copy can request editable parser-friendly TikZ nodes.
- Improved edge routing and anchor calculation for rectangle, ellipse, actor,
  system, class, and other plugin-defined shapes.
- Improved UML Class, UML System, UML Actor, and UML Use Case sizing and label
  handling in both the editor and TikZ export.
- Updated Windows, macOS, and Linux release workflows to build plugins as part
  of the release process.
- Changed Windows workflow artifact packaging so the downloadable workflow
  artifact extracts directly to the app folder instead of containing another zip
  file.
- Changed the release publish job to create the Windows release zip only at the
  GitHub Release publishing step.
- Exported shared core API symbols needed by Windows plugins, including
  `floatToString`.
- Changed so automatic update checker URL points to MGB-UML instead of TikZiT.


### Fixed

- Fixed Copy, Cut, and Paste for UML elements by copying editable TikZ node
  data instead of plugin-rendered low-level TikZ drawing commands.
- Fixed Cut so copied content is placed on the clipboard before selected items
  are removed.
- Fixed pasted path data being omitted when inserting copied graph fragments.
- Fixed pasted UML selections not being visibly reselected after paste.
- Fixed Windows release plugin linking for plugins that use shared utility
  functions from the core app.
- Fixed packaged Windows builds to include custom plugins and remove compiler
  scratch files from the plugin folder.

## v1.0.0 - Initial MGB-UML release

### Added

- Forked TikZiT into MGB-UML as a UML-focused PGF/TikZ diagram editor.
- Rebranded the application name, executable, icons, update links, and release
  metadata for MGB-UML.
- Added an MGB-UML palette for UML-oriented diagram creation.
- Added initial UML element support, including UML classes and use cases.
- Added UML-oriented styling support through injected TikZ styles.
- Added UML edge styling support, including generalization, aggregation, and
  composition-style edge behavior.
- Added MGB-UML-specific file management helpers.
- Added bring-to-front and send-to-back ordering for overlapping diagram
  elements.
- Added release automation for Windows, macOS, and Linux builds.
- Added generated API documentation and documentation publishing workflow.
- Added Docker-related development and deployment files.
- Added project version metadata.

### Changed

- Updated the build from the upstream TikZiT baseline for the MGB-UML target.
- Updated editor behavior and palette flow around UML diagram workflows.
- Updated PDF/TikZ handling and style generation for MGB-UML diagrams.
- Updated menu text, app metadata, and repository links for the MGB-UML project.

### Fixed

- Fixed early MGB-UML packaging paths for the renamed executable.
- Fixed release scripts to package MGB-UML assets instead of the original
  TikZiT executable naming.
