# Node Presentation and Editing — Design

**Portal v0.12.0 · UE 5.8.x · Win64 · Blueprint Editor**

## A compact mirrored design

<figure class="doc-media" data-kind="image" data-media-id="node-ui-anatomy">
  <img src="../../assets/media-placeholder.png" alt="Mirrored node layouts and family counts communicate Portal direction and usage at a glance." loading="lazy">
  <figcaption>Mirrored node layouts and family counts communicate Portal direction and usage at a glance.<span class="media-capture-instructions"><strong>Capture instructions · Image:</strong> Capture close-up Input and Output nodes showing pin, icon, name and count positions. Include one Input with count 0 and another family with multiple Outputs for comparison.</span></figcaption>
</figure>

Input and Output nodes are intentionally symmetrical: **Input** is `pin | icon | name | count`, while **Output** is `count | name | icon | pin`. This helps identify the direction of data routing from the shape alone.

## Labels and tooltips

Both endpoints show their family name. Inputs display an explicit **0** with no Outputs. Hover a node to inspect its tooltip for family information. The node keeps a consistent height during inline rename.

## Rename from the Input

Use the Input as the family authority for naming. Outputs use the same family presentation rather than owning unrelated names. If an Output displays an unexpected label, inspect family membership and repair state.

## Display is not the data model

Portal suppresses the bridge wire visually while installed, but the internal native pins still exist. If you remove Portal, their ordinary reroute appearance returns. This keeps visual convenience separate from serialized graph connectivity.

## Current UI scope

These widgets are Blueprint graph editor Slate nodes, not UMG widgets, Blueprint runtime classes, or standalone Content Browser assets.

For native implementation and engine boundaries, [read Technical](TECHNICAL.md).
