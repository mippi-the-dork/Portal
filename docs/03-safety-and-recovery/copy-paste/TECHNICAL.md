# Copy/Paste, Undo, and Graph Integrity — Technical

**Portal v0.12.0 · UE 5.8.x · Win64 · Blueprint Editor**

## Metadata fallback

Package `FMetaData` holds detailed family identity, but native copy/paste may omit that package metadata. A readable Portal `NodeComment` marker retains role and name information to allow later reconstruction.

## Duplicate GUID repair

`RepairGraph` groups Inputs by family GUID. For collisions it assigns fresh IDs to duplicate declarations and propagates identity to physically linked Outputs, using graph GUID to keep the family local.

## Transactions

Creation/deletion/retarget/convert helpers use `FScopedTransaction`, `Modify`, `NotifyGraphChanged` and Blueprint modification signals. The exact behavior with external graph editing plugins is not established by these source paths alone.

## Failure modes

A copied lone Output might have insufficient data to recover its old Input; a pasted graph has to satisfy graph identity and pin compatibility. Portal does not invent an upstream source or bridge between different graphs.

## Implementation references

- [PortalNativeUtils.cpp](https://github.com/mippi-the-dork/Portal/blob/main/Source/Portal/Private/PortalNativeUtils.cpp)
- [PortalModule.cpp](https://github.com/mippi-the-dork/Portal/blob/main/Source/Portal/Private/PortalModule.cpp)
- [PortalInputProcessor.cpp](https://github.com/mippi-the-dork/Portal/blob/main/Source/Portal/Private/PortalInputProcessor.cpp)
- [Native-Safe Architecture](https://github.com/mippi-the-dork/Portal/blob/main/Doc/Native-Safe-Architecture.md)

**Verification:** Source-reviewed; live UE Editor automation/manual validation remains separate.

For actual steps and expected behavior, [read Design](DESIGN.md).
