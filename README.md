# Portal 0.6.0

Portal brings Named Reroute-style data routing to Unreal Engine Blueprint graphs.

Portal 0.6.0 keeps the native-safe foundation and tightens Portal authoring and readability. Portal Input and Output nodes now mirror one another visually, both ends show the family Output count, data output pins can create a Portal directly, and execution pins are explicitly excluded from Portal authoring.

## Target

- Unreal Engine 5.8.0 - 5.8.3
- Windows 64-bit
- UncookedOnly
- Blueprint editor tooling
- Data pins only
- No runtime Portal system
- No engine source changes

## Native-Safe Foundation

Portal Inputs and Outputs are saved as ordinary Unreal Blueprint reroute nodes (`UK2Node_Knot`). Portal identity and presentation are editor metadata and Slate behavior layered on top.

With Portal installed:

```text
Source -> [ Game Time ]

                    [ Game Time ] -> Consumer
```

Underneath, the Blueprint contains native reroutes and a real native wire:

```text
Source -> Reroute ---------------- Reroute -> Consumer
```

Portal hides the internal bridge visually while installed. The actual native pins and connections remain intact.

If Portal is removed, the Blueprint falls back to normal reroute nodes and visible wires instead of missing Portal node classes.

## 0.6.0 UX Changes

- User-facing terms are now **Portal Input** and **Portal Output**.
- Input icon: `→◉`
- Output icon: `◉→`
- Both Input and Output nodes display the same family Output count as a plain number, for example `3`.
- Input layout is `pin | icon | name | count`.
- Output layout mirrors it as `count | name | icon | pin`.
- Portal nodes use a fixed 30-pixel content height so inline rename does not resize the node vertically.
- Right-click any compatible data output pin and choose `Create Portal` to create and connect a new Portal Input automatically.
- Execution pins are not supported by Portal. Exec pins do not offer `Create Portal`, exec reroutes do not offer conversion, and any legacy/wildcard Portal that becomes exec-typed safely falls back to native reroutes.
- Double-click a Portal Output to select and focus its Portal Input.
- Double-click a Portal Input to select all of its Portal Outputs.
- Context menu terminology now uses Input and Output.
- Native fallback comments use `Portal Input: Name` and `Portal Output: Name`.
- Existing 0.4.x `Portal Declaration:` and `Portal Usage:` fallback comments remain readable and are normalized automatically.

## Fast Creation Shortcut

**Shift + R + Left Click** is Portal's fast authoring gesture in Blueprint graphs.

- With no Portal family member selected, Shift + R + Left Click creates a new Portal Input at the click and begins renaming it.
- With one Portal Input or Output selected, Shift + R + Left Click creates another Output for that Portal family at the click.
- The shortcut only runs in editable K2 Blueprint graphs.
- Plain R + Left Click remains available for Unreal's normal reroute workflow.

## Context Menu

On a normal Blueprint reroute:

- `Convert to Portal Input`

On a Portal Input:

- `Create Output`
- `Select Portal Family`
- `Convert Portal to Reroutes`

On a Portal Output:

- `Jump to Input`
- `Create Another Output`
- `Select Portal Family`
- `Convert Portal to Reroutes`

## Uninstall Safety

The key safety test remains:

1. Create a Portal Input and one or more Outputs.
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

See `Doc/Prototype-Test-Plan.md` for the validation pass.
