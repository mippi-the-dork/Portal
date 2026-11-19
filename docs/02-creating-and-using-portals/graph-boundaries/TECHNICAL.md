# Graph Scope and Type Compatibility — Technical

**Portal v0.12.0 · UE 5.8.x · Win64 · Blueprint Editor**

## Recorded graph identity

`FPortalKnotData` includes `SourceGraphGuid` and `FamilyGuid`. `UPortalNodeSpawner::IsTemplateNodeFilteredOut` filters Output menu entries by K2 graph context; `PortalNativeUtils::CanRetargetUsageToDeclaration` and pin connection routines enforce matching graphs.

## Execution exclusion

`IsExecKnot` rejects native K2 reroute pins with `UEdGraphSchema_K2::PC_Exec`. `RepairGraph` detects existing Portal-tagged exec knots and demotes affected families to ordinary reroutes rather than continuing unsupported Portal semantics.

## Clipboard/paste identity

A `NodeComment` fallback helps recover role and name if package `FMetaData` does not round-trip through clipboard serialization. The repair path resolves GUID collisions and connected family membership, but independently copied usages from other graphs are intentionally not assumed to belong to an unrelated family.

## Implementation references

- [PortalNativeUtils.cpp](https://github.com/mippi-the-dork/Portal/blob/main/Source/Portal/Private/PortalNativeUtils.cpp)
- [PortalModule.cpp](https://github.com/mippi-the-dork/Portal/blob/main/Source/Portal/Private/PortalModule.cpp)
- [PortalInputProcessor.cpp](https://github.com/mippi-the-dork/Portal/blob/main/Source/Portal/Private/PortalInputProcessor.cpp)
- [Native-Safe Architecture](https://github.com/mippi-the-dork/Portal/blob/main/Doc/Native-Safe-Architecture.md)

**Verification:** Source-reviewed; live UE Editor automation/manual validation remains separate.

For actual steps and expected behavior, [read Design](DESIGN.md).
