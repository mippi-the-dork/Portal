# Finding and Selecting Families — Design

**Portal v0.12.0 · UE 5.8.x · Win64 · Blueprint Editor**

## Navigate directly

<figure class="doc-media" data-kind="gif" data-media-id="navigation-jump">
  <img src="../../assets/media-placeholder.png" alt="Double-click navigation makes distant Portal family endpoints easy to find and inspect." loading="lazy">
  <figcaption>Double-click navigation makes distant Portal family endpoints easy to find and inspect.<span class="media-capture-instructions"><strong>Capture instructions · GIF:</strong> Record a wide Blueprint graph with the Input and Outputs far apart. Double-click an Output to jump to the Input, then double-click the Input to select all Outputs; show the selection clearly.</span></figcaption>
</figure>

**Double-click an Output** to jump to its Input. **Double-click an Input** to select all Outputs of that family. These shortcuts are designed for large Blueprint graphs where the endpoints may be several screens apart.

## Select the complete family

Right-click a healthy family endpoint and choose **Select Portal Family**. The Input and all associated Outputs are added to the graph selection, which is useful before moving related nodes or reviewing their count.

## Create additional Outputs

Right-click the Input for **Create Output**, or right-click an Output for **Create Another Output**. The Input's count reflects the family total after an endpoint is created.

## Troubleshooting navigation

If double-clicking an Output cannot find its Input, treat it as potentially orphaned. Right-click and inspect **Create Missing Input** or **Reconnect Output**. A valid Portal family should always remain within one graph.

## Keep names meaningful

Portal removes long visual wires, so names carry more of the graph's readability burden. Prefer value-oriented names, and keep related families near their logical source when possible.

For native implementation and engine boundaries, [read Technical](TECHNICAL.md).
