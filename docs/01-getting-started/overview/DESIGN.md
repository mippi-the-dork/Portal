# What Portal Does — Design

**Portal v0.12.0 · UE 5.8.x · Win64 · Blueprint Editor**

## What problem does Portal solve?

<figure class="doc-media" data-kind="image" data-media-id="overview-graph">
  <img src="../../assets/media-placeholder.png" alt="A Portal family replaces long visible data wires with named Input and Output endpoints." loading="lazy">
  <figcaption>A Portal family replaces long visible data wires with named Input and Output endpoints.<span class="media-capture-instructions"><strong>Capture instructions · Image:</strong> Show a Blueprint graph with a data source connected to one named Portal Input and two distant Portal Outputs feeding consumers. Keep the family name, count and graph context visible.</span></figcaption>
</figure>

Long Blueprint data wires make visual scripting difficult to scan. Portal lets a value enter a **named Portal Input** in one part of a Blueprint graph and emerge through one or more **Portal Outputs** elsewhere in that *same* graph. The editor hides the internal connecting wire while Portal is installed, leaving a readable name at each endpoint.

## What appears in the graph?

An **Input** accepts the source data. One family can have several **Outputs**, each connected to a consumer. The Input shows the number of Outputs; it shows **0** before any Outputs exist. Both sides display the same Portal family name. Inputs and Outputs have mirrored layouts and remain ordinary Blueprint graph nodes underneath.

## What is different from a variable?

Portal is not a new Blueprint variable or data store. It is graph-local data routing. Values follow normal wires and Blueprint compiler rules; Portal does not store a value for later use or bridge across function and Event graphs.

## What should I try?

Create a Portal Input from a compatible data output pin, name it, then create an Output and connect it to a distant data input pin. Double-click the Output to jump back to the Input. Save, close, and reopen the Blueprint to see the family restored.

## Important limitations

Portal supports **Blueprint data pins**, not execution pins. It is editor-only, does not add runtime components, and cannot pass values between different Blueprint graphs. The current documented release is **0.12.0 release candidate** for Unreal Engine **5.8.0–5.8.3 on Win64**.

For native implementation and engine boundaries, [read Technical](TECHNICAL.md).
