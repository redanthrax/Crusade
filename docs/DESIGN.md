# Crusade — GBA Design Document

Working title **Crusade** (subtitle: *Godfrey of Bouillon*); the ending
title remains "Defender of Christ's Sepulchre".

A historical action RPG for the Game Boy Advance, built with devkitARM, libtonc,
grit, and Maxmod. C99, fixed-point only, static memory pools, no heap in the
hot path. This document is the canonical design reference for implementation;
keep it in sync with `include/` struct definitions as the engine evolves.

## 1. Premise

Godfrey of Bouillon, a noble of Charlemagne's line raised under his devout
mother's eye and trained to arms from boyhood, takes the cross after Urban
II's call at Clermont in 1095 and leads a contingent through the Balkans and
Anatolia, through the long starvation-siege of Antioch, to the walls of
Jerusalem; after the city falls in 1099 he refuses the crown offered him over
the place Christ was crucified, taking instead the title Defender of the Holy
Sepulchre, and dies defending the kingdom in 1100 — a game about arms,
command, and piety, not a secret destiny.

## 2. Prelude Beat Sheet

1. **Jerusalem, post-Constantine (4th c.)** — Map: Church of the Holy
   Sepulchre courtyard + colonnade. Tutorial: d-pad move, A interact, B run,
   keep a pilgrim procession bunched (herd mechanic: stragglers nudged back
   into a radius). Non-lethal scrap: shove/stun verb only, no sword.
2. **After 637 (walked time-jump)** — One scrolling corridor map, auto-walk
   with periodic player-confirmed steps (press A to advance). Vignettes at
   waypoints: a tolerant administrator waving pilgrims through a gate
   (text-only), then a brutal purge (forced stealth-walk past patrols), ending
   in a scripted massacre-escape (timed non-combat dash, instant-fail on
   capture, generous retry). Rulers differ — not a monolithic villain.
3. **1009** — Map: Chapel interior + collapsing exterior. The chapel guard
   (NPC-controlled briefly) always loses a scripted defense; the player's job
   is only to get two named survivors to the exit before the collapse timer.
   Hard cut to a text-bank cutscene: Urban II at Clermont, 1095. Final cut:
   Godfrey kneels, takes the cross. Player gains control at Act 1.

## 3. Act-by-Act Outline

### Act 1 — Bouillon
- Maps: training yard, chapel (mother scene), village (feud NPCs), muster
  field.
- Enemies: training dummies (scripted, no damage), rival retainer duel (1v1
  scripted-rules bout), feud raiders (sword + bow).
- Set-piece: the feud skirmish — defend the village gate, two waves,
  companion #1 (sergeant-at-arms) joins mid-fight.
- Side mission: recover stolen livestock before the muster deadline
  (exploration/timer, Piety reward).

### Act 2 — The March (Balkans/Anatolia)
- Maps: river ford, mountain pass ambush, supply camp, burned waystation.
- Enemies: Pecheneg skirmishers (ranged, hit-and-run), bandit raiders, a
  scripted Baldwin/Bohemond quarrel (dialogue-choice flavor, flag only).
- Set-piece: night ambush on the supply train — defend wagons across two
  screens, no rest between waves.
- Side mission: scout ahead for Baldwin (flags Act 4 branch).

### Act 3 — Antioch
- Maps: siege lines, city wall exterior, starvation camp, traitor's postern
  gate interior.
- Enemies: wall archers (fixed emplacements), Turkish garrison soldiers,
  elite guard mini-boss at the postern.
- Set-piece: traitor-inside infiltration (stealth-lite, avoid torches/cones),
  alarm triggers a fight to open the main gate, chains into the relief-army
  (Kerbogha) wave-defense battle.
- Side mission: share food with starving soldiers before the assault (Piety
  choice, no combat).

### Act 4 — Road to Jerusalem
- Maps: desert road (heat attrition — stamina drain unless resting at
  wells), plague camp, mountain shrine (relic found here).
