# Portal 0.12.0 Release Candidate

Portal brings Named Reroute-style data routing to Unreal Engine Blueprint graphs.

Portal is editor-only and data-only. Portal Inputs and Outputs are saved as native Blueprint reroute nodes, with Portal identity stored as editor metadata. Removing the plugin therefore exposes ordinary reroute nodes and their native wires instead of leaving missing custom Blueprint node classes.

## Compatibility

- Unreal Engine 5.8.0 - 5.8.3
- Windows 64-bit
- Editor only / UncookedOnly
- Blueprint graphs
- Data pins only
- No engine source changes

## Creating Portals

### Blueprint action browser

Right-click empty Blueprint graph space and search for `Portal`.

Available actions include:

- `Create Portal Reroute Node`
- `Portal > Outputs > Portal Output: <Portal Name>` for Portal Inputs in the current graph

Portal Output actions are graph-local. An Input in Event Graph does not appear in a function graph, macro graph, or another Blueprint.

You can also search by the Portal name or common terms such as `named reroute`, `reroute`, `get`, and `getter`.

### From pins

- Right-click a compatible data output pin and choose `Create Portal` to create and connect a new Portal Input.
- Right-click an unconnected compatible data input pin and choose `Connect Portal Output` to choose an existing Portal Input and connect a new Output.
- Pin-created Portal nodes are positioned next to the exact pin used to create them.

### Fast creation

- Shift + R + Left Click creates a Portal Input.
- Shift + R + Left Click while one Portal family member is selected creates another Output for that family.

## Portal node layout

Portal Inputs and Outputs mirror one another:

```text
Input:   pin | icon | name | count
Output:  count | name | icon | pin
```

The count is the total number of Outputs in the family. Inputs show `0` when they do not yet have any Outputs.

- Double-click an Output to jump to its Input.
- Double-click an Input to select all Outputs in that family.
- `Select Portal Family` selects the Input and all Outputs.
- The node keeps a fixed height while renaming.

## Family editing

Right-click a Portal Output to:

- create another Output
- jump to its Input
- select the Portal family
- change the Output to another compatible Input

Right-click an orphaned Output to:

- `Create Missing Input`
- `Reconnect Output`
- `Convert Output to Reroute`

If several orphaned Outputs share the same missing family identity, `Create Missing Input` repairs the family together.

Right-click a Portal Input and choose `Delete Portal Family` to remove the Input and all Outputs in one undoable operation.

Pressing Delete on a Portal Input that still has Outputs asks whether to delete the whole family, delete only the Input and intentionally leave recoverable Outputs, or cancel.

## Native-safe fallback

With Portal installed, a family can look like this:

```text
Source -> [ Game Time ]

                    [ Game Time ] -> Consumer
```

The saved Blueprint contains native reroutes and a native connection underneath:

```text
Source -> Reroute ---------------- Reroute -> Consumer
```

Portal hides the internal bridge visually while installed. The native pins and connections remain in the asset.

If Portal is removed:

- no Portal-specific Blueprint node class is required to load the saved graph
- Portal nodes appear as ordinary Blueprint reroutes
- the hidden internal wire becomes visible
- Blueprint connectivity remains intact

## Execution pins

Portal does not support execution pins. Unreal's native exec reroute behavior assumes one outgoing execution continuation, which does not map safely to one Portal Input with multiple hidden Outputs.

## 0.12.0 release-candidate polish

- Inputs now display an explicit Output count of `0` instead of hiding the count when a family has no Outputs.
- Portal node tooltips now include the family name consistently on both Inputs and Outputs.
- User-facing documentation has been consolidated around the current Input / Output terminology and current Blueprint action-browser workflow.
- The release validation plan now covers creation paths, action search, copy/paste, Undo/Redo, orphan recovery, graph scoping, execution-pin rejection, save/reopen, and uninstall fallback.
- No new runtime dependency or serialized Portal node class was introduced.

See `Doc/Prototype-Test-Plan.md` for the full release-candidate validation checklist.
