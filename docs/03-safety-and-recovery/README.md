# 03. Native Safety and Recovery

Native-safe storage is Portal’s primary compatibility distinction: it serializes stock `UK2Node_Knot` reroutes and uses metadata for naming and family identity. Orphan repair and legacy migration protect user work, but they still need verification in real Blueprints, particularly under copy/paste and uninstall scenarios.

## What to know before proceeding

Every Portal route is **data-only and graph-local**. Native Blueprint pins and links remain the saved source of truth. The editor-only Portal UI is not a runtime data store.

## Native Safety and Recovery topics

| Topic | Covers | Design | Technical |
| --- | --- | --- | --- |
| **Native Reroutes and Plugin Removal** | Why Portal data connections remain native when the editor extension is absent. | [Design](native-safe/DESIGN.md) | [Technical](native-safe/TECHNICAL.md) |
| **Orphaned Outputs and Recovery** | Repair outputs whose source Input is missing or inaccessible. | [Design](orphans/DESIGN.md) | [Technical](orphans/TECHNICAL.md) |
| **Legacy Migration and Repair** | How older Portal node formats are converted and checked. | [Design](migration/DESIGN.md) | [Technical](migration/TECHNICAL.md) |
| **Copy/Paste, Undo, and Graph Integrity** | Avoid identity collisions and unintended family reassociation. | [Design](copy-paste/DESIGN.md) | [Technical](copy-paste/TECHNICAL.md) |

**Design** explains user steps and expected results; **Technical** explains native Blueprint APIs, serialized state, and limitations.
