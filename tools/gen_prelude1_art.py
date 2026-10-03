#!/usr/bin/env python3
"""Generates the prelude vignette 1 art (Holy Sepulchre courtyard, 4th c.):

  graphics/spr_guide.png    player: pilgrim guide, hooded cloak + staff
  graphics/spr_pilgrim.png  procession pilgrims (3 palette variants)
  graphics/spr_looter.png   looter in a short tunic
  graphics/p1_tiles.png     16x16 metatile sheet (8 per row) for the map
  data/maps/map_prelude1_layout.c   metatile layer + collision flags

Sprite sheets are 64x96: columns = walk frames 0-3, rows = facing
down / up / right (left is the right row h-flipped at runtime). Each frame
is 16x32 = 8 tiles; grit's -Mw2 -Mh4 keeps every frame's tiles contiguous.

Rerun after edits:  python3 tools/gen_prelude1_art.py
"""
import os
import random
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from pixart import Canvas, rgb, write_png  # noqa: E402

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
GFX = os.path.join(ROOT, "graphics")

# ---------------------------------------------------------------- sprites --

# Shared sprite palette index layout (every character sheet).
T, O, SKS, SK, HAIR, RD, RM, RL, MD, MM, ML, TRIM, WOOD, LEATH, WHITE, GOLD = range(16)

LEG = {
    "o": O, "S": SKS, "s": SK, "h": HAIR, "R": RD, "r": RM, "l": RL,
    "M": MD, "m": MM, "k": ML, "b": TRIM, "w": WOOD, "f": LEATH,
    "W": WHITE, "g": GOLD,
}


def mirror(half):
    return [row + row[::-1] for row in half]


# Robed + hooded figure (guide, pilgrims). Rows 3..26; hem/feet are drawn
# per frame below row 26.
ROBE_DOWN = mirror([
    "......oo",  # 3
    "....oomm",
    "...ommkk",
    "..omkkkk",
    "..omkMMM",
    "..omMSss",  # 8
    "..omMsos",
    "..omMsss",
    "..ommMSs",
    "..ommmMS",
    ".ommmmmM",  # 13
    ".omkmmMr",
    "omkmmMrr",
    "omkmmrrr",
    "omkmMrrr",
    "omkmMbbb",  # 18
    "omkmMrrr",
    "oSsmMrrl",
    "oSsoMrrl",
    ".oomMrrl",
    "..omRrrl",  # 23
    "..omRrrl",
    "..oRRrrl",
    "..oRRrrl",  # 26
])

ROBE_UP = mirror([
    "......oo",
    "....oomm",
    "...ommmm",
    "..ommmmm",
    "..omkmmm",
    "..omkmmm",
    "..omkmmm",
    "..omkmmm",
    "..omkmmm",
    "..omMkmm",
    ".ommMkmm",
    ".omkmMkm",
    "omkmmMmm",
    "omkmmMmm",
    "omkmmMmm",
    "omkmmMbb",
    "omkmmMrr",
    "oSsmMRrr",
    "oSsoMRrr",
    ".oomRRrr",
    "..oRRrrr",
    "..oRRrrr",
    "..oRRrrr",
    "..oRRrrr",
])

ROBE_SIDE = [
    ".....oooo.......",  # 3
    "....ommmmoo.....",
    "...ommkkkkmo....",
    "..ommkkkkkkmo...",
    "..ommkkkMMMMo...",
    "..ommkkMSssso...",  # 8
    "..ommkkMssosso..",
    "..ommkkMsssso...",
    "..ommkkMSsso....",
    "..ommmkMMSo.....",
    "...ommmmmmo.....",  # 13
    "..ommmmmmmmo....",
    "..ommmmmmmmro...",
    "..ommmmmmmrro...",
    "..ommmmmmrrro...",
    "..obbbbbbbbbo...",  # 18
    "..oRrrrrrrrro...",
    "..oRrrrrrrrlo...",
    "..oRrrrrrrrlo...",
    "..oRRrrrrrrlo...",
    "..oRRrrrrrrlo...",  # 23
    "..oRRrrrrrrlo...",
    "..oRRrrrrrrlo...",
    "..oRRrrrrrrrlo..",  # 26
]

# Looter: bare head, headband, beard, short tunic, bare arms and legs.
TUNIC_DOWN = mirror([
    "......oo",  # 3
    "....oohh",
    "...ohhhh",
    "..ohhhhh",
    "..obbbbb",
    "..ohSsss",  # 8
    "..ohssos",
    "..oSssss",
    "..ohSsss",
    "...ohhhh",
    "..oorrrr",  # 13
    ".orrrrrr",
    "osrrrrrl",
    "osSrrrrl",
    "osSrrrrl",
    "osSbbbbb",  # 18
    "oSsrrrrl",
    ".osRrrrl",
    "..oRrrrl",
    "..oRRrrr",
    "..oooooo",  # 23
])

