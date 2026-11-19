# Creating Portals from Menus and Pins — Technical

**Portal v0.12.0 · UE 5.8.x · Win64 · Blueprint Editor**

## Action spawners

`UPortalNodeSpawner` integrates with the Blueprint action database. It registers search-friendly Input and Output actions but creates or resolves native `UK2Node_Knot` nodes through `PortalNativeUtils`, rather than preserving a newly spawned legacy Portal node as the stored type.

## Menu constraints

The schema must be `UEdGraphSchema_K2`. `CanCreatePortalFromOutputPin` excludes exec pins, and `CanConnectPortalToInputPin` checks graph, direction, existing connections, node eligibility and schema compatibility.

## Action list refresh

`FPortalModule::BuildPortalActionSignature` detects relevant Input family changes. On change, the ticker refreshes class actions so Output search entries match the graph's current Portal Inputs.

## Shortcut path

`FPortalInputProcessor::HandleMouseButtonDownEvent` requires Shift, held `R`, left mouse click, an editable K2 graph panel, and no Ctrl/Alt. The selected-family case differs from the no-selected-family case.

## Implementation references

- [PortalNativeUtils.cpp](https://github.com/mippi-the-dork/Portal/blob/main/Source/Portal/Private/PortalNativeUtils.cpp)
- [PortalModule.cpp](https://github.com/mippi-the-dork/Portal/blob/main/Source/Portal/Private/PortalModule.cpp)
- [PortalInputProcessor.cpp](https://github.com/mippi-the-dork/Portal/blob/main/Source/Portal/Private/PortalInputProcessor.cpp)
- [Native-Safe Architecture](https://github.com/mippi-the-dork/Portal/blob/main/Doc/Native-Safe-Architecture.md)

**Verification:** Source-reviewed; live UE Editor automation/manual validation remains separate.

For actual steps and expected behavior, [read Design](DESIGN.md).
