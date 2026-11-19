# Portal

**Named Data Routing for Unreal Engine Blueprints**

**Spend less time tracing wires. Spend more time building Blueprint logic.**

Portal makes large Blueprint graphs easier to understand by replacing long, distracting **data wires** with clearly named **Portal Inputs and Outputs**. Place an Input near the source, create compatible Outputs wherever that value is needed in the same graph, and keep your logic readable without changing the underlying native Blueprint connections.

[![Unreal Engine](https://img.shields.io/badge/Unreal%20Engine-5.8.x-black?logo=unrealengine)](https://www.unrealengine.com/) ![Platform](https://img.shields.io/badge/Platform-Win64-blue) ![Editor Only](https://img.shields.io/badge/Plugin-Editor%20Only-217346) ![Version](https://img.shields.io/badge/Portal-v0.12.0%20RC-blue) ![Beta](https://img.shields.io/badge/Status-Beta-orange) [![License: MIT](https://img.shields.io/badge/License-MIT-green)](LICENSE)

<p align="center">
  <img src="Resources/Icon128.png" alt="Portal plugin icon" width="128">
</p>

---

## Blueprint graphs should be about logic, not tracing wires

As Blueprints grow, a value might be used in several places across a large graph. The wires connecting everything can become harder to follow than the logic itself. Traditional reroutes help organize those wires, but the long connection still runs through the graph.

**Portal keeps the relationship without the clutter.**

- **Give connections meaningful names.** Identify where a value comes from without chasing a wire across the graph.
- **Use a value wherever you need it.** One Portal Input can supply multiple compatible Outputs in its own graph.
- **Keep the graph easy to read.** Reduce crossing wires and visual noise without creating additional variables.
- **Get to the source quickly.** Navigate between Outputs and their Input, or select an entire family to see where it is used.
- **Keep Blueprints native-safe.** Saved graphs use Unreal's standard reroute nodes and physical connections, so removing Portal does not leave missing custom node classes.

## Explore the documentation

> ### [**Portal Documentation — Full User Guide and Technical Reference**](https://mippi-the-dork.github.io/Portal/)
>
> **Everything you need to learn and use Portal, in one place.** Create your first Portal family, explore graph editing workflows, understand native-safe saving and recovery, or look up implementation details and troubleshooting steps.
>
> **[Get Started](https://mippi-the-dork.github.io/Portal/?page=01-getting-started%2FREADME.md)** · **[Creating & Using Portals](https://mippi-the-dork.github.io/Portal/?page=02-creating-and-using-portals%2FREADME.md)** · **[Native Safety & Recovery](https://mippi-the-dork.github.io/Portal/?page=03-safety-and-recovery%2FREADME.md)** · **[Troubleshooting](https://mippi-the-dork.github.io/Portal/?page=05-support%2FREADME.md)**

This README is an overview of what Portal offers. **The [documentation website](https://mippi-the-dork.github.io/Portal/) is the maintained reference for exact controls, installation, workflows and technical behavior.**

## Named Inputs. Reusable Outputs. Cleaner graphs.

| Portal feature | What you can do |
| --- | --- |
| **[Named Inputs & Outputs](https://mippi-the-dork.github.io/Portal/?page=02-creating-and-using-portals%2Finputs-and-outputs%2FDESIGN.md)** | Replace long data routes with clearly named endpoints, including a visible Output count for each family. |
| **[Fast Creation](https://mippi-the-dork.github.io/Portal/?page=02-creating-and-using-portals%2Fcreation-paths%2FDESIGN.md)** | Create Portals through the Blueprint action menu, from compatible data pins, or with **Shift + R + Left Click**. |
| **[Family Navigation](https://mippi-the-dork.github.io/Portal/?page=02-creating-and-using-portals%2Fnavigation%2FDESIGN.md)** | Jump from an Output to its Input, select the Outputs of an Input, or select an entire family. |
| **[Family Editing](https://mippi-the-dork.github.io/Portal/?page=02-creating-and-using-portals%2Ffamily-editing%2FDESIGN.md)** | Rename Inputs, add Outputs, retarget compatible Outputs, and deliberately delete a family. |
| **[Orphan Recovery](https://mippi-the-dork.github.io/Portal/?page=03-safety-and-recovery%2Forphans%2FDESIGN.md)** | Reconnect an orphaned Output, recreate its missing Input, or convert it into an ordinary reroute. |
| **[Native-Safe Connections](https://mippi-the-dork.github.io/Portal/?page=03-safety-and-recovery%2Fnative-safe%2FDESIGN.md)** | Keep saved Blueprint routing in native `UK2Node_Knot` nodes and normal wires, even when Portal is absent. |

Portal is **graph-local and data-only**. It improves how Blueprint data connections are presented and managed in the editor; it does not introduce a replacement execution or runtime data system.

## Useful in real Blueprint workflows

### Make complicated graphs easier to follow

Place a named Portal Input beside the data source, then create Outputs next to its distant consumers. A value can travel across the graph without a visible wire cutting through unrelated logic.

[**Create your first Portal family →**](https://mippi-the-dork.github.io/Portal/?page=01-getting-started%2Ffirst-portal%2FDESIGN.md)

### Reuse a value without creating another variable

When several nodes need the same compatible value, add Outputs to the existing family rather than drawing another long connection. Each Input displays its Output count so you can see how many endpoints depend on it.

### Find and reorganize connections faster

Double-click an Output to jump to its Input, or double-click an Input to select its Outputs. When a graph changes, rename the family, retarget a compatible Output, or deliberately remove the family and its connected endpoints.

[**Learn about family navigation and editing →**](https://mippi-the-dork.github.io/Portal/?page=02-creating-and-using-portals%2Ffamily-editing%2FDESIGN.md)

### Keep your Blueprints usable without Portal

With Portal installed, named endpoints hide the long internal bridge from view. Underneath, Unreal still saves **native reroute nodes and physical wires**.

If you disable or remove Portal, the saved graph exposes those standard reroutes and connections instead of requiring custom Portal node classes. Portal's names and presentation are editor-specific; your Blueprint's data routing remains native.

[**Understand native-safe saving and plugin removal →**](https://mippi-the-dork.github.io/Portal/?page=03-safety-and-recovery%2Fnative-safe%2FDESIGN.md)

## Get Portal

**Portal v0.12.0** targets **Unreal Engine 5.8.0–5.8.3 on Windows 64-bit** and runs in the **Editor only**. It is currently marked as a **beta/release candidate**.

The full source is available in this repository. There are currently no published GitHub Releases, so use the repository source and follow the installation instructions for your project.

**[Portal Source](https://github.com/mippi-the-dork/Portal)** · **[Installation Guide](https://mippi-the-dork.github.io/Portal/?page=01-getting-started%2Finstallation%2FDESIGN.md)** · **[First Portal Family](https://mippi-the-dork.github.io/Portal/?page=01-getting-started%2Ffirst-portal%2FDESIGN.md)**

## Important limitations

Portal is a **Blueprint editor workflow utility**, not a new gameplay or networking framework.

- **Data pins only.** Portal does not support execution pins. Unreal's native execution reroute behavior does not safely map to one Input feeding multiple hidden Outputs.
- **Graph-local families.** An Input and its Outputs must belong to the same Blueprint graph; they cannot connect across functions, macros, Event Graphs or other Blueprints.
- **Native pin compatibility still applies.** Outputs can only connect to appropriate data pin types.
- **No variable storage or cross-graph communication.** Portal routes existing data connections; it does not save values or substitute for Blueprint variables, functions or interfaces.
- **No packaged-game Portal runtime.** The plugin is `UncookedOnly`. Native Blueprint links remain, but Portal's editor presentation is not a runtime system.
- **Beta verification remains ongoing.** Test copy/paste, Undo/Redo, migration, orphan recovery, save/reopen and uninstall behavior in a disposable project before depending on them in production.

For further details, see [Graph Scope & Type Compatibility](https://mippi-the-dork.github.io/Portal/?page=02-creating-and-using-portals%2Fgraph-boundaries%2FDESIGN.md), [FAQ](https://mippi-the-dork.github.io/Portal/?page=05-support%2Ffaq%2FDESIGN.md) and [Troubleshooting](https://mippi-the-dork.github.io/Portal/?page=05-support%2Ftroubleshooting%2FDESIGN.md).

## What's next?

Portal's documented history includes its **native-safe architecture transition in v0.4.1** and the current **v0.12.0 release candidate**.

Future ideas and research areas are tracked separately. No specific next major version, release date or future functionality has been committed.

[**View the Portal Roadmap →**](https://mippi-the-dork.github.io/Portal/?page=ROADMAP.md)

## Issues, feature requests and contributions

Found a bug? Please [open an issue](https://github.com/mippi-the-dork/Portal/issues) with your Portal/Unreal versions, relevant graph and pin types, steps to reproduce, and screenshots, video or Output Log excerpts where helpful.

Have an idea? [Submit a feature request](https://github.com/mippi-the-dork/Portal/issues) describing the Blueprint workflow problem you'd like to solve and the result you're after.

**Pull requests are welcome.** For significant changes, open an issue first to discuss the intended behavior and scope. Portal aims to stay easy to use, predictable, graph-local and compatible with native Blueprint data.

## License

Portal is distributed under the **MIT License**. See [LICENSE](LICENSE) for the full terms.

---

**Portal is created by Mippi the Dork.**

**[Explore the Documentation](https://mippi-the-dork.github.io/Portal/)** · **[Portal Source](https://github.com/mippi-the-dork/Portal)** · **[Report an Issue](https://github.com/mippi-the-dork/Portal/issues)**
