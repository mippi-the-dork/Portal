# Frequently Asked Questions — Technical

**Portal v0.12.0 · UE 5.8.x · Win64 · Blueprint Editor**

## FAQ cross-reference by implementation

| Question | Implementation |
| --- | --- |
| Graph boundaries | `FPortalKnotData::SourceGraphGuid`; native K2 schema checks |
| Type eligibility | `CanCreatePortalFromOutputPin`, `CanConnectPortalToInputPin` |
| Count | `SPortalNativeKnotNode::GetPortalCountText` and `GetUsages` |
| Uninstall fallback | `UK2Node_Knot` storage and Slate bridge suppression |
| Orphans | `IsOrphanUsage`, `CreateMissingInputForUsage`, `RetargetUsage` |
| Clipboard repair | `NodeComment` fallback, `RepairGraph` duplicate GUID handling |
| Browser availability | `SPortalBrowser.cpp` exists; module registration not found |

## Verification policy

These answers are grounded in Portal's 0.12.0 README and source; they are not a substitute for an executed UE Editor regression matrix. Where support depends on a particular project configuration, test that project rather than treating a descriptive guarantee as a test result.

## Implementation references

- [PortalNativeUtils.cpp](https://github.com/mippi-the-dork/Portal/blob/main/Source/Portal/Private/PortalNativeUtils.cpp)
- [PortalModule.cpp](https://github.com/mippi-the-dork/Portal/blob/main/Source/Portal/Private/PortalModule.cpp)
- [PortalInputProcessor.cpp](https://github.com/mippi-the-dork/Portal/blob/main/Source/Portal/Private/PortalInputProcessor.cpp)
- [Native-Safe Architecture](https://github.com/mippi-the-dork/Portal/blob/main/Doc/Native-Safe-Architecture.md)

**Verification:** Source-reviewed; live UE Editor automation/manual validation remains separate.

For actual steps and expected behavior, [read Design](DESIGN.md).
