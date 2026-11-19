# Frequently Asked Questions — Design

**Portal v0.12.0 · UE 5.8.x · Win64 · Blueprint Editor**

## Does Portal change the runtime game?

<figure class="doc-media" data-kind="image" data-media-id="faq-graph-comparison">
  <img src="../../assets/media-placeholder.png" alt="A Portal is a readable presentation of a normal Blueprint data connection." loading="lazy">
  <figcaption>A Portal is a readable presentation of a normal Blueprint data connection.<span class="media-capture-instructions"><strong>Capture instructions · Image:</strong> Create a simple side-by-side Blueprint screenshot: a healthy named Portal family on the left and its ordinary native reroute equivalent on the right. Keep the wire and data pins legible.</span></figcaption>
</figure>

It adds no runtime subsystem or Portal node class dependency to packaged gameplay. It changes the editor presentation of native Blueprint data reroutes and their metadata.

## Can an Input feed several Outputs?

Yes. The Input shows the number of Outputs in its family, including **0** when none exist.

## Can Portal cross function, macro or Event Graph boundaries?

No. The family belongs to a single Blueprint graph.

## Can it route execution pins?

No. Multi-output execution continuation would not safely preserve native Blueprint flow-control semantics.

## What if I uninstall the plugin?

Current Portal families are stored as native reroutes, so the graph should display ordinary reroute nodes and a visible bridge wire. Compile and verify representative Blueprints after uninstall.

## Is Portal Input the same as a Blueprint input parameter?

No. A Portal Input is a named reroute source inside the graph, not a function parameter or graph interface.

## Can I rename or recolor families?

The family name is managed from its Input. Color is stored in metadata, but the current README does not specify a distinct supported color-picker workflow; do not assume a user-facing color editor exists.

## Why is Portal Browser not documented as a window?

A browser widget exists in C++ source, but no user-accessible registration path was established in the current module. The supported navigation workflows are node gestures and context menus.

## What's the safest way to remove a family?

Use **Delete Portal Family**, or review the three-option Delete prompt on a selected Input with Outputs.

For native implementation and engine boundaries, [read Technical](TECHNICAL.md).
