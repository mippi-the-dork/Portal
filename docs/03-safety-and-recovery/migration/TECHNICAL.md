# Legacy Migration and Repair — Technical

**Portal v0.12.0 · UE 5.8.x · Win64 · Blueprint Editor**

## Migration readers

`UK2Node_PortalDeclaration` and `UK2Node_PortalUsage` remain in source for legacy assets. `PortalNativeUtils::MigrateLegacyGraph` creates equivalent native `UK2Node_Knot` endpoints, copies pin links and presentation data, removes the legacy nodes and marks the graph/Blueprint modified.

## Editor reconciliation

`FPortalModule::TickRepair` runs via the core ticker every **0.75 seconds** and calls `MigrateAndRepairLoadedBlueprints`. `RepairGraph` can materialize missing metadata from `NodeComment`, correct duplicate family GUIDs, sync names/colors and resolve same-graph Output bridges.

## Transaction/performance boundaries

A low-frequency scan does not imply zero overhead; loaded Blueprint count and graph complexity can affect cost. Migration can dirty assets intentionally. Avoid calling this path a zero-cost or universal data-recovery system.

## Execution safety

If a legacy or wildcard Portal becomes execution-typed, `RepairGraph` demotes it to ordinary reroutes to avoid unsupported multi-output execution semantics.

## Implementation references

- [PortalNativeUtils.cpp](https://github.com/mippi-the-dork/Portal/blob/main/Source/Portal/Private/PortalNativeUtils.cpp)
- [PortalModule.cpp](https://github.com/mippi-the-dork/Portal/blob/main/Source/Portal/Private/PortalModule.cpp)
- [PortalInputProcessor.cpp](https://github.com/mippi-the-dork/Portal/blob/main/Source/Portal/Private/PortalInputProcessor.cpp)
- [Native-Safe Architecture](https://github.com/mippi-the-dork/Portal/blob/main/Doc/Native-Safe-Architecture.md)

**Verification:** Source-reviewed; live UE Editor automation/manual validation remains separate.

For actual steps and expected behavior, [read Design](DESIGN.md).
