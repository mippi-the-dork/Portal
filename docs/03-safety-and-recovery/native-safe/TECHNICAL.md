# Native Reroutes and Plugin Removal — Technical

**Portal v0.12.0 · UE 5.8.x · Win64 · Blueprint Editor**

## Persisted node classes

`PortalNativeUtils::CreateKnot` creates `UK2Node_Knot`, not a runtime Portal node. `WritePortalData` stores identity in package-owned `FMetaData`; the node's readable `NodeComment` is a copy/paste fallback. Both the source graph and family have GUIDs.

## Why the wire disappears visually

`SPortalNativeKnotNode` renders only the outward-facing pin for each endpoint and omits the internal bridge pin widget. The pin and link remain in the graph's underlying data. The Unreal graph compiler handles normal reroute connectivity.

## Explicit conversion

`ConvertPortalFamilyToReroutes` clears the Portal metadata, preserves native reroute objects, retains their physical connections, and leaves the Blueprint to render those nodes normally. It does not construct a replacement network from scratch.

## Compatibility warning

Older `UK2Node_PortalDeclaration`/`UK2Node_PortalUsage` classes are migration readers. Version 0.12.0's native-save design should not be confused with the legacy representation present in older assets before migration.

## Implementation references

- [PortalNativeUtils.cpp](https://github.com/mippi-the-dork/Portal/blob/main/Source/Portal/Private/PortalNativeUtils.cpp)
- [PortalModule.cpp](https://github.com/mippi-the-dork/Portal/blob/main/Source/Portal/Private/PortalModule.cpp)
- [PortalInputProcessor.cpp](https://github.com/mippi-the-dork/Portal/blob/main/Source/Portal/Private/PortalInputProcessor.cpp)
- [Native-Safe Architecture](https://github.com/mippi-the-dork/Portal/blob/main/Doc/Native-Safe-Architecture.md)

**Verification:** Source-reviewed; live UE Editor automation/manual validation remains separate.

For actual steps and expected behavior, [read Design](DESIGN.md).
