# Finding and Selecting Families — Technical

**Portal v0.12.0 · UE 5.8.x · Win64 · Blueprint Editor**

## Slate double-click behavior

`SPortalNativeKnotNode::OnMouseButtonDoubleClick` handles Input and Output navigation. A valid Output can call `FKismetEditorUtilities::BringKismetToFocusAttentionOnObject` on its Input; a selected Input can enumerate associated Outputs via `GetUsages` and add them to selection.

## Context-menu selection

`FPortalModule::AddPortalContextEntries` registers **Select Portal Family** for valid Inputs or nonorphan Outputs. It resolves the Input, then adds it and each Output to selection in the same owning graph.

## Failure case

`FindDeclarationForUsage` can fail if graph identity and physical links disagree. Portal exposes repair/retarget paths rather than permitting an Output to jump to a similarly named Input in a different graph.

## Implementation references

- [PortalNativeUtils.cpp](https://github.com/mippi-the-dork/Portal/blob/main/Source/Portal/Private/PortalNativeUtils.cpp)
- [PortalModule.cpp](https://github.com/mippi-the-dork/Portal/blob/main/Source/Portal/Private/PortalModule.cpp)
- [PortalInputProcessor.cpp](https://github.com/mippi-the-dork/Portal/blob/main/Source/Portal/Private/PortalInputProcessor.cpp)
- [Native-Safe Architecture](https://github.com/mippi-the-dork/Portal/blob/main/Doc/Native-Safe-Architecture.md)

**Verification:** Source-reviewed; live UE Editor automation/manual validation remains separate.

For actual steps and expected behavior, [read Design](DESIGN.md).
