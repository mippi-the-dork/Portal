# Orphaned Outputs and Recovery — Technical

**Portal v0.12.0 · UE 5.8.x · Win64 · Blueprint Editor**

## Native orphan detection

`PortalNativeUtils::IsOrphanUsage` checks whether an Output resolves to a valid Input through physical bridge links and/or family identity in the current graph.

## Reconstruction

`CreateMissingInputForUsage` creates a native Input near the orphaned Output, restores identity and attempts to gather/reconnect other Outputs with the same family GUID. It cannot invent the original upstream data source if that source was removed.

## Retargeting

`CanRetargetUsageToDeclaration` checks graph and pin compatibility. `RetargetUsage` reassigns the hidden bridge and metadata while keeping downstream external consumer links when valid.

## When options appear

`AddPortalContextEntries` builds orphan-specific menu entries. `Reconnect Output` appears only when a compatible Input exists, while **Create Missing Input** is available on an identified orphan.

## Implementation references

- [PortalNativeUtils.cpp](https://github.com/mippi-the-dork/Portal/blob/main/Source/Portal/Private/PortalNativeUtils.cpp)
- [PortalModule.cpp](https://github.com/mippi-the-dork/Portal/blob/main/Source/Portal/Private/PortalModule.cpp)
- [PortalInputProcessor.cpp](https://github.com/mippi-the-dork/Portal/blob/main/Source/Portal/Private/PortalInputProcessor.cpp)
- [Native-Safe Architecture](https://github.com/mippi-the-dork/Portal/blob/main/Doc/Native-Safe-Architecture.md)

**Verification:** Source-reviewed; live UE Editor automation/manual validation remains separate.

For actual steps and expected behavior, [read Design](DESIGN.md).