- Enemies: raiding cavalry (fast, bash-counter required), scripted
  skirmishers near the shrine.
- Set-piece: a flagged companion (from Act 2 scout or Act 3 food choice)
  dies or deserts — branch chosen by prior flags, delivered as cutscene.
- Relic: Sepulchre fragment — long-cooldown defensive miracle (brief
  invulnerability + stagger pulse), gated by minimum Piety.
- Side mission: escort refugees to a safe well (Piety reward).

### Act 5 — Jerusalem, 1099
- Maps: outer walls (siege tower push), breach streets, inner city
  street-fighting, the Sepulchre interior (quiet, no combat).
- Enemies: wall defenders, elite city guard, final street-fight wave.
- Set-piece: wall assault — timed siege-tower approach under missile fire,
  then breach and close-quarters street fighting.
- After the fall: quiet Sepulchre scene (clean leitmotif). The crown offer
  is a **flag-gated ending trial**: Piety >= threshold plays the canonical
  refusal with the "Defender" title card; Arms-only low-Piety plays an
  alternate plainer text branch so the game still completes.
- Side mission: secure the Temple area (reduces an Act 6 casualty flag).

### Act 6 — Defender, Not King (Epilogue)
- Maps: one relief map (besieged outlying town), brief closing council
  scene (folded into the same map to save space).
- Enemies: Fatimid relief force skirmishers/soldiers, final mixed wave.
- Set-piece: the relief battle — last combat encounter, full companion
  assists.
- Ending: scripted death in 1100 (text + portrait, no combat), title card
  "Defender of Christ's Sepulchre," epilogue text scaled to final
  Arms/Command/Piety values.

## 4. Core Loop and the Three Tracks

**Loop:** explore a small metatile map -> fight encounters with
sword/shield-bash/thrown-spear/occasional mounted charge -> manage stamina ->
hit scripted dialogue/decision nodes that nudge Arms/Command/Piety -> clear an
Act set-piece -> cutscene -> next Act.

- **Arms (u8)** — raised by kills/clears and duel wins. Gates damage output,
  unlocks shield-bash and spear throw.
- **Command (u8)** — raised by set-piece objectives and companion-assist
  usage. Gates companion-assist cooldown reduction and mounted charge.
- **Piety (u8)** — raised by non-combat choices (procession-keeping,
  food-sharing, refugee escort, chapel scenes). Gates the canonical refusal
  ending and the relic miracle's availability/power.

Arms-only play still clears the game (lower set-piece difficulty, generic
ending text); Piety is additive narrative content, not a hard combat gate.

## 5. GBA Implementation Constraints

- **Entity pool:** fixed array of `Actor` (player, companions, enemies,
  projectiles) in EWRAM `.bss`, never heap-allocated. See `include/actor.h`.
- **Maps:** metatile maps (16x16 px cells) in ROM `const` data, loaded/
  referenced via `MapHeader`. Collision is a flat per-metatile-id flag byte
  table, not per-pixel. See `include/map.h`.
- **Combat:** sword, shield bash, thrown spear, mounted charge (open maps
  only), one relic miracle. Stamina meter (not mana). Hitstun via the
  `ActorState` state machine.
- **Progression:** three `u8` tracks — Arms, Command, Piety — in SRAM.
  Piety gates the canonical refusal ending; Arms-only still clears.
- **Companions:** at most two, assist moves only, no romance.
- **Save:** SRAM signature + chapter id + flags bitfield + three tracks.
  See `include/save.h`. SRAM is read/written one byte at a time (8-bit bus),
  never via DMA or struct-cast bulk copy.
- **Scenes:** script id table (text bank + portrait id + speaker id + next
  state). Text rendered in 8x8 font, box at the bottom of the screen. See
  `include/scene.h`.
- **Audio:** one Sepulchre leitmotif, a distorted version for 1009, the
  clean version restored in Act 5. Maxmod `.mod`/`.s3m` tracked modules only
  — no streaming audio.
