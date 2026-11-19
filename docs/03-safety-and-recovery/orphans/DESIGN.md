# Orphaned Outputs and Recovery — Design

**Portal v0.12.0 · UE 5.8.x · Win64 · Blueprint Editor**

## What is an orphan Output?

<figure class="doc-media" data-kind="gif" data-media-id="orphan-recovery">
  <img src="../../assets/media-placeholder.png" alt="Orphan recovery can reconstruct an Input and reconnect related Outputs from the same missing family." loading="lazy">
  <figcaption>Orphan recovery can reconstruct an Input and reconnect related Outputs from the same missing family.<span class="media-capture-instructions"><strong>Capture instructions · GIF:</strong> Record deleting a Portal Input while deliberately retaining its Outputs, then right-click an orphan and use Create Missing Input. Show the recovered Input and restored family count; compile afterward.</span></figcaption>
</figure>

An **orphan** is an Output whose original Portal Input cannot be resolved. This can happen after deliberately deleting only the Input, clipboard operations, or damage to family links/identity. The Output can retain a readable name, but that does not guarantee it receives data.

## Recovery choices

Right-click an orphaned Output and choose one of the available actions:

- **Create Missing Input:** create a replacement Input. If other orphan Outputs share the same missing family identity, Portal tries to repair them together.
- **Reconnect Output:** choose an existing compatible Input in the same graph.
- **Convert Output to Reroute:** remove Portal metadata from this Output, leaving an ordinary native Blueprint reroute for manual rewiring.

## Recovery workflow

1. Locate the orphaned Output and inspect its label and upstream data type.
2. Choose whether the original family should be reconstructed or the Output should join a different compatible family.
3. Use **Create Missing Input** or **Reconnect Output**.
4. Confirm the Input exists and its Output count updates.
5. Compile the Blueprint and inspect the consumer results. If no valid source is connected, fix that explicitly.

## Avoid an incorrect 'repair'

Do not connect an orphan to a similarly named Input merely because the text matches. Graph identity and compatible data types matter more than visual names.

For native implementation and engine boundaries, [read Technical](TECHNICAL.md).
