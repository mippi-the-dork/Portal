# Maintaining Portal Documentation

This site reuses the established Mippi documentation design language but documents the **Portal** repository’s own behavior only. The current baseline is v0.12.0 release candidate.

## Design versus Technical

Every topic has **DESIGN.md** for user steps, expected outcomes and limitations and **TECHNICAL.md** for implementation, C++ ownership, metadata, native state, and constraints. Do not paste the same README paragraph into both. Document shipped capability only after source verification and preferably a tested editor workflow.

## Media replacement process

The [Media Inventory](MEDIA_INVENTORY.md) describes each static image and GIF slot with a natural public caption followed by **Capture instructions**. Until real media is captured, the neutral placeholder appears. To replace it, supply a genuine Unreal screenshot/recording at the intended asset path and update the media reference while preserving appropriate accessibility alt text and the public caption. The capture instruction is intentionally visible for the production phase; remove it as a separate editorial decision when images are final.

## Publishing and QA

GitHub Pages should publish from **main → /docs**, subject to the repository’s Pages configuration. Before merging, check five index tables, all 36 paired pages, internal Markdown links, search, media URLs, desktop/mobile layout and [Verification Matrix](VERIFICATION_MATRIX.md). Do not claim browser/UE testing from a source-only check.

## Version history

The historical 0.4.1 native-safe transition is documented in the repository architecture note. No later version history or future feature commitments should be invented from inference. Update docs, roadmap and release information when Portal actually changes.
