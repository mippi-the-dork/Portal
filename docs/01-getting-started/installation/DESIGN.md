# Installation and Compatibility — Design

**Portal v0.12.0 · UE 5.8.x · Win64 · Blueprint Editor**

## Supported environment

<figure class="doc-media" data-kind="image" data-media-id="installation-folder">
  <img src="../../assets/media-placeholder.png" alt="A project installation keeps the Portal descriptor and source files together under the Plugins directory." loading="lazy">
  <figcaption>A project installation keeps the Portal descriptor and source files together under the Plugins directory.<span class="media-capture-instructions"><strong>Capture instructions · Image:</strong> Capture File Explorer at YourProject/Plugins/Portal with Portal.uplugin, Source, Resources, README.md and Doc clearly visible. Include the path bar.</span></figcaption>
</figure>

Portal 0.12.0 targets Unreal Engine **5.8.0 through 5.8.3**, **Win64**, and Blueprint graph editing. The plugin descriptor calls the module `UncookedOnly` and marks it beta. This is not a packaged-game feature and does not modify Unreal Engine source.

## Install from a compatible package

1. Close Unreal Editor.
2. Place the **Portal** plugin folder under `YourProject/Plugins/Portal/` (or an appropriate Engine Plugins location).
3. Check that `Portal.uplugin` is directly inside that folder.
4. Open the project, enable Portal in **Edit → Plugins** if needed, and restart when prompted.
5. Open a Blueprint graph, right-click empty space and search for **Portal**.

## Building from source

Clone the Portal repository into the plugin folder, regenerate project files if needed, and build the UE Editor target with the engine's Windows C++ toolchain. Keep the entire Source tree and descriptor together.

## Before opening older assets

Save a backup or use source control. Portal includes a migration path for older custom Portal node types; loading and saving an asset can change how its nodes are serialized. Confirm the migrated Blueprint compiles and its data paths still work.

## If Portal actions don't appear

Confirm plugin enablement, engine version, graph type, and that the graph is editable. Portal currently targets **K2/Blueprint graphs**, not Material, Niagara, MetaSound, or arbitrary `UEdGraph` editors.

For native implementation and engine boundaries, [read Technical](TECHNICAL.md).
