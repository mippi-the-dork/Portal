# Portal Native-Safe Architecture

Portal 0.4.1 changes what is saved inside a Blueprint.

## Saved Representation

A Portal family is made from stock Unreal `UK2Node_Knot` objects and normal Blueprint pin links.

Portal does not require a Portal-specific K2 node class for the saved Declaration or Usage.

Portal stores editor identity on the native reroute objects:

- role: Declaration or Usage
- family GUID
- source graph GUID
- Portal name
- Portal color

The primary storage is the package-owned `FMetaData` map. A readable `NodeComment` marker is also maintained so copied native reroutes can recover Portal intent when package metadata is unavailable to clipboard serialization.

## Visual Suppression Only

Portal never saves the native bridge pins as hidden.

While Portal is installed, its graph node widget omits:

- Declaration output pin
- Usage input pin

Because the GraphEditor has no visible geometry for those internal pin widgets, the bridge wire is not shown.

The underlying `UEdGraphPin` objects and link remain normal.

## Compiler Behavior

The saved nodes are ordinary `UK2Node_Knot` reroutes. Unreal owns their compiler behavior.

Portal does not insert a runtime function call, variable, subsystem, or gameplay object.

## Plugin Removal

When Portal is absent:

- the stock `UK2Node_Knot` classes still load
- native input and output pins render normally
- the physical bridge connection renders normally
- the Blueprint compiler sees ordinary reroutes
- Portal metadata is inert package data

The special Portal presentation is lost, but graph connectivity is intended to remain unchanged.

## Legacy Migration

Portal 0.4.1 retains the old `UK2Node_PortalDeclaration` and `UK2Node_PortalUsage` classes only as migration readers.

A low-frequency editor repair pass scans loaded Blueprints. When it finds legacy nodes it creates equivalent native reroutes, transfers connections and presentation data, removes the legacy nodes, and marks the Blueprint dirty.

After saving the migrated Blueprint, the asset should no longer depend on Portal K2 node classes.

## Explicit Escape Hatch

`Convert Portal to Reroutes` removes Portal metadata and presentation while leaving the same native nodes and physical links in place.

This is intentionally a metadata operation rather than a graph reconstruction operation.
