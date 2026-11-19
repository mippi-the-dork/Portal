# Inputs, Outputs, and Counts — Design

**Portal v0.12.0 · UE 5.8.x · Win64 · Blueprint Editor**

## One Input, many Outputs

<figure class="doc-media" data-kind="image" data-media-id="node-mirrors">
  <img src="../../assets/media-placeholder.png" alt="Portal Inputs and Outputs mirror their labels, icons and shared Output counts." loading="lazy">
  <figcaption>Portal Inputs and Outputs mirror their labels, icons and shared Output counts.<span class="media-capture-instructions"><strong>Capture instructions · Image:</strong> Capture one Input and two Outputs with the same family name and clearly visible mirrored node layouts and count of 2. Use a clean, zoomed-in Blueprint graph.</span></figcaption>
</figure>

Each Portal family has one **Input** carrying data from its source and any number of **Outputs** that expose that same data elsewhere in the graph. Output nodes are consumers of a shared logical route, not copies of a runtime value store.

## Reading the node

| Endpoint | Layout from left to right | Count meaning |
| --- | --- | --- |
| Input | `pin · icon · name · count` | How many Outputs currently belong to this family |
| Output | `count · name · icon · pin` | Total Outputs in its family, not this node's individual fan-out |

An Input with no Outputs explicitly displays **0**. Names and counts remain legible without opening Details. Both node types use a consistent fixed-height visual while names are edited.

## Naming

Give each Input a short descriptive name identifying the routed value rather than the physical node placement, such as `Aim Direction` or `Current Health`. Rename the Input; its Outputs share the updated family presentation. Avoid reusing indistinguishable names for unrelated values, even though identity is GUID-based.

## Reading an orphan

If an Output loses its Input, the label can retain cached information, but the missing family must be repaired or reassigned. A readable label alone does not confirm a healthy native connection.

For native implementation and engine boundaries, [read Technical](TECHNICAL.md).
