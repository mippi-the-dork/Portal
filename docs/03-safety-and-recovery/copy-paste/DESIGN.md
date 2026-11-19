# Copy/Paste, Undo, and Graph Integrity — Design

**Portal v0.12.0 · UE 5.8.x · Win64 · Blueprint Editor**

## Copying a whole family

<figure class="doc-media" data-kind="gif" data-media-id="copy-paste-family">
  <img src="../../assets/media-placeholder.png" alt="Portal attempts to preserve family membership while resolving duplicate identities introduced by graph copying." loading="lazy">
  <figcaption>Portal attempts to preserve family membership while resolving duplicate identities introduced by graph copying.<span class="media-capture-instructions"><strong>Capture instructions · GIF:</strong> Record copying a Portal Input and its two Outputs within one Blueprint graph, pasting the family, and confirming the copy has a separate identity and correct Output counts. End with Undo/Redo and a successful compile.</span></figcaption>
</figure>

If you copy an Input together with its Outputs, Portal is designed to repair copied family identity where needed. Check that the pasted endpoints still belong together and that the new Input does not accidentally share identity with the original family.

## Copying only an Output

A lone Output pasted elsewhere might become orphaned. In another graph, Portal should not silently attach it to a same-named family. Right-click and reconnect to a compatible Input or create a missing one after checking where the data should originate.

## Undo and Redo

Creation, retargeting, conversion and family deletion use Unreal Editor transaction paths in source. After Undo/Redo, check the family counts and data connections. If the editor has not refreshed yet, inspect the graph again rather than assuming stale presentation indicates a changed runtime result.

## Save and reopen

Save the Blueprint, close its tab, and reopen it. Names, Output counts and family links should be restored from native nodes and metadata. Compile after complex copy/paste edits.

## If a family looks duplicated

Prefer selecting the family and examining its Input rather than renaming one Output in isolation. Duplicate family identity can be resolved by repair, but incorrectly connected consumers still require human judgment.

For native implementation and engine boundaries, [read Technical](TECHNICAL.md).
