# Updating, Support, and Compatibility — Design

**Portal v0.12.0 · UE 5.8.x · Win64 · Blueprint Editor**

## Current documented release

<figure class="doc-media" data-kind="image" data-media-id="updating-folder">
  <img src="../../assets/media-placeholder.png" alt="Updating Portal replaces the complete editor plugin installation while preserving the project’s Blueprint assets." loading="lazy">
  <figcaption>Updating Portal replaces the complete editor plugin installation while preserving the project’s Blueprint assets.<span class="media-capture-instructions"><strong>Capture instructions · Image:</strong> Capture File Explorer showing the complete Portal folder under YourProject/Plugins, with Portal.uplugin visible. If using a release ZIP, show the folder structure that will replace the previous installation.</span></figcaption>
</figure>

**Portal 0.12.0 release candidate**, UE **5.8.0–5.8.3**, **Win64**, **UncookedOnly/editor-oriented**, **Blueprint data pins**. The plugin descriptor also marks the build beta. It does not claim stable API compatibility with earlier UE branches.

## Update a project plugin

1. Close Unreal Editor.
2. Commit/back up Blueprints and the previous plugin files, particularly if migrating from older versions.
3. Replace the complete `Plugins/Portal` folder with the desired compatible release, or update its source checkout and rebuild the Editor target.
4. Restart the editor and open a representative Blueprint family.
5. Compile, save/reopen, test an Output, and confirm native reroute fallback where applicable.

## Help and feedback

Open an issue on the [Portal repository](https://github.com/mippi-the-dork/Portal/issues) and supply a minimal reproduction, Unreal log messages, exact release/build, graph type, pin signature, and whether migration or copy/paste is involved.

## Source and license

The plugin's source is published at [mippi-the-dork/Portal](https://github.com/mippi-the-dork/Portal). Refer to the repository's current [LICENSE](https://github.com/mippi-the-dork/Portal/blob/main/LICENSE) for its terms.

## Roadmap status

Historical native-safe migration is grounded in the included 0.4.1 architecture note. Future Portal-specific features have not been committed by the plugin author in this conversation; the roadmap separates research ideas from promises.

For native implementation and engine boundaries, [read Technical](TECHNICAL.md).
