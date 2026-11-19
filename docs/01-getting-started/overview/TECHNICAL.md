# What Portal Does — Technical

**Portal v0.12.0 · UE 5.8.x · Win64 · Blueprint Editor**

## Runtime ownership

Portal's current persisted nodes are native `UK2Node_Knot` reroutes with ordinary pin connections. `PortalNativeUtils::ReadPortalData` and `WritePortalData` attach role, family GUID, graph GUID, name, and color metadata. The compiled Blueprint uses Unreal's existing reroute semantics; no Portal runtime subsystem is involved.

## Editor integration

`FPortalModule` registers a native graph node factory, pin-context-menu extensions, K2 node menu entries and a Slate input processor. A low-frequency ticker invokes migration/repair for loaded Blueprint assets and refreshes the action database when Portal family data changes.

## Source ownership

Use `PortalNativeUtils.cpp` for state, fallback, linking, orphan detection and migration; `PortalModule.cpp` for context menus; `PortalInputProcessor.cpp` for shortcut input; and `SPortalNativeKnotNode.cpp` for the editor-only presentation.

## Why the distinction matters

The old `UK2Node_PortalDeclaration`/`UK2Node_PortalUsage` classes still exist as migration readers. Current newly saved Portal families are not supposed to depend on those custom node classes. This is an implementation intent grounded in current code, not an assertion that every possible graph has passed uninstall QA.

## Implementation references

- [PortalNativeUtils.cpp](https://github.com/mippi-the-dork/Portal/blob/main/Source/Portal/Private/PortalNativeUtils.cpp)
- [PortalModule.cpp](https://github.com/mippi-the-dork/Portal/blob/main/Source/Portal/Private/PortalModule.cpp)
- [PortalInputProcessor.cpp](https://github.com/mippi-the-dork/Portal/blob/main/Source/Portal/Private/PortalInputProcessor.cpp)
- [Native-Safe Architecture](https://github.com/mippi-the-dork/Portal/blob/main/Doc/Native-Safe-Architecture.md)

**Verification:** Source-reviewed; live UE Editor automation/manual validation remains separate.

For actual steps and expected behavior, [read Design](DESIGN.md).
