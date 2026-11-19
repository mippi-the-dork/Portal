# Troubleshooting and Recovery — Technical

**Portal v0.12.0 · UE 5.8.x · Win64 · Blueprint Editor**

## Diagnostic routing

| Symptom | Inspect |
| --- | --- |
| Actions absent | module startup, `UPortalNodeSpawner`, graph schema |
| Pin action absent | `ExtendPortalPinContextMenu`, pin type and direction |
| Orphan | `IsOrphanUsage`, graph/family GUIDs, physical bridge |
| Wrong family | `CanRetargetUsageToDeclaration`, `RetargetUsage` |
| Duplicate copied family | `RepairGraph` duplicate declaration GUID logic |
| Legacy nodes | `MigrateLegacyGraph` and loaded Blueprint repair ticker |
| Exec-typed Portal | `IsExecKnot` and demotion in `RepairGraph` |
| Uninstall fallback | serialized class must be `UK2Node_Knot` |

## Reproduction baseline

Use one editable Blueprint graph with a float Input and two Outputs, then change one dimension per test: same-graph/following function, exact pin type, copied family, missing Input or changed source. Save/reopen and compile at each step.

## Instrumentation boundary

The current project includes no documented automated test results for all such scenarios. A test plan is supplied in this website, with unrun states clearly marked.

## Implementation references

- [PortalNativeUtils.cpp](https://github.com/mippi-the-dork/Portal/blob/main/Source/Portal/Private/PortalNativeUtils.cpp)
- [PortalModule.cpp](https://github.com/mippi-the-dork/Portal/blob/main/Source/Portal/Private/PortalModule.cpp)
- [PortalInputProcessor.cpp](https://github.com/mippi-the-dork/Portal/blob/main/Source/Portal/Private/PortalInputProcessor.cpp)
- [Native-Safe Architecture](https://github.com/mippi-the-dork/Portal/blob/main/Doc/Native-Safe-Architecture.md)

**Verification:** Source-reviewed; live UE Editor automation/manual validation remains separate.

For actual steps and expected behavior, [read Design](DESIGN.md).
