<div align="center">
  <img src="Resources/Icon128.png" alt="Portal plugin icon" width="90">

# Portal
### Cleaner Blueprint graphs. Same native connections.

**Named, reusable data routes for Unreal Engine Blueprints—without the cross-graph wire clutter.**

**Unreal Engine 5.8.0–5.8.3 · Windows 64-bit · Editor-only · v0.12.0 release candidate**

[**Explore the Documentation Website**](https://mippi-the-dork.github.io/Portal/) · [Getting Started](https://mippi-the-dork.github.io/Portal/?page=01-getting-started/README.md) · [Report an Issue](https://github.com/mippi-the-dork/Portal/issues/new)

</div>

---

## Your Blueprint logic shouldn't get lost in the wires

Blueprint graphs can become difficult to read when a useful value needs to travel across the screen to several distant nodes. Long wires snake through unrelated logic, reroutes multiply, and following a connection means panning back and forth.

**Portal gives those data connections a name.** Place one **Portal Input** near the source and any number of **Portal Outputs** where the value is needed within the same graph. Each endpoint is identifiable, and the editor keeps the long connecting bridge out of the way.

The graph becomes easier to scan, maintain, and explain to someone opening it for the first time.

## What Portal brings to Blueprints

- **Named data routing.** Replace hard-to-follow long data wires with readable Portal Input and Output endpoints.
- **One source, multiple destinations.** Share an Input with as many compatible Outputs as you need in that Blueprint graph. A visible count shows how many Outputs belong to the family.
- **Fast, contextual creation.** Create Portals from the Blueprint action menu, directly from compatible data pins, or with **Shift + R + Left Click**.
- **Easy navigation.** Double-click an Output to jump to its Input, double-click an Input to select its Outputs, or use **Select Portal Family**.
- **Flexible editing.** Rename a family, create additional Outputs, retarget a compatible Output, and delete or convert a family intentionally.
- **Orphan recovery.** If an Input goes missing, reconnect its Outputs, recreate the missing Input, or turn an orphan into an ordinary reroute.
- **Native-safe Blueprint assets.** Portal saves stock Unreal reroute nodes and standard Blueprint pin links—not a special runtime dependency.

## The difference is in what gets saved

Portal isn't just a custom visual node. Its native-safe design is intended to ensure your Blueprint doesn't depend on the plugin to retain its data routing.

**With Portal enabled:** a source and its distant consumers appear as named Input and Output nodes without a long visible bridging wire.

**Without Portal:** those same saved connections appear as ordinary native Blueprint reroutes and their normal wires. The Portal-specific presentation disappears, but the saved graph does not require a custom Portal node class to load.

That means Portal helps you organize a graph **without making its logic dependent on a proprietary Blueprint node format**.

[**How native-safe Portals work →**](https://mippi-the-dork.github.io/Portal/?page=03-safety-and-recovery/native-safe/DESIGN.md) · [Technical architecture](https://mippi-the-dork.github.io/Portal/?page=03-safety-and-recovery/native-safe/TECHNICAL.md)

## Work faster, from creation to cleanup

| When you want to… | Portal helps you… |
| --- | --- |
| Route a value across a large graph | Create a named Input at the source and an Output near the consumer |
| Use the same value in several places | Add multiple Outputs to one Input |
| Find where a value originates | Jump directly from an Output to its Input |
| Reorganize a Blueprint | Rename, select, or retarget an entire Portal family |
| Recover a broken family | Reconnect orphaned Outputs or recreate their missing Input |
| Stop using Portal | Convert families to native reroutes, or let Unreal show the native reroutes if the plugin is absent |

## Learn Portal in the documentation

**The dedicated documentation website is the source for setup, controls, workflows, technical details, and troubleshooting.** This README introduces the plugin; the documentation explains it.

**[OPEN THE PORTAL DOCUMENTATION WEBSITE](https://mippi-the-dork.github.io/Portal/)**

| Guide | What you'll learn |
| --- | --- |
| [Installation and compatibility](https://mippi-the-dork.github.io/Portal/?page=01-getting-started/installation/DESIGN.md) | Supported Unreal versions and how to install Portal |
| [Your first Portal family](https://mippi-the-dork.github.io/Portal/?page=01-getting-started/first-portal/DESIGN.md) | Create, connect, and navigate a working Input/Output pair |
| [Creating Portals from menus and pins](https://mippi-the-dork.github.io/Portal/?page=02-creating-and-using-portals/creation-paths/DESIGN.md) | Choose the fastest creation workflow |
| [Inputs, Outputs, and counts](https://mippi-the-dork.github.io/Portal/?page=02-creating-and-using-portals/inputs-and-outputs/DESIGN.md) | Understand Portal family layout and membership |
| [Renaming, retargeting, and deleting](https://mippi-the-dork.github.io/Portal/?page=02-creating-and-using-portals/family-editing/DESIGN.md) | Safely change or remove existing families |
| [Orphaned Outputs and recovery](https://mippi-the-dork.github.io/Portal/?page=03-safety-and-recovery/orphans/DESIGN.md) | Repair lost source relationships |
| [Frequently asked questions](https://mippi-the-dork.github.io/Portal/?page=05-support/faq/DESIGN.md) | Clear answers to common behavior questions |
| [Technical documentation](https://mippi-the-dork.github.io/Portal/?page=04-editor-integration/README.md) | Editor hooks, native Blueprint data, and implementation |

You can also [read the Markdown documentation directly on GitHub](https://github.com/mippi-the-dork/Portal/tree/main/docs).

## Compatibility and limitations

Portal **v0.12.0 is a release candidate**, and the plugin descriptor currently marks it as beta.

| | |
| --- | --- |
| **Engine** | Unreal Engine 5.8.0–5.8.3 |
| **Platform** | Windows 64-bit |
| **Environment** | Unreal Editor only (`UncookedOnly`) |
| **Graphs** | Unreal Blueprint graphs |
| **Connections** | Compatible data pins; one Portal family per graph |
| **Engine modifications** | Not required |

**What Portal does not do:** It does not route execution wires, share values between separate graphs or Blueprints, create variables or persistent data storage, or add a runtime gameplay system. Portals are graph-local, data-only editing tools. Native Unreal pin compatibility rules still apply.

See [Graph Scope and Type Compatibility](https://mippi-the-dork.github.io/Portal/?page=02-creating-and-using-portals/graph-boundaries/DESIGN.md) and [Troubleshooting](https://mippi-the-dork.github.io/Portal/?page=05-support/troubleshooting/DESIGN.md) for examples of unsupported cases and recovery workflows.

## Help shape Portal

**Found a bug?** [Report an issue](https://github.com/mippi-the-dork/Portal/issues/new) with your Unreal Engine version, Portal version, reproduction steps, expected behavior, and relevant logs or screenshots.

**Have an idea?** [Request a feature](https://github.com/mippi-the-dork/Portal/issues/new) and describe the Blueprint workflow it would improve. Suggestions and bug reports help prioritize future work.

**Want to contribute?** [Open a pull request](https://github.com/mippi-the-dork/Portal/pulls) with a focused explanation of the change, its intended behavior, and any tests performed. For larger changes, start with an issue so the direction can be discussed first.

You can follow [the Portal roadmap](https://mippi-the-dork.github.io/Portal/?page=ROADMAP.md) for documented release history and future directions. Proposed work should not be treated as shipped functionality.

## License

Portal is available under the **MIT License**. See [LICENSE](LICENSE) for the full terms.

---

<div align="center">

**Spend less time tracing wires. Spend more time building Blueprint logic.**

[**Read the Documentation**](https://mippi-the-dork.github.io/Portal/) · [GitHub Issues](https://github.com/mippi-the-dork/Portal/issues) · [License](LICENSE)

</div>
