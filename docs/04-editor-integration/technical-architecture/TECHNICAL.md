# Editor Module and Data Architecture — Technical

**Portal v0.12.0 · UE 5.8.x · Win64 · Blueprint Editor**

## Module graph

`FPortalModule` registers `FPortalNodeFactory`, native reroute context menus, graph pin menu extenders, input preprocessing, action refresh and `TickRepair`. `PortalNativeUtils` owns family data and native link operations; `UPortalNodeSpawner` integrates with the action database; `SPortalNativeKnotNode` owns the compact editor presentation.

## Saved state

The authoritative objects are native `UK2Node_Knot` instances in `UEdGraph`. Package-owned metadata records role (`Declaration`/`Usage`), GUIDs, label and color. The fallback `NodeComment` marker makes some clipboard/metadata recovery possible.

## Editor safety and shutdown

`ShutdownModule` unregisters ticker, input preprocessor, menu extender, node factory and ToolMenus callbacks. No packaged-game module is declared. Regression tests should include module reload/disable, Blueprint compile and save/reopen under UE 5.8.x.

## Performance note

`TickRepair` scans loaded Blueprint objects on a 0.75-second ticker; it is not a per-frame graph renderer pass. It can still have project-size cost and should be measured with many loaded Blueprints rather than called free.

## Implementation references

- [PortalNativeUtils.cpp](https://github.com/mippi-the-dork/Portal/blob/main/Source/Portal/Private/PortalNativeUtils.cpp)
- [PortalModule.cpp](https://github.com/mippi-the-dork/Portal/blob/main/Source/Portal/Private/PortalModule.cpp)
- [PortalInputProcessor.cpp](https://github.com/mippi-the-dork/Portal/blob/main/Source/Portal/Private/PortalInputProcessor.cpp)
- [Native-Safe Architecture](https://github.com/mippi-the-dork/Portal/blob/main/Doc/Native-Safe-Architecture.md)

**Verification:** Source-reviewed; live UE Editor automation/manual validation remains separate.

For actual steps and expected behavior, [read Design](DESIGN.md).
