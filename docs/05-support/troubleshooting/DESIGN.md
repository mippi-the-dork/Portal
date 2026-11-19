# Troubleshooting and Recovery — Design

**Portal v0.12.0 · UE 5.8.x · Win64 · Blueprint Editor**

## I can't find the Portal action

<figure class="doc-media" data-kind="image" data-media-id="troubleshooting-orphan">
  <img src="../../assets/media-placeholder.png" alt="An orphaned Output offers explicit recovery and conversion choices in its context menu." loading="lazy">
  <figcaption>An orphaned Output offers explicit recovery and conversion choices in its context menu.<span class="media-capture-instructions"><strong>Capture instructions · Image:</strong> Capture an orphaned Output’s context menu with Create Missing Input, Reconnect Output where a compatible Input exists, and Convert Output to Reroute clearly legible. Include part of the surrounding Blueprint graph.</span></figcaption>
</figure>

Check that the plugin is enabled in a UE 5.8.x Win64 editor and that you're editing a **Blueprint K2 graph**, not another graph editor. Search `Create Portal Reroute Node` or `Portal`. If needed, restart the editor after enabling the plugin.

## Create Portal isn't available on a pin

Check the pin **direction** and type. Creation requires a compatible **data output** pin. Execution pins are unsupported. **Connect Portal Output** instead appears on compatible, *unconnected data inputs*, when a compatible Input exists in the same graph.

## An Output appears disconnected

Right-click it and check for **Create Missing Input** or **Reconnect Output**. Repair its family or convert it to a native reroute and rewire manually. A readable cached label is not evidence of a valid source.

## My Output is in the wrong family

Use **Change Input** on a connected Output and pick a compatible graph-local Input. Inspect its Output count and compile again.

## A pasted family behaves strangely

Copying native reroutes can change metadata identity. Select the family, check for duplicated names or orphaned Outputs, compile, and reopen. Source includes GUID collision repair, but it cannot infer an unrelated missing data source.

## My Blueprint looks different without Portal

That is expected: ordinary reroutes and native bridge wires are visible when Portal's editor node widget is absent. Verify connectivity with a compile instead of treating the changed rendering as missing data.

## My project has an older Portal version

Use source control and test legacy migration in a disposable Blueprint first. Migration can dirty Blueprints because old custom node classes are rewritten as native reroutes.

## Reporting a useful issue

Include exact UE/Portal version, graph type, pin category/direction, repro steps, whether the nodes were copied/pasted, whether the Input is still present, compiler errors, and screenshots of the relevant graph section.

For native implementation and engine boundaries, [read Technical](TECHNICAL.md).
