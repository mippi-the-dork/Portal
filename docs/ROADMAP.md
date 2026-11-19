# Portal Roadmap

**Current documented version:** 0.12.0 release candidate · UE 5.8.0–5.8.3 · Win64. This page distinguishes source-grounded history from unapproved future ideas.

## Historical and current milestones

| Milestone | Status | Supported evidence |
| --- | --- | --- |
| Early Portal prototypes | Historical; exact version history incomplete | Legacy `UK2Node_PortalDeclaration` and `UK2Node_PortalUsage` classes retained as migration readers |
| **v0.4.1 — Native-safe architecture** | Historical, documented | `Doc/Native-Safe-Architecture.md` records the switch to native `UK2Node_Knot` serialization plus metadata |
| **v0.12.0 — Release candidate** | Current repository descriptor / README | Portal Input/Output terminology, explicit Output count 0, tooltips, graph-local action search, native-safe fallback and documented repair operations |

## What exists now

Portal creates and routes **data-only, graph-local Input/Output families** in Blueprint graphs. Users can create nodes from menus/pins/Shift+R, navigate between endpoints, retarget Outputs, convert to reroutes, and repair missing Inputs. Saved graphs rely on native Blueprint reroute nodes and ordinary physical connections.

## Future planning

**No next-version feature commitments or dates were supplied for Portal.** Instead of inventing a v1.0 or v2.0 roadmap, track proposed improvements as research topics pending approval:

- **Editor discoverability:** verify whether the source-only `SPortalBrowser` widget should become a supported dockable window. This is a research candidate, **not a shipped window or committed feature**.
- **Automated regression testing:** add native Unreal Automation coverage for mixed data types, copy/paste, graph-locality, orphan recovery, migration and uninstall fallback.
- **Graph-scale performance:** measure repair ticker cost with many loaded Blueprints before changing caching or scheduling.
- **Documentation and UX:** capture actual node screenshots, menu GIFs and versioned compatibility results.

## Decision criteria

Any new Portal capability should preserve native-safe serialization, graph-local data correctness, easy Undo/Redo, Blueprint compiler compatibility, and no Unreal Engine source modifications. Roadmap changes should identify whether they are Approved, In progress, or Released only after implementation evidence exists.

[Current workflows](02-creating-and-using-portals/README.md) · [Native Safety](03-safety-and-recovery/README.md) · [Verification Matrix](VERIFICATION_MATRIX.md)