TUNIC_UP = mirror([
    "......oo",
    "....oohh",
    "...ohhhh",
    "..ohhhhh",
    "..obbbbb",
    "..ohhhhh",
    "..ohhhhh",
    "..ohhhhh",
    "..oShhhh",
    "...oSSSS",
    "..oorrrr",
    ".orrrrrr",
    "osrrrrrr",
    "osSrrrrr",
    "osSrrrrr",
    "osSbbbbb",
    "oSsRrrrr",
    ".osRrrrr",
    "..oRRrrr",
    "..oRRrrr",
    "..oooooo",
])

TUNIC_SIDE = [
    ".....oooo.......",  # 3
    "....ohhhhoo.....",
    "...ohhhhhhho....",
    "..ohhhhhhhhho...",
    "..ohbbbbbbbbo...",
    "..ohhhhSsssso...",  # 8
    "..ohhhhSssosso..",
    "..ohhhSsssssso..",
    "..ohhhhhhSso....",
    "...ohhhhhho.....",
    "...oorrrro......",  # 13
    "..orrrrrrro.....",
    "..orrrrrrrro....",
    "..orrrrrrrro....",
    "..orrrrrrrlo....",
    "..obbbbbbbbo....",  # 18
    "..oRrrrrrrlo....",
    "..oRrrrrrrlo....",
    "..oRRrrrrrlo....",
    "..oRRrrrrrrlo...",
    "..ooooooooooo...",  # 23
]

# Walk cycle: 0 = stand, 1 = stride (left foot), 2 = stand + 1px bob,
# 3 = stride (right foot).
BOB = (0, 0, -1, 0)


def robe_lower(c, d, f, flip=False):
    """Hem (row 27) and feet (rows 28-31) for robed figures."""
    if d in ("down", "up"):
        sway = (0, -1, 0, 1)[f]
        c.hline(2 + max(sway, 0), 14 + min(sway, 0), 27, RD)
        c.set(2 + max(sway, 0), 27, O)
        c.set(13 + min(sway, 0), 27, O)
        c.hline(3, 13, 28, O)
        # feet: (x, length) per foot, front view toes point down
        feet = {0: [(4, 2), (10, 2)], 2: [(4, 2), (10, 2)],
                1: [(4, 3), (10, 1)], 3: [(4, 1), (10, 3)]}[f]
        for x, ln in feet:
            c.rect(x, 29, x + 2, 29 + ln, LEATH)
            c.hline(x, x + 2, 29 + ln, O)
            c.vline(x - 1, 29, 29 + ln, O)
            c.vline(x + 2, 29, 29 + ln, O)
    else:
        spread = (0, 3, 0, 3)[f]
        c.hline(3 - (spread > 0), 13 + (spread > 0), 27, RD)
        c.set(2 - (spread > 0), 27, O)
        c.set(13 + (spread > 0), 27, O)
        c.hline(3 - (spread > 0), 13 + (spread > 0), 28, O)
        if spread:
            front, back = (10, 3) if f == 1 else (9, 4)
            for x in (front + 1, back - 1):
                c.rect(x, 29, x + 3, 30, LEATH)
                c.hline(x, x + 3, 30, O)
                c.set(x - 1, 29, O)
                c.set(x + 3, 29, O)
        else:
            c.rect(6, 29, 10, 30, LEATH)
            c.hline(6, 10, 30, O)
            c.set(5, 29, O)
            c.set(10, 29, O)


def tunic_lower(c, d, f):
    """Bare legs + sandals (rows 24-31) under a short tunic."""
    if d in ("down", "up"):
        legs = {0: [(5, 0), (9, 0)], 2: [(5, 0), (9, 0)],
                1: [(5, 1), (9, -1)], 3: [(5, -1), (9, 1)]}[f]
        for x, dl in legs:
            bottom = 29 + dl
            c.rect(x, 24, x + 2, bottom, SK)
            c.vline(x + 1, 24, bottom, SKS)
            c.rect(x, bottom, x + 2, bottom + 1, LEATH)
            c.vline(x - 1, 24, bottom + 1, O)
            c.vline(x + 2, 24, bottom + 1, O)
            c.hline(x, x + 2, bottom + 1, O)
    else:
        pose = {0: [(6, 6), (8, 8)], 2: [(6, 6), (8, 8)],
                1: [(5, 3), (8, 11)], 3: [(7, 4), (8, 10)]}[f]
        for top, foot in pose:
            # straight-ish leg from hip (top) to foot x, 2px wide
            for y in range(24, 30):
                t = (y - 24) / 5.0
                x = int(round(top + (foot - top) * t))
                c.rect(x, y, x + 2, y + 1, SK)
                c.set(x - 1, y, O if c.get(x - 1, y) == T else c.get(x - 1, y))
                c.set(x + 2, y, O if c.get(x + 2, y) == T else c.get(x + 2, y))
            c.rect(foot, 30, foot + 3, 31, LEATH)
            c.hline(foot - 1, foot + 4, 31, O)