- **Display:** Mode 0, four tile backgrounds, 4bpp tiles, 240x160. OAM
  sprites kept under ~96 active at once.

## 6. Data Layouts

See the canonical definitions in:
- `include/fixed.h` — `fx8` 24.8 fixed-point type and helpers.
- `include/actor.h` — `Actor` struct, `ActorState`, `ActorKind`.
- `include/map.h` — `MapHeader`, metatile collision flags.
- `include/save.h` — `SaveData`, SRAM signature/checksum.
- `include/scene.h` — `SceneScriptEntry`, script VM state.

## 7. File Tree

```
crusade/
├── Makefile
├── docs/
│   └── DESIGN.md
├── include/
│   ├── actor.h
│   ├── map.h
│   ├── save.h
│   ├── scene.h
│   ├── combat.h
│   ├── input.h
│   ├── camera.h
│   ├── oam_pool.h
│   └── fixed.h
├── src/
│   ├── main.c
│   ├── actor.c
│   ├── map.c
│   ├── collision.c
│   ├── combat.c
│   ├── save.c
│   ├── scene.c
│   ├── camera.c
│   ├── audio.c
│   ├── oam_pool.c
│   └── acts/
│       ├── prelude1.c
│       ├── prelude2.c
│       ├── prelude3.c
│       ├── act1_bouillon.c
│       ├── act2_march.c
│       ├── act3_antioch.c
│       ├── act4_road.c
│       ├── act5_jerusalem.c
│       └── act6_epilogue.c
├── data/
│   ├── maps/
│   ├── text/
│   └── enemies/
├── graphics/
│   ├── src/
│   ├── grit/
│   └── gen/
└── audio/
```

## 8. First-Playable Cut (current implementation target)

