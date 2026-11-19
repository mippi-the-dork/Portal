# Native Reroutes and Plugin Removal — Design

**Portal v0.12.0 · UE 5.8.x · Win64 · Blueprint Editor**

## A readable graph without runtime dependencies

<figure class="doc-media" data-kind="image" data-media-id="native-fallback">
  <img src="../../assets/media-placeholder.png" alt="Portal changes how a native data connection is displayed, not the connection that Blueprint serialization preserves." loading="lazy">
  <figcaption>Portal changes how a native data connection is displayed, not the connection that Blueprint serialization preserves.<span class="media-capture-instructions"><strong>Capture instructions · Image:</strong> Make a side-by-side diagram: left shows named Portal Input and Output with the hidden bridge, right shows the same two native reroutes connected by a visible standard Blueprint wire after disabling Portal. Label the preserved connectivity.</span></figcaption>
</figure>

With Portal enabled, a family visually appears as a named Input and distant Outputs, without the bridge wire between them. Underneath, **both endpoints are ordinary Unreal reroute nodes**, connected by a native graph wire.

## What happens when Portal is absent?

The Blueprint no longer shows Portal's special Input/Output presentation. The same saved native nodes appear as ordinary reroutes, and the previously suppressed bridge wire is visible. Since the saved nodes are native `UK2Node_Knot` objects, the asset is not supposed to require Portal-specific node classes just to load.

## Converting deliberately

Use **Convert Portal to Reroutes** on a healthy family to remove the Portal presentation metadata while retaining its native reroute nodes and physical links. This is useful before sharing the Blueprint with a teammate who does not use Portal, or when investigating graph connectivity.

## Important distinction

The fallback retains **connectivity**, not the exact presentation. You will see more wires. You should still compile and test representative Blueprints after conversion or uninstall; plugin removal is not a reason to skip regression checks.

For native implementation and engine boundaries, [read Technical](TECHNICAL.md).