def side_arm(c, f, sleeve, cuff):
    """Near arm in side view, swinging with the stride."""
    dx = (0, 2, 0, -2)[f]
    pts = []
    for y in range(14, 21):
        t = (y - 14) / 6.0
        pts.append((int(round(6 + dx * t)), y))
    for x, y in pts:
        c.rect(x, y, x + 3, y + 1, sleeve)
    for x, y in pts:
        for ox in (x - 1, x + 3):
            if c.get(ox, y) not in (sleeve,):
                c.set(ox, y, O)
    hx, hy = pts[-1]
    c.rect(hx, hy + 1, hx + 3, hy + 3, cuff)
    c.hline(hx, hx + 3, hy + 3, O)
    c.set(hx - 1, hy + 1, O)
    c.set(hx - 1, hy + 2, O)
    c.set(hx + 3, hy + 1, O)
    c.set(hx + 3, hy + 2, O)
    return hx, hy + 1


def staff(c, x, y0, y1):
    c.vline(x, y0, y1, WOOD)
    c.set(x, y0 - 1, O)
    c.set(x, y1, O)
    for y in range(y0, y1):
        if c.get(x - 1, y) == T:
            c.set(x - 1, y, O)
        if c.get(x + 1, y) == T:
            c.set(x + 1, y, O)


def frame(body, d, f, lower, arm_cuff=None, with_staff=False):
    c = Canvas(16, 32)
    lower(c, d, f)
    upper = Canvas(16, 32)
    upper.stamp(body, LEG, 0, 3)
    if d == "side":
        sleeve = MM if body is ROBE_SIDE else RM
        hand = side_arm(upper, f, sleeve, arm_cuff)
    if BOB[f]:
        c.blit(upper, 0, 0, key=T)  # fills the row the bob lifts away from
    c.blit(upper, 0, BOB[f], key=T)
    if with_staff:
        if d == "down":
            staff(c, 1, 7 + BOB[f], 31)
        elif d == "up":
            staff(c, 14, 7 + BOB[f], 31)
        else:
            hx, hy = hand
            sx = hx + 3
            staff(c, sx, 6 + BOB[f], 31)
            # hand wraps the staff
            c.rect(sx - 1, hy + BOB[f], sx + 1, hy + 2 + BOB[f], SK)
    return c


def sheet(down, up, side, lower, cuff, with_staff=False):
    out = Canvas(64, 96)
    for row, (d, body) in enumerate((("down", down), ("up", up), ("side", side))):
        for f in range(4):
            out.blit(frame(body, d, f, lower, cuff, with_staff), f * 16, row * 32)
    return out


def char_palette(robe, mantle, skin=("#a8684a", "#e0a47c"),
                 hair="#3c2618", trim="#c8a050"):
    return [
        rgb("#ff00ff"), rgb("#22161a"), rgb(skin[0]), rgb(skin[1]),
        rgb(hair), rgb(robe[0]), rgb(robe[1]), rgb(robe[2]),
        rgb(mantle[0]), rgb(mantle[1]), rgb(mantle[2]), rgb(trim),
        rgb("#7a5230"), rgb("#5a3a26"), rgb("#f0e8d8"), rgb("#e8b840"),
    ]


