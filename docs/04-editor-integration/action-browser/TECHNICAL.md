# Blueprint Action Browser and Context Menus — Technical

**Portal v0.12.0 · UE 5.8.x · Win64 · Blueprint Editor**

## Action database integration

`UPortalNodeSpawner` registers Input and context-specific Output actions and provides search keywords and tooltip information. `IsTemplateNodeFilteredOut` checks graph schema/context so unrelated graphs do not list invalid family Outputs.

## Refresh model

The module builds a signature over loaded Blueprint Inputs, family GUIDs, graph paths, types and counts. When the signature changes, the ticker refreshes Blueprint action class entries. Thus changes may rely on an editor refresh rather than appearing as global always-on actions.

## Node spawner details

The spawner advertises legacy Portal classes to satisfy the action database's class registration contract, but `Invoke` creates/migrates the placeholder into current native reroutes. Avoid documenting legacy class names as new serialized node types.

## Implementation references

- [PortalNativeUtils.cpp](https://github.com/mippi-the-dork/Portal/blob/main/Source/Portal/Private/PortalNativeUtils.cpp)
- [PortalModule.cpp](https://github.com/mippi-the-dork/Portal/blob/main/Source/Portal/Private/PortalModule.cpp)
- [PortalInputProcessor.cpp](https://github.com/mippi-the-dork/Portal/blob/main/Source/Portal/Private/PortalInputProcessor.cpp)
- [Native-Safe Architecture](https://github.com/mippi-the-dork/Portal/blob/main/Doc/Native-Safe-Architecture.md)

**Verification:** Source-reviewed; live UE Editor automation/manual validation remains separate.

For actual steps and expected behavior, [read Design](DESIGN.md).
