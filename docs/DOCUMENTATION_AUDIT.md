# Portal Documentation Audit

**Baseline:** Portal 0.12.0 release candidate. **Basis:** current public README, plugin descriptor, native-safe design note and source. **Not an executed UE or browser QA report.**

## What the manual covers

Five numbered categories; 18 user topics and 18 paired Technical references; creation, family editing, navigation, graph boundaries, native-safe fallback, orphan repair, migration, copy/paste, editor integration and troubleshooting.

## Remaining gaps and priorities

| Priority | Gap | Next action |
| --- | --- | --- |
| P0 | No executed UE 5.8.0–5.8.3 regression matrix supplied | Run the [Verification Matrix](VERIFICATION_MATRIX.md) and record exact builds/results |
| P0 | Uninstall fallback is implementation-based evidence, not a broad field guarantee | Test with a cloned project and representative compiled Blueprints |
| P0 | `SPortalBrowser` exists as a widget but has no confirmed module entry point | Do not advertise an openable Browser until its registration and actual behavior are verified |
| P1 | No official Portal hero screenshot supplied | Keep neutral placeholder and request actual Blueprint graph capture |
| P1 | Editor walkthroughs need real media | Replace typed image/GIF slots with genuine UE captures according to each capture instruction |
| P1 | Migration can alter/dirty older Blueprints | Verify legacy fixtures, source-control diffs and compiler equivalence |
| P1 | Large graph/loaded Blueprint performance not measured | Profile action refresh and 0.75-second migration/repair ticker |
| P1 | Mobile/browser UX not tested | Validate search, keyboard navigation, captions, diagrams and responsive images |
| P2 | Future roadmap not approved | Keep speculative research separate from shipped features |

## Maintaining accuracy

Design pages should explain how to operate the feature, what success looks like and why a path may be unavailable. Technical pages should identify native node classes, metadata, graph checks, transaction behavior, and limitations. Do not claim cross-graph or execution routing or promise a user-visible Portal Browser window before those behaviors are actually supported.

The repository README mentions `Doc/Prototype-Test-Plan.md`, but the current repository tree does **not** include that file. This site provides an explicit verification plan instead, without pretending the missing file was reviewed.