def gen_sprites():
    guide = sheet(ROBE_DOWN, ROBE_UP, ROBE_SIDE, robe_lower, SK, with_staff=True)
    write_png(os.path.join(GFX, "spr_guide.png"), guide, char_palette(
        robe=("#a89878", "#d8ccb0", "#f0e8d4"),
        mantle=("#4a2e22", "#6e4632", "#946048"),
        trim="#b08840"))

    pilgrim = sheet(ROBE_DOWN, ROBE_UP, ROBE_SIDE, robe_lower, SK)
    pal = char_palette(robe=("#5a5048", "#807468", "#a49888"),
                       mantle=("#2a3c64", "#3e5a8c", "#6a86b4"))
    pal += char_palette(robe=("#6a5a3c", "#8c7a52", "#b0a070"),
                        mantle=("#5c2a2a", "#843c34", "#a85a48"),
                        skin=("#8c5a3c", "#c48c64"))
    pal += char_palette(robe=("#4c5440", "#6c765a", "#909a78"),
                        mantle=("#5a5a52", "#848070", "#aaa492"),
                        hair="#6a6058", skin=("#b07858", "#e8b490"))
    write_png(os.path.join(GFX, "spr_pilgrim.png"), pilgrim, pal)

    looter = sheet(TUNIC_DOWN, TUNIC_UP, TUNIC_SIDE, tunic_lower, SK)
    write_png(os.path.join(GFX, "spr_looter.png"), looter, char_palette(
        robe=("#3a3430", "#5a5048", "#7a6e60"),
        mantle=("#3a3430", "#5a5048", "#7a6e60"),
        skin=("#9a6444", "#d09670"), hair="#1e1410", trim="#9c2c24"))
    return guide, pilgrim, looter


# ------------------------------------------------------------------ tiles --

# BG palette bank 0 for the courtyard.
(BK, DK, ST_SH, ST_DK, ST_MD, ST_LT, ST_HI, RF_DK, RF, RF_LT,
 LF_DK, LF, LF_LT, TRUNK, AU, WATER) = range(16)

TILE_PAL = [rgb(h) for h in (
    "#1c1410", "#2c2018", "#6a5646", "#92806a", "#b8a484", "#d4c4a0",
    "#ece0c0", "#7a3424", "#a85434", "#cc7c4c", "#2c4028", "#4a6834",
    "#74884a", "#5c3e28", "#e0b048", "#4a7898")]

MT = 16
SHEET_COLS = 8
SOLID, TRIGGER = 1, 8  # MTF_SOLID, MTF_TRIGGER in include/map.h


def shade(c, s):
    """Darkens a stone index by s steps (shadowed paving)."""
    if s and ST_SH <= c <= ST_HI:
        return max(DK, c - s)
    return c


def flagstones(c, x0, y0, w, h, rng, s=0, course=8, light=(ST_MD, ST_MD, ST_LT)):
    """Broad, softly jointed paving slabs (joints darker than the slab,
    a single highlight pixel on each slab's top-left corner)."""
    for cy in range(y0, y0 + h, course):
        x = x0 - rng.randrange(0, 10)
        while x < x0 + w:
            sw = rng.choice((10, 12, 13, 16))
            base = rng.choice(light)
            for yy in range(cy, cy + course):
                for xx in range(max(x, x0), min(x + sw, x0 + w)):
                    col = base
                    if yy == cy + course - 1 or xx == x + sw - 1:
                        col = ST_DK
                    c.set(xx, yy, shade(col, s))
            if x0 <= x < x0 + w:
                c.set(x, cy, shade(min(base + 1, ST_HI), s))
            if rng.random() < 0.4:
                sx = rng.randrange(x + 1, x + sw - 2)
                sy = rng.randrange(cy + 1, cy + course - 2)
                if x0 <= sx < x0 + w:
                    c.set(sx, sy, shade(ST_DK if base == ST_MD else ST_MD, s))
            x += sw


def t_pave(seed, s=0):
    def draw():
        c = Canvas(MT, MT)
        rng = random.Random(seed)
        if seed % 2:
            flagstones(c, 0, 0, MT, MT, rng, s, course=16)
        else:
            flagstones(c, 0, 0, MT, 8, rng, s, course=8)
            flagstones(c, 0, 8, MT, 8, rng, s, course=8)
        return c
    return draw


def t_pave_shadow(seed):
    def draw():
        c = t_pave(seed)()
        for y in range(5):
            for x in range(MT):
                c.set(x, y, shade(c.get(x, y), 2 if y < 3 else 1))
        return c
    return draw


def t_path(edge):
    """Processional way: large dressed slabs, two per metatile row."""
    def draw():
        c = Canvas(MT, MT)
        rng = random.Random(77 + edge)
        for i, cy in enumerate((0, 8)):
            off = 4 if i else 12
            for sx in range(off - 16, MT, 16):
                base = ST_LT
                for yy in range(cy, cy + 8):
                    for xx in range(sx, sx + 16):
                        col = base
                        if yy == cy + 7 or xx == sx + 15:
                            col = ST_MD
                        c.set(xx, yy, col)
        for _ in range(2):
            c.set(rng.randrange(1, 15), rng.randrange(1, 7) + 8 * rng.randrange(2), ST_MD)
        if edge < 0:
            c.rect(0, 0, 2, MT, ST_MD)
            c.vline(0, 0, MT, ST_DK)
        elif edge > 0:
            c.rect(14, 0, 16, MT, ST_MD)
            c.vline(15, 0, MT, ST_DK)
        return c
    return draw