Scope: **Prelude Vignette 1** (Jerusalem procession tutorial) -> direct cut
to **Act 1 Bouillon training yard** only (prelude vignettes 2-3 and the Act 1
feud set-piece are out of scope for this slice; the cut stops after the
training-yard duel tutorial and the mother's blessing scene).

Systems required for this slice: movement + collision, the herd/keep-together
trigger (prelude), basic attack + hitstun state machine (training duel), the
scene script VM for dialogue, and a stub `SaveData` initialized at boot (no
title-screen load yet).

## 9. Descope Order (if ROM space is tight)

1. Act 6 epilogue map variety — collapse relief battle + council scene into
   one map.
2. All optional side missions (livestock, scouting, food-sharing, refugee
   escort) — Piety-flavor, not required for an Arms-only clear.
3. Prelude Vignette 2 — compress to a single text-bank cutscene with 2-3
   still portraits instead of a playable corridor map.
4. Companion #2 — ship with one companion only.
5. Alternate low-Piety ending text branch — ship only the canonical refusal
   ending.
6. Mounted charge mechanic — the most hardware/animation-expensive combat
   verb for its narrative payoff; cut first if Act 2/Act 6 set-pieces can be
   redesigned on-foot.

## 10. Explicitly Out of Scope

No romance subsystem. No floating-point physics (fixed-point only). No
per-pixel collision (metatile flags only). No streaming audio (Maxmod
tracked modules only). No more than ~96 simultaneous active OAM sprites. No
dynamic heap allocation for actors or maps at runtime.

## 11. BG VRAM Memory Map (Mode 0)

Fixed layout shared by every map/scene so BG0 (world) and BG3 (text/UI via
libtonc's TTE) never clobber each other:

| Region                     | Charblock | Screenblock |
|-----------------------------|-----------|-------------|
| BG0 world tile gfx           | 0         | -           |
| BG0 world tilemap             | -         | 16 (-19 for 64x64) |
| BG3 font glyph cache (TTE), tiles 0-95 | 1 | -        |
| Drop-shadow font copy, tiles 96-191    | 1 | -        |
| Text-box frame tiles, tiles 192+       | 1 | -        |
| BG2 text-box panel tilemap       | -         | 29          |
| BG3 text tilemap                 | -         | 28          |

BG palette bank 0 holds the world tileset; TTE's default font uses bank 15
(`0xF000` se0 base), so the two never share palette entries either. Banks
12-14 are menu text colours (disabled / normal / selected). Text-box banks:
7 name-tab ink, 8 narration panel, 9 dialogue panel, 10 narration ink,
11 dialogue ink (index 1 ink, index 2 drop shadow). OBJ
(sprite) VRAM is a separate address range and does not interact with any of
the above.

BG0 is priority 1 and sprites use priority 1, so the text layer (BG3,
priority 0) always draws on top.

**Text box** (`src/ui.c`): an opaque panel on BG2 (rows 14-19, name tab on
row 13) under BG3 text drawn with the drop-shadow font. While open, WIN0
(box) and WIN1 (tab) show only BG2+BG3, hiding the world and sprites behind
the text. Each scene entry's `speakerId` picks the style: `SPK_NARRATOR` is
a dark band with gold rules and centred parchment text; `SPK_HINT` is a navy
gold-framed box; named speakers add a gold name tab. Text word-wraps to 28
columns x 4 lines and pages on A, with a blinking advance arrow.

**Title/menu backdrop** (`src/title_bg.c`): the Crusade logo, subtitle,
stars and a Jerusalem skyline (Tower of David, Church of the Holy
Sepulchre) are drawn procedurally once into an EWRAM canvas, packed to
tiles and uploaded into the same BG0 slots as the world (CBB0/SBB16,
palette bank 0); `map_load()` overwrites them when gameplay starts. The
dawn sky is an HDMA gradient on the backdrop colour (DMA channel 0,
restarted every VBlank while a menu is open). The menu cursor is an 8x8
cross sprite (OBJ tile 16, palette bank 1).

**Gothic type** (`src/gothic.c`): the logo and subtitle use a
blackletter face rasterised from UnifrakturCook Bold (SIL OFL 1.1, source
and licence in `graphics/src/fonts/`) by `tools/gen_gothic.py` (needs
ImageMagick) into `graphics/gen/gothic_font.c/.h`: a 1bpp "Crusade" logo
bitmap plus a variable-width 14px glyph table for ASCII 32-126. The
backdrop draws them into its canvas. Menus and in-game dialogue stay on
the plain TTE system font for legibility.

## 12. Playtest Workflow

Each chapter/screen is built, then playtested on mGBA before moving to the
next. Launch with `make playtest` (runs `scripts/playtest.sh [scale]`): it
builds, forces mGBA's locked aspect ratio + integer scaling, and on
Hyprland floats the window at 2x GBA size (240x160 native, 3:2) so tiling
doesn't stretch the image. The developer drives input manually (default
mGBA keys: arrows = D-pad, X = A, Z = B, A = L, S = R, Enter = Start,
Backspace = Select); frames are captured with `grim` for visual checks.
Fix issues found before starting the next chapter's implementation.

While chapters are playtested one at a time, the Makefile's `TEST_CHAPTER`
(a `ChapterId`, default `CHAPTER_PRELUDE_1`) makes New Game jump straight to
that chapter and return to the title when it ends, e.g.
`make TEST_CHAPTER=CHAPTER_ACT1_BOUILLON`. Build the full chained flow with
`make TEST_CHAPTER=`.

`DEBUG=1` (the playtest default) adds a frame-pacing HUD in the top-right
during gameplay: `FPS` (frames presented last second), `DROP` (total missed
VBlanks) and `CPU` (peak share of the 228-line frame used by game logic).
Gameplay loops call `vsync_wait()` (`include/debug.h`), which is plain
`VBlankIntrWait()` in release builds (`make DEBUG=0 TEST_CHAPTER=`).
Changing `DEBUG` or `TEST_CHAPTER` rebuilds automatically (flags stamp in
`build/`). Scroll registers and OAM are only written right after VBlank
(`camera_commit()`, `oam_pool_flush()`) to avoid tearing; judder with a
steady FPS 60 / DROP 0 is host-side (e.g. a VM display at 120 Hz).

