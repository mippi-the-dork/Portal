# Your First Portal Family — Design

**Portal v0.12.0 · UE 5.8.x · Win64 · Blueprint Editor**

## A five-minute example

<figure class="doc-media" data-kind="gif" data-media-id="first-portal">
  <img src="../../assets/media-placeholder.png" alt="Creating a Portal from a pin establishes a named Input and a connected Output in the same Blueprint graph." loading="lazy">
  <figcaption>Creating a Portal from a pin establishes a named Input and a connected Output in the same Blueprint graph.<span class="media-capture-instructions"><strong>Capture instructions · GIF:</strong> Record creating a Portal from a float output pin, renaming its Input, connecting an Output from a distant compatible consumer pin, and double-clicking that Output to jump back. Keep each context menu readable.</span></figcaption>
</figure>

1. Open an editable **Blueprint Event Graph** or function graph.
2. Add a node that emits a **data output** (for example, a float or vector).
3. Right-click that output pin and choose **Create Portal**. Portal creates an **Input** connected to that data pin.
4. Rename the Input to something meaningful, such as `Movement Speed`.
5. Right-click an unconnected compatible **data input** elsewhere in the *same graph*. Choose **Connect Portal Output**, then select your named Input.
6. Inspect the new Output and verify the Input's Output count changes to **1**.
7. Double-click the Output to jump to the Input. Double-click the Input to select its Outputs.
8. Compile and save your Blueprint. Reopen the graph and verify the family still appears correctly.

## What if the pin menu isn't available?

The pin must be data, not execution. Creating from a source requires a compatible output pin; creating at a consumer requires a compatible unconnected input. The existing Portal Input must belong to the same graph and its type must be accepted by the Blueprint schema.

## Alternative creation methods

Right-click empty graph space and search **Create Portal Reroute Node**, or use **Shift + R + left click** on an editable graph. With exactly one valid Portal family member selected, the shortcut creates another Output for that family instead of a new Input.

## Before relying on the family

Portal labels simplify the drawing, but the real connection still exists as native reroute wiring. If a graph becomes confusing, use **Select Portal Family** or convert the family to ordinary reroutes.

For native implementation and engine boundaries, [read Technical](TECHNICAL.md).
