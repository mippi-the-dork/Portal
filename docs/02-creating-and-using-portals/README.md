# 02. Creating and Using Portals

Creation, navigation, and editing all operate on Portal families within a single Blueprint graph. An Input provides a named source and Outputs provide distant destinations, while normal Unreal reroute wiring remains underneath. This section covers the actual UI paths and safe family operations.

## What to know before proceeding

Every Portal route is **data-only and graph-local**. Native Blueprint pins and links remain the saved source of truth. The editor-only Portal UI is not a runtime data store.

## Creating and Using Portals topics

| Topic | Covers | Design | Technical |
| --- | --- | --- | --- |
| **Inputs, Outputs, and Counts** | Read the mirrored node layouts and family count. | [Design](inputs-and-outputs/DESIGN.md) | [Technical](inputs-and-outputs/TECHNICAL.md) |
| **Creating Portals from Menus and Pins** | Choose the fastest valid creation route for each scenario. | [Design](creation-paths/DESIGN.md) | [Technical](creation-paths/TECHNICAL.md) |
| **Renaming, Retargeting, and Deleting Families** | Edit family membership without losing track of downstream consumers. | [Design](family-editing/DESIGN.md) | [Technical](family-editing/TECHNICAL.md) |
| **Finding and Selecting Families** | Move between distant graph endpoints without tracing hidden data wires. | [Design](navigation/DESIGN.md) | [Technical](navigation/TECHNICAL.md) |
| **Graph Scope and Type Compatibility** | Understand why a family works only inside one Blueprint graph. | [Design](graph-boundaries/DESIGN.md) | [Technical](graph-boundaries/TECHNICAL.md) |

**Design** explains user steps and expected results; **Technical** explains native Blueprint APIs, serialized state, and limitations.
