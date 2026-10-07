# Portal 0.4.2

Portal brings Named Reroute-style data routing to Unreal Engine Blueprint graphs.

0.4.2 keeps the native-safe foundation introduced in 0.4.x and fixes the InputCore link dependency required by Portal's double-click mouse handling. The native-safe architecture still replaces the prototype custom saved-node architecture with a native-safe foundation. Portal families are now built from stock `UK2Node_Knot` Blueprint reroute nodes. Portal adds identity and presentation through editor metadata and Slate only.

## Target

- Unreal Engine 5.8.0 - 5.8.3
- Windows 64-bit
- UncookedOnly
- Blueprint editor tooling
- Data pins only
- No runtime Portal system
- No engine source changes


## 0.4.2 Link Fix

- Added the Unreal `InputCore` module dependency required by `EKeys::LeftMouseButton` in `SPortalNativeKnotNode`.
- Fixes the UE 5.8 linker error for `EKeys::LeftMouseButton`.
- No Portal persistence, migration, metadata, family, fallback, or UX behavior changed from 0.4.1.

## Native-Safe Foundation

A Portal Declaration and every Portal Usage are saved as ordinary Unreal Blueprint reroute nodes.

With Portal installed:

```text
Source -> [ Game Time ]

                    [ Game Time ] -> Consumer
```

Underneath, the Blueprint contains native reroutes and a real native wire:

```text
Source -> Reroute ---------------- Reroute -> Consumer
```

Portal's custom Slate presentation hides the internal bridge pin and wire while the plugin is installed. The actual pins are not serialized as hidden.

If Portal is removed, Unreal can display the same saved nodes and connections as ordinary reroutes and wires. Blueprint execution does not depend on Portal code.

## 0.4.2 Features

- Native `UK2Node_Knot` backing nodes for new Portal families.
- Automatic migration of loaded Portal 0.1 - 0.3 custom nodes to native reroutes.
- Portal role, family GUID, source graph GUID, name, and color stored in standard package metadata.
- Human-readable native `NodeComment` fallback markers for metadata recovery and copy/paste recovery.
- Compact Portal presentation supplied through a graph node factory.
- Declaration shows only its source input while Portal is installed.
- Usage shows only its consumer output while Portal is installed.
- The hidden bridge remains a real native graph connection.
- Declaration Usage count is shown in the compact node.
- Declaration and Usage have distinct `D` and `U` badges.
- Orphaned Usages display a red `Missing` state.
- Double-click a Usage to jump to its Declaration.
- `Create Usage` from a Declaration.
- `Create Another Usage` from a Usage.
- `Select Portal Family`.
- `Convert Portal to Reroutes` removes Portal metadata but preserves nodes and connections.
- Native reroutes can be converted into Portal Declarations.
- Existing outgoing reroute branches become Portal Usages during conversion.
- Portal names remain graph-local and unique.
- Low-frequency editor repair pass restores metadata and copied-family identity when possible.

## Creating a Portal in 0.4.2

0.4.2 intentionally uses a safety-first creation flow:

1. Create a normal Blueprint Reroute Node.
2. Right-click the Reroute Node.
3. Choose `Convert to Portal Declaration` in the Portal section.
4. Rename the Declaration inline.
5. Right-click the Declaration and choose `Create Usage`.

If the original reroute already had outgoing branches, Portal creates Usage reroutes for those branches automatically.

A direct Portal entry in the Blueprint graph action menu is planned after the native-safe foundation is validated.

## Migration from 0.3.0

Portal 0.4.1 keeps the old prototype classes only so existing assets can load long enough to migrate.

When a loaded Blueprint contains 0.1 - 0.3 Portal nodes, Portal replaces them with native `UK2Node_Knot` nodes, preserves their external links, positions, names, colors, and families, and marks the Blueprint dirty so the native-safe representation can be saved.

Open and save prototype Blueprints once with 0.4.1 before testing complete plugin removal.

## Uninstall Safety

The key 0.4.1 acceptance test is:

1. Create or migrate a Portal family.
2. Compile and run the Blueprint.
3. Save the Blueprint.
4. Close Unreal Editor.
5. Remove the Portal plugin folder.
6. Reopen the project and Blueprint.

Expected result:

- no missing Portal node classes
- no broken Portal-specific nodes
- Portal nodes appear as ordinary Blueprint reroutes
- the previously hidden internal wire becomes visible
- Blueprint connectivity is preserved
- Blueprint still compiles and runs

See `Doc/Prototype-Test-Plan.md` for the complete validation pass.
