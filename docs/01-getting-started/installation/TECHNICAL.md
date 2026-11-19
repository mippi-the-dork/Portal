# Installation and Compatibility — Technical

**Portal v0.12.0 · UE 5.8.x · Win64 · Blueprint Editor**

## Plugin descriptor

`Portal.uplugin` declares `VersionName: 0.12.0`, `EngineVersion: 5.8.0`, `SupportedTargetPlatforms: [Win64]`, and an `UncookedOnly` module named `Portal`, with `LoadingPhase: Default`. `CanContainContent` is false and `IsBetaVersion` is true.

## Build dependencies

`Portal.Build.cs` includes BlueprintGraph, GraphEditor, Kismet, KismetCompiler, UnrealEd, Slate, ToolMenus, InputCore and standard Core/Engine dependencies. The editor uses native K2 graph types and a graph node factory. No game module should depend on Portal APIs for runtime execution.

## Activation and teardown

`FPortalModule::StartupModule` registers the node factory, graph menu extender, input preprocessor, tool-menu startup callback and a ticker. `ShutdownModule` unregisters these resources. Editor lifecycle and loading compatibility require testing in the specific target UE minor version.

## Implementation references

- [PortalNativeUtils.cpp](https://github.com/mippi-the-dork/Portal/blob/main/Source/Portal/Private/PortalNativeUtils.cpp)
- [PortalModule.cpp](https://github.com/mippi-the-dork/Portal/blob/main/Source/Portal/Private/PortalModule.cpp)
- [PortalInputProcessor.cpp](https://github.com/mippi-the-dork/Portal/blob/main/Source/Portal/Private/PortalInputProcessor.cpp)
- [Native-Safe Architecture](https://github.com/mippi-the-dork/Portal/blob/main/Doc/Native-Safe-Architecture.md)

**Verification:** Source-reviewed; live UE Editor automation/manual validation remains separate.

For actual steps and expected behavior, [read Design](DESIGN.md).
