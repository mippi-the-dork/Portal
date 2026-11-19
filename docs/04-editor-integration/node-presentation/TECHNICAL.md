# Node Presentation and Editing — Technical

**Portal v0.12.0 · UE 5.8.x · Win64 · Blueprint Editor**

## Widget class

`SPortalNativeKnotNode` subclasses the native graph node presentation, constructs a mirrored horizontal row, and uses `SInlineEditableTextBlock` for Input renaming. Tooltip, text tint, icon and Output count are read dynamically from `PortalNativeUtils`.

## Hidden bridge geometry

The Input hides its internal output-pin widget; the Output hides its internal input-pin widget. Neither changes the saved `UEdGraphPin` objects or link relationships.

## Readonly Output label

`IsNameReadOnly` and `OnPortalNameCommitted` govern inline editing: rename requests update a valid Input family rather than arbitrarily mutating an Output's cached name.

## Source files

`SPortalNativeKnotNode.cpp`, `SPortalGraphNode.cpp`, and `PortalNodeFactory.cpp` own rendering and factory selection. The latter two should not be mistaken for independent saved Blueprint node classes.

## Implementation references

- [PortalNativeUtils.cpp](https://github.com/mippi-the-dork/Portal/blob/main/Source/Portal/Private/PortalNativeUtils.cpp)
- [PortalModule.cpp](https://github.com/mippi-the-dork/Portal/blob/main/Source/Portal/Private/PortalModule.cpp)
- [PortalInputProcessor.cpp](https://github.com/mippi-the-dork/Portal/blob/main/Source/Portal/Private/PortalInputProcessor.cpp)
- [Native-Safe Architecture](https://github.com/mippi-the-dork/Portal/blob/main/Doc/Native-Safe-Architecture.md)

**Verification:** Source-reviewed; live UE Editor automation/manual validation remains separate.

For actual steps and expected behavior, [read Design](DESIGN.md).
