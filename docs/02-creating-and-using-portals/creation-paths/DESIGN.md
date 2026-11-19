# Creating Portals from Menus and Pins — Design

**Portal v0.12.0 · UE 5.8.x · Win64 · Blueprint Editor**

## Use the creation method that fits the task

<figure class="doc-media" data-kind="gif" data-media-id="creation-choices">
  <img src="../../assets/media-placeholder.png" alt="Portal supports graph-menu creation as well as context-aware creation directly from compatible data pins." loading="lazy">
  <figcaption>Portal supports graph-menu creation as well as context-aware creation directly from compatible data pins.<span class="media-capture-instructions"><strong>Capture instructions · GIF:</strong> Record three brief sequences: creating an Input by right-clicking empty graph space, creating it from a data output pin, and connecting a Portal Output from an unconnected data input pin. Highlight the chosen menu item each time.</span></figcaption>
</figure>

| Starting point | Action | Result |
| --- | --- | --- |
| Empty Blueprint graph | Right-click → search `Create Portal Reroute Node` | Unconnected Portal Input |
| Compatible data **output** pin | Right-click → **Create Portal** | New Input automatically wired to that pin |
| Unconnected compatible data **input** pin | Right-click → **Connect Portal Output** | New Output automatically wired to that pin |
| Existing Input | Right-click → **Create Output** | Another family Output |
| Existing Output | Right-click → **Create Another Output** | Another Output for the same Input |
| Selected family member | **Shift + R + left click** on graph | Another Output, placed at pointer |
| No selected family | **Shift + R + left click** | New Input at pointer |

## Working without a known consumer

Start with the Input, give it a clear name and create Outputs later. A family count of **0** is expected and useful; it does not indicate a broken node.

## Working from a known consumer

Use the target pin's context menu to see compatible existing families. If the menu is empty, confirm the consumer is an **unconnected data input** and the family belongs to the same Blueprint graph.

## Search behavior

The Blueprint action menu accepts Portal names and common search terms such as `named reroute`, `reroute`, `get`, and `getter`. Output actions are contextual to current graph families, not global across the entire Blueprint project.

For native implementation and engine boundaries, [read Technical](TECHNICAL.md).
