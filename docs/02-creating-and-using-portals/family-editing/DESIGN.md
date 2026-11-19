# Renaming, Retargeting, and Deleting Families — Design

**Portal v0.12.0 · UE 5.8.x · Win64 · Blueprint Editor**

## Renaming a family

<figure class="doc-media" data-kind="gif" data-media-id="family-retarget">
  <img src="../../assets/media-placeholder.png" alt="An Output can be moved to a compatible Input without replacing its downstream consumer node." loading="lazy">
  <figcaption>An Output can be moved to a compatible Input without replacing its downstream consumer node.<span class="media-capture-instructions"><strong>Capture instructions · GIF:</strong> Record one Portal family with two Outputs, open Change Input on one Output, retarget it to another compatible Input, and show the two families’ updated counts while the consumer remains connected.</span></figcaption>
</figure>

Rename the Input to change the family label. All connected Outputs belong to the same named family and are synchronized by Portal's editor logic. Keep names descriptive; the name is not a substitute for the family identifier.

## Move an Output to another Input

Right-click a healthy Output and choose **Change Input**, then select a compatible Input from the **same graph**. This retargets that Output while keeping its downstream consumer wiring where possible. If there is no compatible alternative family, the option may be absent.

## Delete the entire family

Right-click the Input and choose **Delete Portal Family** to remove the Input and all its Outputs in one Undoable action.

## What pressing Delete on an Input does

With a single selected Input that still has Outputs, Delete opens a choice:

- **Yes:** delete the full family.
- **No:** delete only the Input, leaving recoverable orphaned Outputs.
- **Cancel:** do not change anything.

That prompt is intentional protection against silently stranding downstream consumer nodes. If you choose No, review the resulting orphans before compiling.

## Reverse accidental edits

Use Unreal's normal Undo/Redo and inspect the graph. Retargeting and deletion alter connections and can require another compile. Source code implements transactions, but difficult copy/paste and save transitions should still be tested in the target project.

For native implementation and engine boundaries, [read Technical](TECHNICAL.md).
