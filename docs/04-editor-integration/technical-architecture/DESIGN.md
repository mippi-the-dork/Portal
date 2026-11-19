# Editor Module and Data Architecture — Design

**Portal v0.12.0 · UE 5.8.x · Win64 · Blueprint Editor**

## The components you actually interact with

<figure class="doc-media" data-kind="image" data-media-id="architecture-diagram">
  <img src="../../assets/media-placeholder.png" alt="Portal organizes editor interaction around native Blueprint reroutes and a separate presentation layer." loading="lazy">
  <figcaption>Portal organizes editor interaction around native Blueprint reroutes and a separate presentation layer.<span class="media-capture-instructions"><strong>Capture instructions · Image:</strong> Make a labeled technical diagram with FPortalModule at the center, connected to UPortalNodeSpawner, ToolMenus, FPortalInputProcessor, FPortalNodeFactory/SPortalNativeKnotNode, and PortalNativeUtils. Below, show native UK2Node_Knot plus metadata as the saved graph state.</span></figcaption>
</figure>

Portal has three main user touchpoints: Blueprint action/pin menus for creation; compact named graph nodes for viewing and editing; and keyboard/mouse shortcuts for fast placement. The same native Blueprint graph remains responsible for saved connectivity and compilation.

## Behind the interface

A small editor module registers menus, a custom node drawing factory, a Slate input processor, and repair/update callbacks. Internal utilities manage metadata, graph-local identity and native reroute links.

## Why this separation is important

A native reroute is the saved and compiled form; the visual Portal widget is an editor convenience. Removing the editor extension does not remove the native node type, though it does expose the wire and remove the special label display.

## What the extension does not provide

It does not create cross-graph data stores, runtime Blueprint functions, a network layer, execution-flow branches, or an engine replacement. Its applicability is intentionally narrow: compatible data pins in Blueprint graphs.

For native implementation and engine boundaries, [read Technical](TECHNICAL.md).
