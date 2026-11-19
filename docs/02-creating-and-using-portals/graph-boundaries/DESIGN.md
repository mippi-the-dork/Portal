# Graph Scope and Type Compatibility — Design

**Portal v0.12.0 · UE 5.8.x · Win64 · Blueprint Editor**

## Portal is graph-local

<figure class="doc-media" data-kind="image" data-media-id="graph-locality">
  <img src="../../assets/media-placeholder.png" alt="Portal families belong to individual Blueprint graphs rather than spanning functions or other graphs." loading="lazy">
  <figcaption>Portal families belong to individual Blueprint graphs rather than spanning functions or other graphs.<span class="media-capture-instructions"><strong>Capture instructions · Image:</strong> Create a labeled two-panel diagram of Event Graph and a function graph. Show a valid Input-to-Output family within Event Graph and a clearly blocked connection across the graph boundary.</span></figcaption>
</figure>

An Input in an **Event Graph** cannot be used as an Output in a **function**, **macro**, or another Blueprint graph. The same names can appear in multiple graphs without creating a connection between them. A Portal is a presentation for native wiring, not a cross-graph reference.

## Which types are supported?

Portal is **data-only**. Data pin types must pass the native Blueprint graph schema's compatibility checks. An Input's supported type can be inferred from connected native reroute pins; incompatible Outputs cannot be retargeted to that family.

## Why execution wires are excluded

Execution pins have flow-control semantics that do not safely match one Input fanning out invisibly to several Outputs. Portal does not promise multi-output execution or replace explicit execution flow nodes.

## Common example

A float Portal family in an Event Graph can feed multiple eligible float consumers in that graph. If a function needs the value, use the Blueprint's ordinary function inputs, variables or other explicit data transfer mechanisms instead.

## After pasting between graphs

An Output pasted alone into a different graph is not supposed to silently attach to a same-named Input. Inspect it as an orphan and reconnect intentionally to a compatible Input in the destination graph.

For native implementation and engine boundaries, [read Technical](TECHNICAL.md).
