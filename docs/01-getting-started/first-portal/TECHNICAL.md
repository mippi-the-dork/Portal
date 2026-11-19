# Your First Portal Family — Technical

**Portal v0.12.0 · UE 5.8.x · Win64 · Blueprint Editor**

## Pin context menus

`FPortalModule::ExtendPortalPinContextMenu` inspects `UEdGraphPin` direction, graph schema and `CanCreatePortalFromOutputPin` / `CanConnectPortalToInputPin`. Valid output-pin creation calls `CreatePortalFromOutputPin`; compatible input-pin creation calls `CreateUsageAndConnectToInputPin`.

## Position capture

`FPortalInputProcessor` caches the exact graph-space right-click position before the popup opens. Context-menu action lambdas use that position when creating endpoints near the selected pin rather than guessing from menu window coordinates.

## Serialized result

`PortalNativeUtils::CreateKnot` constructs native `UK2Node_Knot` nodes. Input and Output identity is written to package metadata with a readable `NodeComment` fallback; the physical bridge is a real native pin link.

## First-use test coverage

Test wildcard and typed data pins, a second Output, graph save/reopen, Undo/Redo, and compiler behavior. None of these steps proves cross-graph routing; that feature is intentionally unsupported.

## Implementation references

- [PortalNativeUtils.cpp](https://github.com/mippi-the-dork/Portal/blob/main/Source/Portal/Private/PortalNativeUtils.cpp)
- [PortalModule.cpp](https://github.com/mippi-the-dork/Portal/blob/main/Source/Portal/Private/PortalModule.cpp)
- [PortalInputProcessor.cpp](https://github.com/mippi-the-dork/Portal/blob/main/Source/Portal/Private/PortalInputProcessor.cpp)
- [Native-Safe Architecture](https://github.com/mippi-the-dork/Portal/blob/main/Doc/Native-Safe-Architecture.md)

**Verification:** Source-reviewed; live UE Editor automation/manual validation remains separate.

For actual steps and expected behavior, [read Design](DESIGN.md).
