# Inputs, Outputs, and Counts — Technical

**Portal v0.12.0 · UE 5.8.x · Win64 · Blueprint Editor**

## Presentation source

`SPortalNativeKnotNode::UpdateGraphNode` builds mirrored rows for the Input (`pin | icon | name | count`) and Output (`count | name | icon | pin`). `GetPortalCountText` calculates family Output count by calling `PortalNativeUtils::GetUsages`.

## Identity versus presentation

`FPortalKnotData` stores role, family GUID, source graph GUID, name and color. GUID identity is authoritative for family membership; name is human-readable. Output presentation is synchronized from the Input during `RepairGraph`.

## Why the hidden wire stays native

The Slate graph-node widget omits the internal Input output-pin and Output input-pin visual widgets, while their `UEdGraphPin` instances and links remain valid. The Blueprint compiler receives ordinary reroute graphs.

## Implementation references

- [PortalNativeUtils.cpp](https://github.com/mippi-the-dork/Portal/blob/main/Source/Portal/Private/PortalNativeUtils.cpp)
- [PortalModule.cpp](https://github.com/mippi-the-dork/Portal/blob/main/Source/Portal/Private/PortalModule.cpp)
- [PortalInputProcessor.cpp](https://github.com/mippi-the-dork/Portal/blob/main/Source/Portal/Private/PortalInputProcessor.cpp)
- [Native-Safe Architecture](https://github.com/mippi-the-dork/Portal/blob/main/Doc/Native-Safe-Architecture.md)

**Verification:** Source-reviewed; live UE Editor automation/manual validation remains separate.

For actual steps and expected behavior, [read Design](DESIGN.md).
