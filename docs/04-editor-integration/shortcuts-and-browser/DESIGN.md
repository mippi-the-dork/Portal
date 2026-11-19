# Shortcuts and Portal Browser Status — Design

**Portal v0.12.0 · UE 5.8.x · Win64 · Blueprint Editor**

## Confirmed shortcut

<figure class="doc-media" data-kind="gif" data-media-id="shortcut-creation">
  <img src="../../assets/media-placeholder.png" alt="The Shift+R shortcut creates an Input or an additional Output depending on the current Portal family selection." loading="lazy">
  <figcaption>The Shift+R shortcut creates an Input or an additional Output depending on the current Portal family selection.<span class="media-capture-instructions"><strong>Capture instructions · GIF:</strong> Record the Blueprint graph with no selection, hold Shift+R and left-click to create an Input. Then select that Input and repeat Shift+R+left-click elsewhere to create an Output; keep cursor positions and selection visible.</span></figcaption>
</figure>

Hold **Shift + R**, then **left-click** in an editable Blueprint graph to create a Portal Input. If exactly one valid Portal Input or Output is selected, the shortcut instead creates another Output from its family.

The gesture requires the cursor over the actual graph panel. It is not a global editor command and does not apply to Material, Niagara, or other graph editors. Ctrl or Alt held with the shortcut prevents this input path in current source.

## Delete confirmation

Select a single Portal Input with at least one Output and press **Delete** while the cursor is over its editable graph. Choose to delete the full family, leave orphaned Outputs, or cancel. This is separate from deleting arbitrary non-Portal nodes.

## What about the Portal Browser?

The repository includes `SPortalBrowser`, with a search box, `New Input`, refresh, focus and Output navigation functions. **However, the current module source does not show a registered tab/menu that opens the widget.** Until a confirmed integration exists, it should not be described as a supported window users can open.

## Reliable way to navigate today

Use the Blueprint action search, node context menus, double-click navigation, and **Select Portal Family**. These paths are connected to the shipping module in source.

For native implementation and engine boundaries, [read Technical](TECHNICAL.md).
