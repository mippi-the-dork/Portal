# Legacy Migration and Repair — Design

**Portal v0.12.0 · UE 5.8.x · Win64 · Blueprint Editor**

## Why older assets may change on load

<figure class="doc-media" data-kind="image" data-media-id="migration-before-after">
  <img src="../../assets/media-placeholder.png" alt="Legacy Portal data is converted into native reroute nodes while preserving compatible Blueprint connections." loading="lazy">
  <figcaption>Legacy Portal data is converted into native reroute nodes while preserving compatible Blueprint connections.<span class="media-capture-instructions"><strong>Capture instructions · Image:</strong> Create a diagram showing historical custom Portal nodes migrating to current native UK2Node_Knot endpoints, with metadata and existing wires retained. Include a note that migrated assets must be compiled and saved.</span></figcaption>
</figure>

Portal's native-safe architecture was introduced in a historical **0.4.1** milestone. Older Blueprints might contain Portal-specific Declaration/Usage node classes. Current Portal code includes a migration reader that replaces them with native reroutes and transfers their connections and labels.

## Before migrating a project

Use source control or back up representative Blueprint assets. Open a copy of a Blueprint using older Portals, allow editor repair to run, then inspect the graph, compile, and save. Compare the result with the previous version before migrating a large asset library.

## What you should see

Current Inputs and Outputs continue to look like Portal nodes while Portal is enabled, but the underlying serialized graph uses native reroutes. The Blueprint may become dirty because migration changes stored node classes and connections.

## Repair without old versions

The editor also checks metadata and physical links to repair family identity, names and some copy/paste conflicts. This is a safety mechanism, not a guarantee that every malformed or disconnected graph can be reconstructed without human review.

## Important historical scope

The available architecture note confirms the **0.4.1** transition. Other intermediate feature history has not been reconstructed from release notes; the public roadmap should not invent it.

For native implementation and engine boundaries, [read Technical](TECHNICAL.md).
