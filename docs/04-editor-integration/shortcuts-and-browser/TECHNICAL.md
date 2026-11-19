# Shortcuts and Portal Browser Status — Technical

**Portal v0.12.0 · UE 5.8.x · Win64 · Blueprint Editor**

## Input processor

`FPortalInputProcessor::HandleKeyDownEvent` records `R` key state and intercepts selected-Input Delete. `HandleMouseButtonDownEvent` tests Shift, left mouse, held R, editable `SGraphPanel`, K2 schema, and no Ctrl/Alt before creating an Input/Output at the pointer's graph-space location.

## Slate browser source versus registration

`SPortalBrowser::Construct` creates a search/list panel and buttons; its implementation can create Inputs and focus family endpoints. A source-wide search of `FPortalModule` does **not** expose a registered Nomad tab spawner, menu action or other user invocation of `SPortalBrowser`. This is a source-grounded limitation, not a claim that the widget does not exist in code.

## Lifecycle

The input processor is registered with `FSlateApplication` at module startup and unregistered on shutdown. Graph panel discovery uses the widget path under the cursor; non-graph window focus must not trigger accidental creation.

## Implementation references

- [PortalNativeUtils.cpp](https://github.com/mippi-the-dork/Portal/blob/main/Source/Portal/Private/PortalNativeUtils.cpp)
- [PortalModule.cpp](https://github.com/mippi-the-dork/Portal/blob/main/Source/Portal/Private/PortalModule.cpp)
- [PortalInputProcessor.cpp](https://github.com/mippi-the-dork/Portal/blob/main/Source/Portal/Private/PortalInputProcessor.cpp)
- [Native-Safe Architecture](https://github.com/mippi-the-dork/Portal/blob/main/Doc/Native-Safe-Architecture.md)

**Verification:** Source-reviewed; live UE Editor automation/manual validation remains separate.

For actual steps and expected behavior, [read Design](DESIGN.md).
