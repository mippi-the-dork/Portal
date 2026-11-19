# Portal v0.12.0 Verification Matrix

**Scope:** UE 5.8.0–5.8.3, Win64, Portal v0.12.0. **Status:** cases below are proposed; no passing Editor test results are recorded in this documentation.

| ID | Scenario | Expected outcome | Status |
| --- | --- | --- | --- |
| POR-01 | Enable Portal, create Input from empty Blueprint graph | Correct native-backed Input created | Not run |
| POR-02 | Right-click compatible data output pin → Create Portal | Input created and wired to that output | Not run |
| POR-03 | Right-click unconnected compatible data input → Connect Portal Output | Output created and connected to consumer | Not run |
| POR-04 | Shift+R+left-click with nothing selected | New Input at cursor in editable K2 graph | Not run |
| POR-05 | Shift+R+left-click with one family member selected | New Output for selected family | Not run |
| POR-06 | Input with no Outputs then 1/2 Outputs | Counts show 0,1,2 on mirrored nodes | Not run |
| POR-07 | Rename Input and inspect Outputs | Family names and tooltips synchronize | Not run |
| POR-08 | Double-click Output and Input | Jump to Input; select family Outputs | Not run |
| POR-09 | Select Portal Family context action | Input and every Output selected | Not run |
| POR-10 | Retarget Output to compatible alternate Input | Consumer remains connected and counts update | Not run |
| POR-11 | Try retarget incompatible pin and unrelated graph | Rejected, unavailable or safely unchanged | Not run |
| POR-12 | Delete Input with Outputs using Delete → Yes/No/Cancel | Whole-family deletion / orphan creation / no change respectively | Not run |
| POR-13 | Create Missing Input from orphan Output | Same-identity orphans repaired where possible | Not run |
| POR-14 | Convert Output/family to native reroutes | Portal metadata cleared and physical links remain | Not run |
| POR-15 | Save, close and reopen a Blueprint family | Native nodes, name, counts and links restored | Not run |
| POR-16 | Copy/paste entire family in same graph | No identity collisions; family stays independent | Not run |
| POR-17 | Copy one Output into other function/graph | Does not silently bind unrelated same-name Input | Not run |
| POR-18 | Create Portal on exec pin or convert wildcard to exec | Unsupported / safely converted back to native reroutes | Not run |
| POR-19 | Migrate historical custom Portal nodes | Equivalent native reroutes; expected asset dirtiness | Not run |
| POR-20 | Disable/uninstall plugin then reopen compiled Blueprint | Native reroute graph/wires remain; compile passes | Not run |
| POR-21 | Undo/Redo creation, retargeting, deletion and conversion | Expected graph and counts restored | Not run |
| POR-22 | Many loaded Blueprints and graph action refresh | No unacceptable recurring repair scan cost | Not run |
| DOC-01 | Open Pages site and navigate all five categories | All paired Design/Technical pages and links work | Not run |
| DOC-02 | Search titles and body-only terms | Useful results and excerpts, graceful slow fetch | Not run |
| DOC-03 | View at 320px, tablet and desktop widths | Images, captions, tables, nav and TOC do not overflow | Not run |
| DOC-04 | Keyboard search, arrows, Enter, Escape, back navigation | Accessible and predictable navigation | Not run |
| DOC-05 | Replace one media placeholder with final screenshot/GIF | Figure keeps caption, instructions and intended width | Not run |

## Evidence collection

Record exact Unreal version, Portal commit, Blueprint graph fixture, pin category, screenshot/GIF, Output Log or Automation command and observed result. Only promote a case to **Passed** after actually running it. Use native Unreal Automation tests where feasible; script review alone is not a pass.