def t_roof():
    c = Canvas(MT, MT)
    for cy in range(0, MT, 4):
        off = 2 if (cy // 4) % 2 else 0
        c.rect(0, cy, MT, cy + 4, RF)
        c.hline(0, MT, cy, RF_LT)
        c.hline(0, MT, cy + 3, RF_DK)
        for x in range(off, MT, 4):
            c.vline(x, cy + 1, cy + 3, RF_DK)
    return c


def t_eave():
    """Roof running north-south with its eave on the east edge."""
    c = t_roof()
    c.rect(11, 0, 16, MT, BK)
    c.vline(10, 0, MT, RF_DK)
    c.vline(11, 0, MT, DK)
    flagstones(c, 12, 0, 4, MT, random.Random(5), s=2)
    c.vline(12, 0, MT, DK)
    return c


def t_shade(seed):
    return t_pave(seed, s=2)


def t_column(seed):
    def draw():
        c = t_shade(seed)()
        # cast shadow to the east
        for y in range(2, 16):
            for x in range(12, 15):
                c.set(x, y, shade(c.get(x, y), 1))
        c.rect(3, 0, 13, 3, ST_LT)       # capital
        c.hline(3, 13, 0, ST_HI)
        c.hline(3, 13, 2, ST_DK)
        c.rect(5, 3, 11, 13, ST_MD)      # shaft
        c.vline(5, 3, 13, ST_DK)
        c.vline(7, 3, 13, ST_LT)
        c.vline(8, 3, 13, ST_HI)
        c.vline(10, 3, 13, ST_SH)
        c.rect(4, 13, 12, 16, ST_LT)     # base
        c.hline(4, 12, 13, ST_HI)
        c.hline(4, 12, 15, ST_DK)
        for x, y0, y1 in ((2, 0, 3), (13, 0, 3), (4, 3, 13), (11, 3, 13),
                          (3, 13, 16), (12, 13, 16)):
            c.vline(x, y0, y1, DK)
        return c
    return draw


def ashlar(c, x0, y0, w, h, rng, row0=0):
    for i, cy in enumerate(range(y0, y0 + h, 4)):
        off = 4 if (row0 + i) % 2 else 0
        for bx in range(x0 - off, x0 + w, 8):
            base = rng.choice((ST_MD, ST_MD, ST_LT))
            for yy in range(cy, min(cy + 4, y0 + h)):
                for xx in range(max(bx, x0), min(bx + 8, x0 + w)):
                    col = base
                    if yy == cy + 3 or xx == bx + 7:
                        col = ST_SH
                    elif yy == cy:
                        col = min(base + 1, ST_HI)
                    c.set(xx, yy, col)


def t_wall(seed):
    def draw():
        c = Canvas(MT, MT)
        ashlar(c, 0, 0, MT, MT, random.Random(seed))
        return c
    return draw


def t_wall_base(seed):
    def draw():
        c = t_wall(seed)()
        c.rect(0, 13, MT, 16, ST_DK)
        c.hline(0, MT, 13, ST_MD)
        c.hline(0, MT, 15, DK)
        return c
    return draw


def t_window():
    c = t_wall(31)()
    c.rect(4, 3, 12, 15, ST_LT)          # frame
    c.rect(6, 5, 10, 14, DK)             # opening
    c.hline(7, 9, 4, DK)                 # arch
    c.rect(5, 4, 6, 6, ST_LT)
    c.set(6, 5, ST_HI)
    c.set(9, 5, ST_HI)
    c.hline(4, 12, 14, ST_HI)            # sill
    c.hline(4, 12, 15, ST_SH)
    c.set(7, 10, AU)                     # lamp glow inside
    return c


def t_cornice():
    c = Canvas(MT, MT)
    c.rect(0, 0, MT, 9, ST_LT)
    for x in range(0, MT, 4):
        c.vline(x, 1, 8, ST_MD)
    c.hline(0, MT, 0, ST_HI)
    c.hline(0, MT, 8, ST_HI)
    c.rect(0, 9, MT, 12, ST_MD)
    for x in range(1, MT, 3):
        c.set(x, 10, ST_HI)              # dentils
        c.set(x, 11, ST_SH)
    c.rect(0, 12, MT, 16, ST_SH)
    c.hline(0, MT, 12, DK)
    ashlar(c, 0, 14, MT, 2, random.Random(3), row0=1)
    return c


def t_cap():
    """South wall seen from above: coping stones."""
    c = Canvas(MT, MT)
    c.rect(0, 0, MT, 3, ST_SH)
    c.hline(0, MT, 0, DK)
    c.rect(0, 3, MT, 13, ST_LT)
    c.hline(0, MT, 3, ST_HI)
    for x in (0, 8):
        c.vline(x + 7, 3, 13, ST_DK)
    c.rect(0, 13, MT, 16, ST_MD)
    c.hline(0, MT, 15, ST_SH)
    return c


def t_steps():
    c = Canvas(MT, MT)
    for i, y0 in enumerate((0, 5, 10)):
        c.rect(0, y0, MT, y0 + 5, ST_LT)
        c.hline(0, MT, y0, ST_HI)
        c.hline(0, MT, y0 + 4, ST_DK)
        c.hline(0, MT, y0 + 3, ST_MD)
    c.rect(0, 15, MT, 16, ST_SH)
    return c


def t_basket():
    c = t_pave(41)()
    c.ellipse(8, 13, 7, 3, shade(ST_MD, 2))      # ground shadow
    c.rect(3, 4, 13, 14, TRUNK)
    c.ellipse(8, 4.5, 5, 2, DK)
    c.ellipse(8, 4.5, 4, 1.4, LF_DK)             # produce inside
    c.set(6, 4, LF_LT)
    c.set(9, 4, LF)
    for y in range(6, 14, 2):
        c.hline(3, 13, y, RF_DK)                  # weave
    for x in (5, 8, 11):
        c.vline(x, 5, 14, RF_DK)
    c.vline(2, 4, 14, DK)
    c.vline(13, 4, 14, DK)
    c.hline(3, 13, 14, DK)
    return c


# -- multi-metatile structures: draw on a big canvas, slice into metatiles --

def s_door():
    c = Canvas(32, 32)
    ashlar(c, 0, 0, 32, 32, random.Random(9))
    c.rect(0, 29, 32, 32, ST_DK)
    c.hline(0, 32, 29, ST_MD)
    # arch voussoirs + opening
    c.ellipse(16, 13, 13, 11, ST_LT)
    c.rect(3, 13, 29, 30, ST_LT)
    c.ellipse(16, 13, 10, 9, DK)
    c.rect(6, 13, 26, 30, DK)
    for a in range(-4, 5):
        x = 16 + a * 3
        c.set(x, 3 if abs(a) < 3 else 5, ST_SH)
    # bronze doors
    c.rect(8, 12, 24, 30, TRUNK)
    c.ellipse(16, 12, 8, 6, TRUNK)
    c.vline(15, 7, 30, DK)
    c.vline(16, 7, 30, DK)
    for y in range(11, 29, 5):
        for x in (10, 13, 19, 22):
            c.set(x, y, AU)
    c.set(14, 20, AU)
    c.set(17, 20, AU)
    c.vline(7, 12, 30, DK)
    c.vline(24, 12, 30, DK)
    return c


def s_dome():
    c = Canvas(128, 32)
    for y in range(32):
        for x in range(128):
            c.set(x, y, t_roof().get(x % MT, y % MT))
    c.ellipse(64, 46, 46, 36, DK)
    c.ellipse(64, 46, 45, 35, ST_MD)
    c.ellipse(58, 44, 36, 30, ST_LT)
    c.ellipse(52, 40, 20, 18, ST_HI)
    for rx in (-36, -24, -12, 0, 12, 24, 36):
        for y in range(10, 32):
            dy = (y - 46) / 35.0
            span = max(0.0, 1 - dy * dy) ** 0.5
            x = int(round(64 + rx * span))
            c.set(x, y, ST_DK if rx > 0 else ST_MD)
    c.rect(60, 9, 68, 12, ST_LT)           # lantern
    c.hline(60, 68, 9, ST_HI)
    c.vline(59, 9, 12, DK)
    c.vline(68, 9, 12, DK)
    c.vline(63, 1, 9, AU)                  # gilded cross
    c.vline(64, 1, 9, AU)
    c.hline(61, 67, 3, AU)
    c.outline(DK, skip=(RF, RF_LT, RF_DK), where=lambda x, y: y < 10)
    return c


def s_well():
    c = Canvas(32, 32)
    flagstones(c, 0, 0, 32, 32, random.Random(12))
    c.ellipse(17, 19, 14, 10, shade(ST_MD, 2))   # shadow
    c.ellipse(16, 16, 14, 11, DK)
    c.ellipse(16, 16, 13, 10, ST_LT)
    c.ellipse(16, 15, 13, 9, ST_HI)
    c.ellipse(16, 15, 9, 6, ST_DK)
    c.ellipse(16, 16, 8, 5, WATER)
    c.ellipse(14, 15, 3, 1.5, ST_LT)             # glint
    c.rect(3, 16, 30, 24, ST_MD)                  # drum face
    c.ellipse(16, 23, 13, 4, ST_MD)
    for x in range(5, 29, 5):
        c.vline(x, 17, 26, ST_DK)
    c.ellipse(16, 15, 13, 9, ST_HI)
    c.ellipse(16, 15, 9, 6, ST_DK)
    c.ellipse(16, 16, 8, 5, WATER)
    c.ellipse(14, 15, 3, 1.5, ST_LT)
    c.vline(2, 15, 24, DK)
    c.vline(29, 15, 24, DK)
    return c


def s_olive(seed):
    def draw():
        c = Canvas(32, 32)
        flagstones(c, 0, 0, 32, 32, random.Random(seed))
        c.ellipse(17, 27, 12, 4, shade(ST_MD, 2))
        c.rect(14, 18, 18, 28, TRUNK)               # gnarled trunk
        c.vline(14, 18, 28, DK)
        c.vline(18, 18, 28, DK)
        c.set(15, 22, ST_SH)
        c.hline(13, 20, 28, DK)
        rng = random.Random(seed)
        blobs = [(16, 12, 13, 10)] + [
            (rng.randrange(6, 26), rng.randrange(5, 18), rng.randrange(5, 8),
             rng.randrange(4, 6)) for _ in range(5)]
        for cx, cy, rx, ry in blobs:
            c.ellipse(cx, cy, rx, ry, LF_DK)
        for cx, cy, rx, ry in blobs:
            c.ellipse(cx - 1, cy - 1, rx - 2, ry - 2, LF)
        for _ in range(40):
            x, y = rng.randrange(4, 28), rng.randrange(2, 20)
            if c.get(x, y) == LF:
                c.set(x, y, LF_LT)
        c.outline(DK, skip=tuple(range(ST_SH, ST_HI + 1)),
                  where=lambda x, y: y < 22)
        return c
    return draw


def s_calvary():
    """The Rock of Golgotha with its gilded cross (Constantine's era)."""
    c = Canvas(32, 32)
    flagstones(c, 0, 0, 32, 32, random.Random(21))
    c.ellipse(17, 26, 14, 5, shade(ST_MD, 2))
    c.ellipse(16, 21, 14, 9, DK)
    c.ellipse(16, 21, 13, 8, ST_DK)
    c.ellipse(13, 19, 8, 5, ST_MD)
    c.ellipse(11, 18, 4, 2, ST_LT)
    c.set(20, 22, ST_SH)
    c.set(22, 20, ST_SH)
    c.rect(15, 1, 17, 17, AU)
    c.rect(11, 5, 21, 7, AU)
    c.vline(17, 2, 17, ST_SH)
    c.hline(12, 21, 7, ST_SH)
    c.outline(DK, skip=tuple(range(ST_SH, ST_HI + 1)) + (BK,),
              where=lambda x, y: y < 14)
    return c


# ---------------------------------------------------------------- layout --

W, H = 30, 20


def build_layout():
    """Returns a W x H grid of (metatile-name) and the name -> (draw, flags)
    registry. Names ending in @x,y are slices of a multi-cell structure."""
    reg = {}

    def add(name, draw, flags=0):
        reg.setdefault(name, (draw, flags))
        return name

    pave = [add(f"pave{i}", t_pave(100 + i)) for i in range(4)]
    shade_t = [add(f"shade{i}", t_shade(200 + i)) for i in range(2)]
    col = add("column", t_column(200), SOLID)
    roof = add("roof", t_roof, SOLID)
    eave_w = add("eave_w", t_eave, SOLID)
    eave_e = add("eave_e", lambda: t_eave().flipped(), SOLID)
    cor = add("cornice", t_cornice, SOLID)
    walls = [add(f"wall{i}", t_wall(300 + i), SOLID) for i in range(2)]
    wbase = [add(f"wbase{i}", t_wall_base(310 + i), SOLID) for i in range(2)]
    win = add("window", t_window, SOLID)
    pshadow = [add(f"pshadow{i}", t_pave_shadow(400 + i)) for i in range(2)]
    steps = add("steps", t_steps, TRIGGER)
    cap = add("cap", t_cap, SOLID)
    path_l = add("path_l", t_path(-1))
    path_m = add("path_m", t_path(0))
    path_r = add("path_r", t_path(1))
    basket = add("basket", t_basket, SOLID)

    rng = random.Random(1099)
    g = [[rng.choice(pave) for _ in range(W)] for _ in range(H)]

    for y in range(H):
        g[y][0] = roof
        g[y][1] = eave_w if y >= 2 else roof
        g[y][W - 1] = roof
        g[y][W - 2] = eave_e if y >= 2 else roof
    for x in range(2, W - 2):
        g[0][x] = roof
        g[1][x] = roof
        g[2][x] = cor
        g[3][x] = walls[x % 2]
        g[4][x] = wbase[x % 2]
        g[5][x] = pshadow[x % 2]
    for x in (5, 9, 20, 24):
        g[3][x] = win
    for y in range(5, H - 1):
        g[y][2] = shade_t[y % 2]
        g[y][W - 3] = shade_t[(y + 1) % 2]
        g[y][3] = col if y % 2 else pshadow[0] if y == 5 else g[y][3]
        g[y][W - 4] = col if y % 2 else pshadow[1] if y == 5 else g[y][W - 4]
    for y in range(6, H):
        g[y][13], g[y][14], g[y][15], g[y][16] = path_l, path_m, path_m, path_r
    for x in range(12, 18):
        g[5][x] = steps
    for x in range(2, W - 2):
        if not 13 <= x <= 16:
            g[H - 1][x] = cap
    g[11][24] = basket
    g[12][24] = basket
    g[12][23] = basket

    def struct(name, draw, ox, oy, w, h, flags):
        canvas = draw()
        for j in range(h):
            for i in range(w):
                n = f"{name}@{i},{j}"
                reg.setdefault(n, ((lambda cv=canvas, i=i, j=j:
                                    cv.crop(i * MT, j * MT, MT, MT)), flags))
                g[oy + j][ox + i] = n

    struct("dome", s_dome, 11, 0, 8, 2, SOLID)
    struct("door", s_door, 14, 3, 2, 2, SOLID)
    struct("well", s_well, 7, 9, 2, 2, SOLID)
    struct("calvary", s_calvary, 21, 7, 2, 2, SOLID)
    struct("oliveA", s_olive(5), 19, 10, 2, 2, SOLID)
    struct("oliveB", s_olive(6), 6, 14, 2, 2, SOLID)
    struct("oliveC", s_olive(7), 21, 15, 2, 2, SOLID)
    return g, reg


def gen_tiles():
    g, reg = build_layout()
    names, cells = [], {}
    for row in g:
        for n in row:
            if n not in cells:
                cells[n] = len(names)
                names.append(n)
    rows = (len(names) + SHEET_COLS - 1) // SHEET_COLS
    sheet = Canvas(SHEET_COLS * MT, rows * MT)
    full = Canvas(W * MT, H * MT)
    drawn = {n: reg[n][0]() for n in names}
    for i, n in enumerate(names):
        sheet.blit(drawn[n], (i % SHEET_COLS) * MT, (i // SHEET_COLS) * MT)
    for y in range(H):
        for x in range(W):
            full.blit(drawn[g[y][x]], x * MT, y * MT)
    write_png(os.path.join(GFX, "p1_tiles.png"), sheet, TILE_PAL)

    out = ["/* Generated by tools/gen_prelude1_art.py -- do not edit. */",
           '#include "map_prelude1_layout.h"', "",
           f"const u16 g_p1MetaLayer[P1_MAP_W * P1_MAP_H] = {{"]
    for row in g:
        out.append("    " + ",".join(f"{cells[n]:2d}" for n in row) + ",")
    out.append("};\n")
    out.append("/* MTF_* flags per metatile id; ids index p1_tiles.png "
               f"({SHEET_COLS} per row). */")
    out.append("const u8 g_p1Collision[P1_META_COUNT] = {")
    for i, n in enumerate(names):
        out.append(f"    {reg[n][1]}, /* {i:2d} {n} */")
    out.append("};")
    with open(os.path.join(ROOT, "data/maps/map_prelude1_layout.c"), "w") as f:
        f.write("\n".join(out) + "\n")
    with open(os.path.join(ROOT, "data/maps/map_prelude1_layout.h"), "w") as f:
        f.write(f"""/* Generated by tools/gen_prelude1_art.py -- do not edit. */
#ifndef CRUSADE_MAP_PRELUDE1_LAYOUT_H
#define CRUSADE_MAP_PRELUDE1_LAYOUT_H

#include <tonc_types.h>

#define P1_MAP_W {W}
#define P1_MAP_H {H}
#define P1_META_COUNT {len(names)}
#define P1_SHEET_COLS {SHEET_COLS}

extern const u16 g_p1MetaLayer[P1_MAP_W * P1_MAP_H];
extern const u8  g_p1Collision[P1_META_COUNT];

#endif
""")
    return full, len(names)


if __name__ == "__main__":
    gen_sprites()
    full, n = gen_tiles()
    write_png("/tmp/p1_full.png", full, TILE_PAL)
    print(f"{n} metatiles")
