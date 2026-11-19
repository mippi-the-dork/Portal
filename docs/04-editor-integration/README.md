# 04. Editor Integration

Portal integrates with the existing Blueprint graph editor through action menus, Slate graph node drawing, an input processor, and metadata repair. These guides distinguish shipped user-facing controls from source classes that are present but not wired to an accessible editor window.

## What to know before proceeding

Every Portal route is **data-only and graph-local**. Native Blueprint pins and links remain the saved source of truth. The editor-only Portal UI is not a runtime data store.

## Editor Integration topics

| Topic | Covers | Design | Technical |
| --- | --- | --- | --- |
| **Blueprint Action Browser and Context Menus** | Find graph-local actions by name and type. | [Design](action-browser/DESIGN.md) | [Technical](action-browser/TECHNICAL.md) |
| **Node Presentation and Editing** | Understand naming, counts, icon placement, and editor-only styling. | [Design](node-presentation/DESIGN.md) | [Technical](node-presentation/TECHNICAL.md) |
| **Shortcuts and Portal Browser Status** | Use confirmed shortcuts and understand the Browser implementation boundary. | [Design](shortcuts-and-browser/DESIGN.md) | [Technical](shortcuts-and-browser/TECHNICAL.md) |
| **Editor Module and Data Architecture** | How the editor-side systems cooperate without a runtime dependency. | [Design](technical-architecture/DESIGN.md) | [Technical](technical-architecture/TECHNICAL.md) |

**Design** explains user steps and expected results; **Technical** explains native Blueprint APIs, serialized state, and limitations.
