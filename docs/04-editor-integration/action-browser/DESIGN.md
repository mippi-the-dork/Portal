# Blueprint Action Browser and Context Menus — Design

**Portal v0.12.0 · UE 5.8.x · Win64 · Blueprint Editor**

## Search by what you want to do

<figure class="doc-media" data-kind="image" data-media-id="action-browser">
  <img src="../../assets/media-placeholder.png" alt="Blueprint action search exposes the Input creation command and Output actions for families in the current graph." loading="lazy">
  <figcaption>Blueprint action search exposes the Input creation command and Output actions for families in the current graph.<span class="media-capture-instructions"><strong>Capture instructions · Image:</strong> Capture the right-click Blueprint action search showing Create Portal Reroute Node and several Portal Output entries for actual Inputs in the active graph. Keep the search term, family name and type/count context readable.</span></figcaption>
</figure>

Right-click empty space in a Blueprint graph and search `Portal` to find **Create Portal Reroute Node**. When the current graph has Portal Inputs, look under **Portal → Outputs** for entries named **Portal Output: [Portal Name]**.

The menu also accepts the family name and related search terms including `named reroute`, `reroute`, `get`, and `getter`.

## What the Output entries show

Output actions are associated with Inputs from the current graph. Their display can include family names, pin type information and current Output counts, making it easier to select a suitable route.

## What the menu won't do

A Portal Input in Event Graph will not create a matching Output action inside an unrelated function or Blueprint. A selected source pin and a selected target pin expose different contextual commands.

## If an action is missing

Check that you're in an editable K2 graph, the family Input exists in that graph, and there is a compatible data type. Try right-clicking the intended pin directly if the goal is to connect a source or consumer.

For native implementation and engine boundaries, [read Technical](TECHNICAL.md).
