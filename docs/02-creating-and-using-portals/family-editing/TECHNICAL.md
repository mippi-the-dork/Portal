# Renaming, Retargeting, and Deleting Families — Technical

**Portal v0.12.0 · UE 5.8.x · Win64 · Blueprint Editor**

## Family operations

`PortalNativeUtils::SetPortalName` updates Input metadata and family presentation. `RetargetUsage` checks compatibility and rewires the hidden native bridge without intentionally breaking the Output's downstream external links.

## Transactions

`DeletePortalFamily` uses `FScopedTransaction`, modifies the graph, breaks affected pin links, removes nodes and marks the Blueprint modified. InputProcessor intercepts Delete only for a single selected Input with Outputs in an editable K2 graph panel under the cursor.

## Delete prompt

`FPortalInputProcessor::HandleKeyDownEvent` explicitly calls a three-option `FMessageDialog::Open(EAppMsgType::YesNoCancel)`; the No branch removes only the Input, leaving its Outputs for recovery.

## Caveat

Retargeting depends on native Blueprint schema checks and the eligible target Input. No arbitrary cross-graph migration or execution rerouting is performed.

## Implementation references

- [PortalNativeUtils.cpp](https://github.com/mippi-the-dork/Portal/blob/main/Source/Portal/Private/PortalNativeUtils.cpp)
- [PortalModule.cpp](https://github.com/mippi-the-dork/Portal/blob/main/Source/Portal/Private/PortalModule.cpp)
- [PortalInputProcessor.cpp](https://github.com/mippi-the-dork/Portal/blob/main/Source/Portal/Private/PortalInputProcessor.cpp)
- [Native-Safe Architecture](https://github.com/mippi-the-dork/Portal/blob/main/Doc/Native-Safe-Architecture.md)

**Verification:** Source-reviewed; live UE Editor automation/manual validation remains separate.

For actual steps and expected behavior, [read Design](DESIGN.md).
