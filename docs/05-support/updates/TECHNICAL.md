# Updating, Support, and Compatibility — Technical

**Portal v0.12.0 · UE 5.8.x · Win64 · Blueprint Editor**

## Descriptor and module constraints

`Portal.uplugin` specifies `VersionName 0.12.0`, `EngineVersion 5.8.0`, Win64, `UncookedOnly`, `IsBetaVersion: true`, and no content bundle. Compile using the UE editor modules from `Portal.Build.cs`.

## Serialized assets across updates

Old custom node classes can be migrated through `MigrateLegacyGraph`. The current native-safe graph representation should not depend on those legacy classes once saved, but users must verify compiler equivalence for their actual Blueprints.

## Update QA priority

Test action search, pin menus, Shift+R shortcut, Input/Output counts, same-graph scoping, orphan recovery, save/reopen, Undo/Redo, and uninstall fallback. Record actual execution results instead of treating planned test cases as successful tests.

## Implementation references

- [PortalNativeUtils.cpp](https://github.com/mippi-the-dork/Portal/blob/main/Source/Portal/Private/PortalNativeUtils.cpp)
- [PortalModule.cpp](https://github.com/mippi-the-dork/Portal/blob/main/Source/Portal/Private/PortalModule.cpp)
- [PortalInputProcessor.cpp](https://github.com/mippi-the-dork/Portal/blob/main/Source/Portal/Private/PortalInputProcessor.cpp)
- [Native-Safe Architecture](https://github.com/mippi-the-dork/Portal/blob/main/Doc/Native-Safe-Architecture.md)

**Verification:** Source-reviewed; live UE Editor automation/manual validation remains separate.

For actual steps and expected behavior, [read Design](DESIGN.md).
