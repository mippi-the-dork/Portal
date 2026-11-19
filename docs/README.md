# Portal Documentation

**Portal v0.12.0 · Unreal Engine 5.8.0–5.8.3 · Win64 · Editor-only / UncookedOnly**

Portal brings **named reroute-style data routing** to Unreal Engine Blueprint graphs. Place a named Input near the source, then create Outputs wherever that value is needed in the **same graph**. The long bridge wire disappears in the editor, while the underlying Blueprint still contains native reroutes and normal connections.

## What can I do with Portal?

<figure class="doc-media" data-kind="image" data-media-id="portal-home-hero">
  <img src="assets/media-placeholder.png" alt="Example of Portal Input and Outputs in a Blueprint graph" loading="lazy">
  <figcaption>Portal replaces distant data wires with clearly named endpoints while preserving native Blueprint connections.<span class="media-capture-instructions"><strong>Capture instructions · Image:</strong> Capture a clean wide Unreal Blueprint graph: a data source connected to a named Portal Input, two Outputs connected to distant consumers, and enough surrounding nodes to demonstrate the reduction in wire clutter. Use a sharp 16:9 view with the names and count legible.</span></figcaption>
</figure>

- **Make busy graphs easier to read:** replace visible long data wires with named Input and Output endpoints.
- **Reuse one source:** one Input can have multiple Outputs, with a visible family Output count.
- **Create quickly:** from empty graph space, directly from data pins, or by holding **Shift + R** and clicking.
- **Navigate and edit families:** jump between endpoints, select a family, retarget an Output, or delete an entire family deliberately.
- **Protect Blueprint compatibility:** native `UK2Node_Knot` nodes and physical wires remain when Portal is no longer installed.

## Get started in three steps

1. [Install Portal](01-getting-started/installation/DESIGN.md) in a supported Unreal Engine Editor project.
2. [Create a first family](01-getting-started/first-portal/DESIGN.md) from a compatible data output pin.
3. [Explore creation and editing](02-creating-and-using-portals/README.md) to handle multiple Outputs, shortcuts and navigation.

## What are the important limits?

Portal is **data-only**. It does **not** support execution pins, cross-graph routing, variable storage, or packaged runtime components. A Portal Input in the Event Graph cannot feed an Output in a function or another Blueprint. Current documentation describes the **0.12.0 release candidate**, not a declared final 1.0 release.

## How to use this documentation

Each topic has a **Design guide** for controls, step-by-step workflows, and practical troubleshooting and a **Technical reference** for C++ ownership, editor callbacks, data model, and engine constraints. They complement rather than duplicate one another.

## Explore the manual

| Category | Topics | What you'll find |
| --- | ---: | --- |
| [01. Getting Started](01-getting-started/README.md) | 3 | Understand Portal, install it, and route your first Blueprint data pin. |
| [02. Creating and Using Portals](02-creating-and-using-portals/README.md) | 5 | Create Inputs and Outputs, organize families, and navigate graphs. |
| [03. Native Safety and Recovery](03-safety-and-recovery/README.md) | 4 | Understand saved reroutes, repair orphans, migration, and safe editing. |
| [04. Editor Integration](04-editor-integration/README.md) | 4 | Blueprint actions, graph widgets, shortcuts, and implementation architecture. |
| [05. Support and Troubleshooting](05-support/README.md) | 3 | Resolve common issues, check compatibility, and follow future development. |

## Roadmap and documentation quality

The [Roadmap](ROADMAP.md) records the historical move to native-safe reroutes and the current release-candidate baseline. No future Portal releases or features are promised without an approved plan. The [Documentation Audit](DOCUMENTATION_AUDIT.md) and [Verification Matrix](VERIFICATION_MATRIX.md) track what still needs editor and website testing.

## Project links

- [Portal source](https://github.com/mippi-the-dork/Portal)
- [Portal issues](https://github.com/mippi-the-dork/Portal/issues)
- [License](https://github.com/mippi-the-dork/Portal/blob/main/LICENSE)

**Documentation basis:** Portal v0.12.0 README, plugin descriptor and C++ implementation. Source review is not a claim that the complete UE 5.8 test matrix has passed.
