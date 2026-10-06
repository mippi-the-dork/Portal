# Portal 0.3.0

Portal brings Named Reroute-style data routing to Unreal Engine Blueprint graphs.

Portal is still in prototype development. The hidden-edge compiler architecture and graph-local identity system are proven. 0.3.0 is the first dedicated UX pass.

## Target

- Unreal Engine 5.8.0 - 5.8.3
- Windows 64-bit
- UncookedOnly
- Blueprint editor tooling
- No runtime Portal system
- No engine source changes

## 0.3.0 Compact UX

- Portal nodes now opt into Unreal's native compact K2 node presentation.
- Declaration displays as the compact source endpoint with only its visible input side.
- Usage displays as the compact destination endpoint with only its visible output side.
- Internal Declaration-to-Usage bridge pins and wires remain hidden.
- Portal family color continues to identify related Declaration and Usage nodes.
- Declaration context actions are shortened to `Create Usage` and `Select Portal Family`.
- `Select Portal Family` now includes the Declaration itself plus all linked Usages.
- Usage context menu adds `Create Another Usage` and `Select Portal Family`.
- Usage keeps `Jump to Declaration`, and double-click navigation remains supported.
- Declaration tooltip reports the number of linked Usages.
- Newly created Usages use tighter placement suited to compact nodes.

## Core Model

A Portal Declaration receives one Blueprint data value. Portal Usages expose that same value elsewhere in the same Blueprint graph without a visible wire.

```text
Source -> [ Portal Name ]

                    [ Portal Name ] -> Consumer
                    [ Portal Name ] -> Consumer
```

The Declaration-to-Usage relationship is still stored as a real hidden K2 graph connection. Portal uses a compiler-transparent reroute base and merges its pin nets during Blueprint compilation, so Portal is not intended to create runtime instructions or runtime objects.

## Scope

Portal identities are graph-local. A Declaration and its Usages must live in the same `UEdGraph`. Portal remains data-only in 0.3.0.

## Installation

1. Close Unreal Editor.
2. Replace the existing `Portal` plugin folder with this version.
3. Regenerate project files if required.
4. Build the Editor target.
5. Launch Unreal Editor.

See `Doc/Prototype-Test-Plan.md` for the focused 0.3.0 validation pass.
