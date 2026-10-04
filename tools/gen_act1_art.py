#!/usr/bin/env python3
"""Generates the Act 1 (Bouillon training yard) art:

  graphics/spr_knight.png   Godfrey / Baldwin (palette variants), 32x32 frames
  graphics/spr_ida.png      Ida of Lorraine (robed body from the prelude art)
  graphics/spr_wicher.png   Wicher the master-at-arms (tunic body)
  graphics/spr_pell.png     straw practice pell, 16x32 frames
  graphics/a1_tiles.png     16x16 metatile sheet (8 per row) for the yard
  data/maps/map_act1_layout.c/.h   metatile layer + collision flags

Knight sheet (128x160): rows 0-2 = walk down / up / right (4 frames each,
as the prelude sheets); row 3 = attack down windup, down strike, up windup,
up strike; row 4 = right windup, right strike, hurt down, hurt right.
Left is the right-facing art h-flipped at runtime.

Rerun after edits:  python3 tools/gen_act1_art.py
"""
import os
import random
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from pixart import Canvas, rgb, write_png  # noqa: E402
import gen_prelude1_art as p1  # noqa: E402

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
GFX = os.path.join(ROOT, "graphics")

# ---------------------------------------------------------------- sprites --

# Same index layout as the prelude sheets. For the knight: R/r/l = tabard
# (heraldic field), b = tabard/shield charge (Bouillon's white fess),
# M/m/k = mail and helm steel, h = beard, W = blade glint / swing smear.
T, O, SKS, SK, HAIR, RD, RM, RL, MD, MM, ML, TRIM, WOOD, LEATH, WHITE, GOLD = range(16)
LEG = p1.LEG

KW = 32


def _mirror(half, dark):
    rows = []
    for row in half:
        assert len(row) == 16, (len(row), row)
        rows.append(row + "".join(dark.get(ch, ch) for ch in row[::-1]))
    return rows


DARK = {"k": "m", "m": "M", "l": "r", "r": "R", "s": "S"}

# Front: conical nasal helm, blond beard, tabard with the fess. Arms, legs,
# sword and shield are drawn per frame.
KNIGHT_DOWN = _mirror([
    "................",  # 0
    "...............o",
    "..............ok",
    ".............okm",
    ".............okm",
    "............okkm",  # 5
    "............okmm",
    "...........oMMMM",
    "..........oMSoSM",
    "..........oMSssM",
    "..........oMShhh",  # 10
    ".......okkkkMhhh",
    ".......okmmMlrhh",
    ".......okmMlrroh",
    ".......okmMlrrrr",
    ".......okmMlrrrr",  # 15
    ".......okmMbbbbb",
    ".......okmMbbbbb",
    ".......okmMlrrrr",
    ".......okmMlrrrr",
    ".......okmMfffff",  # 20
    "........okMlrrrr",
    "........okMlrrrr",
    "........okMlrrRo",
    "........okMlrRok",
    "........oMlrrRok",  # 25
    "........omkmkmkm",
], DARK)
KNIGHT_DOWN[20] = KNIGHT_DOWN[20][:15] + "gg" + KNIGHT_DOWN[20][17:]

KNIGHT_UP = _mirror([
    "................",  # 0
    "...............o",
    "..............ok",
    ".............okm",
    ".............okm",
    "............okkm",  # 5
    "............okmm",
    "...........oMMMM",
    "..........omMmMm",
    "..........oMmMmM",
    "..........omMmMm",  # 10
    ".......okkkkmmmm",
    ".......okmmMlrrr",
    ".......okmMlrrrr",
    ".......okmMlrrrr",
    ".......okmMlrrrr",  # 15
    ".......okmMbbbbb",
    ".......okmMbbbbb",
    ".......okmMlrrrr",
    ".......okmMlrrrr",
    ".......okmMfffff",  # 20
    "........okMlrrrr",
    "........okMlrrrr",
    "........okMlrrrr",
    "........okMlrrrr",
    "........oMlrrrrr",  # 25
    "........omkmkmkm",
], {"k": "m", "m": "M", "l": "r", "r": "R"})

# Facing right. Nasal and beard jut forward; the far (left) arm is hidden
# behind the shield, which is drawn per frame.
KNIGHT_SIDE = [r.ljust(KW, ".") for r in [
    "................",  # 0
    "...............oo",
    "..............okmo",
    ".............okkmmo",
    ".............okmmMo",
    "............okkmmMMo",  # 5
    "............okmmmMMo",
    "...........oMMMMMMMMo",
    "...........oMmMmMSoSMo",
    "...........oMmMmMSssso",
    "...........omMmMmShhho",  # 10
    ".........okkkkkkMMhhho",
    "........okmmmlrrrrohho",
    "........okmmlrrrrrRoo",
    "........okmmlrrrrrRo",
    "........okmmlrrrrrRo",  # 15
    "........okmmbbbbbbbo",
    "........okmmbbbbbbbo",
    "........okmmlrrrrrRo",
    "........okmmlrrrrrRo",
    "........okmmffffffgo",  # 20
    ".........okmlrrrrRo",
    ".........okmlrrrrRRo",
    ".........okmlrrrrRRo",
    "........okmllrrrrRRo",
    "........oklrrrrrrRRo",  # 25
    "........omkmkmkmkmMo",
]]

BOB = (0, 0, -1, 0)


def outline_pts(c, pts):
    for (x, y) in pts:
        for dx, dy in ((1, 0), (-1, 0), (0, 1), (0, -1)):
            nx, ny = x + dx, y + dy
            if (nx, ny) not in pts and c.get(nx, ny) == T:
                c.set(nx, ny, O)


def put(c, pts):
    """pts: dict (x,y)->colour; draws then outlines against transparency."""
    for (x, y), col in pts.items():
        c.set(x, y, col)
    outline_pts(c, pts)


def legs(c, d, f):
    """Hose + boots, rows 27-31, stride per frame."""
    if d in ("down", "up"):
        stride = {0: (0, 0), 1: (1, -1), 2: (0, 0), 3: (-1, 1)}[f]
        for x, s in ((12, stride[0]), (18, stride[1])):
            top, bot = 27, 30 + (1 if s > 0 else 0) - (1 if s < 0 else 0)
            pts = {}
            for y in range(top, bot):
                pts[(x, y)] = LEATH
                pts[(x + 1, y)] = LEATH
            for xx in range(x - (1 if x < 16 else 0), x + 2 + (1 if x > 16 else 0)):
                pts[(xx, bot)] = O if False else WOOD
            put(c, pts)
            c.hline(x - (1 if x < 16 else 0), x + 2 + (1 if x > 16 else 0), bot + 1, O)
    else:
        spread = {0: 0, 1: 3, 2: 0, 3: -3}[f]
        for i, (x0, s) in enumerate(((14, spread), (15, -spread))):
            pts = {}
            for y in range(27, 30):
                xx = x0 + (s * (y - 26)) // 3
                pts[(xx, y)] = LEATH
                pts[(xx + 1, y)] = LEATH if i else MD
            fx = x0 + s
            for xx in range(fx, fx + 4):
                pts[(xx, 30)] = WOOD
            put(c, pts)
            c.hline(fx, fx + 4, 31, O)


def kite_front(c, x0, y0, h=15, w=8, device=True):
    """Kite shield face: field R/r/l, white fess, gold boss."""
    pts = {}
    for j in range(h):
        if j < h - 6:
            half = w // 2
        else:
            half = max(0, (w // 2) * (h - j) // 7)
        cx = x0 + w // 2
        for x in range(cx - half, cx + half):
            col = RL if x < cx - half + 2 else RM if x < cx else RD
            if device and 3 <= j <= 4:
                col = TRIM if x < cx else ML
            if j == 0:
                col = GOLD
            pts[(x, y0 + j)] = col
    pts[(x0 + w // 2 - 1, y0 + 6)] = GOLD
    put(c, pts)


def kite_back(c, x0, y0, h=15, w=8):
    pts = {}
    for j in range(h):
        half = w // 2 if j < h - 6 else max(0, (w // 2) * (h - j) // 7)
        cx = x0 + w // 2
        for x in range(cx - half, cx + half):
            pts[(x, y0 + j)] = WOOD if (x + j) % 5 else LEATH
    for x in range(x0 + 1, x0 + w - 1):
        pts[(x, y0 + 4)] = LEATH
        pts[(x, y0 + 8)] = LEATH
    put(c, pts)


def kite_edge(c, x0, y0, h=16):
    """Shield seen from the side, held forward: a narrow red sliver."""
    pts = {}
    for j in range(h):
        w = 3 if j < h - 5 else 2 if j < h - 2 else 1
        for i in range(w):
            pts[(x0 + i, y0 + j)] = (RL, RM, RD)[i] if not 3 <= j <= 4 else TRIM
    put(c, pts)


def arm(c, sx, sy, hx, hy, sleeve=MM):
    """Mail sleeve from shoulder (sx,sy) to hand (hx,hy), 2px thick, plus
    a fist. Returns the pts drawn."""
    pts = {}
    n = max(abs(hx - sx), abs(hy - sy), 1)
    for i in range(n + 1):
        x = sx + (hx - sx) * i // n
        y = sy + (hy - sy) * i // n
        pts[(x, y)] = ML if i < n // 2 else sleeve
        pts[(x + 1, y)] = sleeve
    pts[(hx, hy)] = SK
    pts[(hx + 1, hy)] = SK
    pts[(hx, hy + 1)] = SKS
    pts[(hx + 1, hy + 1)] = SKS
    put(c, pts)


def sword(c, hx, hy, dx, dy, length=11):
    """Sword gripped at (hx,hy) pointing along (dx,dy) (unit-ish steps):
    gold cross-guard, steel blade with a lit edge, pommel behind the hand."""
    pts = {}
    # pommel
    pts[(hx - dx, hy - dy)] = GOLD
    # guard perpendicular to the blade
    gx, gy = hx + dx, hy + dy
    px_, py_ = -dy, dx
    for k in (-2, -1, 0, 1, 2):
        pts[(gx + px_ * k, gy + py_ * k)] = GOLD
    for i in range(2, length):
        x, y = hx + dx * i, hy + dy * i
        pts[(x, y)] = WHITE if i % 4 == 1 else ML
        if dx and dy:
            pts[(x + (1 if dx < 0 else 0), y + (1 if dy < 0 else 0) - (0 if dy < 0 else 0))] = MM
    put(c, pts)


def smear(c, pts_list):
    """White swing arc; only paints transparent pixels."""
    for (x, y) in pts_list:
        if c.get(x, y) == T:
            c.set(x, y, WHITE)


def arc(cx, cy, r, a0, a1, steps=40):
    import math
    out = []
    for i in range(steps + 1):
        a = math.radians(a0 + (a1 - a0) * i / steps)
        out.append((round(cx + r * math.cos(a)), round(cy + r * math.sin(a))))
    return out


def body(d, b=0):
    art = {"down": KNIGHT_DOWN, "up": KNIGHT_UP, "side": KNIGHT_SIDE}[d]
    assert all(len(r) == KW for r in art), d
    up = Canvas(KW, 32)
    up.stamp(art, LEG, 0, b)
    if b:
        up.stamp(art[-1:], LEG, 0, 26)  # fill the row the bob lifts from
    return up


def walk_frame(d, f):
    c = Canvas(KW, 32)
    b = BOB[f]
    swing = (0, 1, 0, -1)[f]
    if d == "down":
        legs(c, d, f)
        c.blit(body(d, b), 0, 0, key=T)
        arm(c, 7, 13 + b, 5, 20 + b + swing)
        sword(c, 5, 21 + b + swing, 0, 1, 10)
        kite_front(c, 20, 13 + b, w=7)
    elif d == "up":
        legs(c, d, f)
        kite_back(c, 5, 13 + b)
        c.blit(body(d, b), 0, 0, key=T)
        arm(c, 23, 13 + b, 25, 20 + b - swing)
        sword(c, 25, 21 + b - swing, 0, 1, 10)
    else:
        sword(c, 12 - swing, 21 + b, -1, 1, 9)
        legs(c, d, f)
        kite_edge(c, 20, 12 + b)
        c.blit(body(d, b), 0, 0, key=T)
        arm(c, 12, 13 + b, 12 - swing, 20 + b)
    return c


def attack_frame(d, phase):
    """phase 0 = windup, 1 = strike."""
    c = Canvas(KW, 32)
    if d == "down":
        legs(c, d, 0)
        c.blit(body(d), 0, 0, key=T)
        kite_front(c, 20, 13, w=7)
        if phase == 0:
            arm(c, 7, 13, 4, 9)
            sword(c, 4, 8, 0, -1, 9)
        else:
            smear(c, arc(15, 18, 13, 160, 60))
            smear(c, arc(15, 18, 12, 150, 70))
            arm(c, 9, 13, 14, 19)
            sword(c, 14, 20, 0, 1, 11)
    elif d == "up":
        legs(c, d, 0)
        kite_back(c, 5, 13)
        if phase == 1:
            smear(c, arc(16, 10, 13, 200, 330))
            smear(c, arc(16, 10, 12, 210, 320))
            arm(c, 22, 13, 18, 6)
            sword(c, 17, 5, -1, -1, 7)
        c.blit(body(d), 0, 0, key=T)
        if phase == 0:
            arm(c, 23, 13, 26, 9)
            sword(c, 26, 8, 0, -1, 9)
    else:
        legs(c, d, 1)
        kite_edge(c, 19, 12)
        if phase == 0:
            c.blit(body(d), -1, 0, key=T)
            arm(c, 12, 14, 9, 10)
            sword(c, 8, 9, -1, -1, 8)
        else:
            smear(c, arc(18, 18, 13, -70, 40))
            smear(c, arc(18, 18, 12, -60, 30))
            c.blit(body(d), 1, 0, key=T)
            arm(c, 14, 14, 21, 17)
            sword(c, 22, 17, 1, 0, 10)
    return c


def hurt_frame(d):
    c = Canvas(KW, 32)
    if d == "down":
        legs(c, d, 0)
        c.blit(body(d, -1), 0, 0, key=T)
        kite_front(c, 21, 11, w=7)
        arm(c, 7, 12, 3, 16)
        sword(c, 3, 17, -1, 1, 8)
    else:
        legs(c, "side", 0)
        c.blit(body("side", -1), -2, 0, key=T)
        kite_edge(c, 19, 10)
        arm(c, 11, 13, 7, 17)
        sword(c, 7, 18, -1, 1, 7)
    return c


def knight_sheet():
    out = Canvas(KW * 4, 32 * 5)
    for row, d in enumerate(("down", "up", "side")):
        for f in range(4):
            out.blit(walk_frame(d, f), f * KW, row * 32)
    extra = [attack_frame("down", 0), attack_frame("down", 1),
             attack_frame("up", 0), attack_frame("up", 1),
             attack_frame("side", 0), attack_frame("side", 1),
             hurt_frame("down"), hurt_frame("side")]
    for i, fr in enumerate(extra):
        out.blit(fr, (i % 4) * KW, (3 + i // 4) * 32)
    return out


def knight_palette(field, charge, hair, beard_skin=("#9a6448", "#d49c78")):
    pal = p1.char_palette(robe=field, mantle=("#3a3c46", "#6a6e7c", "#a4a8b4"),
                          skin=beard_skin, hair=hair, trim=charge)
    pal[O] = rgb("#140e10")
    pal[WOOD] = rgb("#6a4a2c")
    pal[LEATH] = rgb("#3e2c22")
    pal[WHITE] = rgb("#f4f0e4")
    pal[GOLD] = rgb("#d8a838")
    return pal


def gen_knights():
    sheet = knight_sheet()
    pal = knight_palette(("#7a1c1c", "#a8282a", "#d04838"), "#ece4d4", "#d8b060")
    pal += knight_palette(("#1c2c6a", "#28409a", "#4064c4"), "#e0b840", "#4a3020",
                          beard_skin=("#8a5a40", "#c88c68"))
    write_png(os.path.join(GFX, "spr_knight.png"), sheet, pal)
    return sheet, pal


# --- Pell: straw-stuffed sack on a post with a crossbar, 16x32 frames:
# rest, leaning right, leaning left, battered (beaten flat, straw out).
# Palette: R/r/l = straw, M/m/k = sacking, b = rope, w/f = post.

def pell_frame(lean, battered=False):
    c = Canvas(16, 32)
    pts = {}
    for y in range(18, 31):
        pts[(7, y)] = WOOD
        pts[(8, y)] = LEATH
    put(c, pts)
    c.hline(5, 11, 31, O)
    c.set(6, 30, LEATH)
    c.set(9, 30, LEATH)

    def sx(y):  # shear: the top leans further than the base
        return (lean * (22 - y)) // 12 if y < 22 else 0

    top = Canvas(16, 32)
    tp = {}
    for y in range(8, 22):  # sack body
        w = 4 if y < 10 or y > 19 else 5
        for x in range(8 - w, 8 + w):
            col = ML if x < 6 else MM if x < 10 else MD
            if (x + y) % 7 == 0:
                col = MD
            tp[(x + sx(y), y)] = col
    for y in range(3, 8):  # straw head
        w = 3 if 4 <= y <= 6 else 2
        for x in range(8 - w, 8 + w):
            tp[(x + sx(y), y)] = RL if x < 7 else RM if x < 9 else RD
    for x in range(1, 15):  # crossbar arms
        tp[(x + sx(11), 11)] = WOOD
    for y in (9, 15, 19):
        for x in range(4, 12):
            tp[(x + sx(y), y)] = TRIM
    if battered:
        for (x, y) in ((3, 13), (12, 16), (2, 17), (13, 12), (5, 21), (11, 21)):
            tp[(x + sx(y), y)] = RL
        for (x, y) in list(tp):
            if y < 6:
                del tp[(x, y)]
        tp[(8 + sx(6), 6)] = RM
        tp[(6 + sx(6), 6)] = RD
    put(top, tp)
    c.blit(top, 0, 0, key=T)
    return c


def gen_pell():
    out = Canvas(64, 32)
    for i, (lean, bat) in enumerate(((0, False), (3, False), (-3, False), (2, True))):
        out.blit(pell_frame(lean, bat), i * 16, 0)
    pal = p1.char_palette(robe=("#8a6a2c", "#b8923c", "#e0c878"),
                          mantle=("#5e4e36", "#86744e", "#a89870"), trim="#4a3020")
    pal[WOOD] = rgb("#6a4a2c")
    pal[LEATH] = rgb("#46301e")
    write_png(os.path.join(GFX, "spr_pell.png"), out, pal)
    return out, pal


def gen_people():
    ida = p1.sheet(p1.ROBE_DOWN, p1.ROBE_UP, p1.ROBE_SIDE, p1.robe_lower, p1.SK)
    ida_pal = p1.char_palette(robe=("#2e2246", "#463468", "#685296"),
                              mantle=("#a49e92", "#d2ccbe", "#f2eee2"),
                              skin=("#b07c62", "#e8b896"), hair="#5a4a40",
                              trim="#d0a440")
    write_png(os.path.join(GFX, "spr_ida.png"), ida, ida_pal)
    wicher = p1.sheet(p1.TUNIC_DOWN, p1.TUNIC_UP, p1.TUNIC_SIDE, p1.tunic_lower, p1.SK)
    w_pal = p1.char_palette(robe=("#5a4428", "#7e643c", "#a48658"),
                            mantle=("#3e3226", "#5a4a38", "#7a6650"),
                            skin=("#8a5a3e", "#c48a66"), hair="#9a948a",
                            trim="#7a2a24")
    write_png(os.path.join(GFX, "spr_wicher.png"), wicher, w_pal)
    return (ida, ida_pal), (wicher, w_pal)


# ------------------------------------------------------------------ tiles --

(BK, DK, DI_DK, DI, DI_LT, SAND, GR_DK, GR, GR_LT,
 ST_DK, ST, ST_LT, ST_HI, WD_DK, WD, RED) = range(16)
TILE_PAL = [rgb(h) for h in (
    "#18120e", "#2c2218", "#5a4430", "#76593c", "#94754e", "#c4a676",
    "#34482a", "#546c30", "#7a9244", "#4a4850", "#6e6a6c", "#949088",
    "#bcb4a2", "#4a3020", "#7a5432", "#9a2c24")]
WHITE_T = ST_HI

MT = 16
SHEET_COLS = 8
SOLID, TRIGGER = 1, 8


def dirt(seed, base=DI, tufts=0, worn=False):
    def draw():
        rng = random.Random(seed)
        c = Canvas(MT, MT, base)
        for _ in range(28):
            x, y = rng.randrange(MT), rng.randrange(MT)
            c.set(x, y, rng.choice((DI_DK, DI_LT, DI_DK if worn else DI)))
        for _ in range(3):  # pebbles
            x, y = rng.randrange(1, MT - 1), rng.randrange(1, MT - 1)
            c.set(x, y, ST_LT)
            c.set(x, y + 1, DI_DK)
        for _ in range(tufts):
            x, y = rng.randrange(1, MT - 2), rng.randrange(2, MT)
            for dx, dy, col in ((0, 0, GR), (1, -1, GR_LT), (-1, -1, GR),
                                (0, -2, GR_LT), (1, 0, GR_DK), (0, 1, GR_DK)):
                c.set(x + dx, y + dy, col)
        return c
    return draw


def sand(seed, edge=None):
    """Sparring ring floor; `edge` = side(s) that fray into the dirt."""
    def draw():
        rng = random.Random(seed)
        c = Canvas(MT, MT, SAND)
        for _ in range(22):
            c.set(rng.randrange(MT), rng.randrange(MT), rng.choice((DI_LT, DI_LT, ST_HI)))
        for x in range(MT):
            for y in range(MT):
                d = {"n": y, "s": MT - 1 - y, "w": x, "e": MT - 1 - x}
                for e in (edge or ""):
                    if d[e] < 3 and rng.random() < (0.9 if d[e] == 0 else 0.55 if d[e] == 1 else 0.2):
                        c.set(x, y, rng.choice((DI, DI_LT)))
        return c
    return draw


def post(seed):
    """Rope post at a ring corner, on sand."""
    def draw():
        c = sand(seed)()
        c.ellipse(8, 13, 5, 2.5, DI_LT)
        c.rect(6, 2, 10, 14, WD)
        c.vline(6, 2, 14, DK)
        c.vline(9, 2, 14, WD_DK)
        c.rect(6, 1, 10, 3, DI_LT)
        c.hline(6, 10, 1, DK)
        c.hline(6, 10, 14, DK)
        c.hline(6, 10, 5, RED)
        c.hline(6, 10, 6, WD_DK)
        return c
    return draw


def rope_h(seed):
    def draw():
        c = sand(seed)()
        for x in range(MT):
            y = 5 + (1 if 4 <= x <= 11 else 0)
            c.set(x, y, RED)
            c.set(x, y + 1, WD_DK)
        return c
    return draw


def rope_v(seed):
    def draw():
        c = sand(seed)()
        for y in range(MT):
            c.set(8, y, RED if y % 3 else WD_DK)
        return c
    return draw


def ashlar(c, x0, y0, w, h, rng, course=5):
    c.rect(x0, y0, x0 + w, y0 + h, ST)
    for j, yy in enumerate(range(y0, y0 + h, course)):
        c.hline(x0, x0 + w, yy, ST_DK)
        off = (j * 5 + rng.randrange(3)) % 8
        for xx in range(x0 + off, x0 + w, 8):
            c.vline(xx, yy, min(yy + course, y0 + h), ST_DK)
        for xx in range(x0, x0 + w):
            if rng.random() < 0.18:
                c.set(xx, min(yy + 1 + rng.randrange(course - 1), y0 + h - 1),
                      rng.choice((ST_LT, ST_DK)))
        c.hline(x0, x0 + w, yy + 1, ST_LT)


def t_crenel(seed):
    def draw():
        rng = random.Random(seed)
        c = Canvas(MT, MT, ST)
        ashlar(c, 0, 0, MT, MT, rng)
        c.rect(0, 0, MT, 6, BK)  # sky gap above the merlons
        c.rect(1, 0, 7, 6, ST_LT)
        c.rect(9, 0, 15, 6, ST_LT)
        c.vline(1, 0, 6, ST_HI)
        c.vline(9, 0, 6, ST_HI)
        c.vline(6, 0, 6, ST_DK)
        c.vline(14, 0, 6, ST_DK)
        c.hline(0, MT, 6, DK)
        c.hline(0, MT, 7, ST_HI)
        return c
    return draw


def t_wall(seed, slit=False):
    def draw():
        rng = random.Random(seed)
        c = Canvas(MT, MT)
        ashlar(c, 0, 0, MT, MT, rng)
        if slit:
            c.rect(7, 3, 9, 13, BK)
            c.vline(6, 3, 13, ST_DK)
            c.vline(9, 3, 13, ST_HI)
            c.hline(6, 10, 13, ST_HI)
        return c
    return draw


def t_wall_base(seed):
    def draw():
        rng = random.Random(seed)
        c = Canvas(MT, MT)
        ashlar(c, 0, 0, MT, 11, rng)
        c.rect(0, 11, MT, 15, ST_DK)
        c.hline(0, MT, 11, ST_LT)
        c.hline(0, MT, 15, DK)
        for x in range(0, MT, 5):
            c.vline(x, 12, 15, DK)
        return c
    return draw


def t_wall_shadow(seed):
    def draw():
        c = dirt(seed, tufts=1)()
        for y in range(5):
            for x in range(MT):
                if y < 3 or (x + y) % 2:
                    v = c.get(x, y)
                    c.set(x, y, {DI_LT: DI, DI: DI_DK, DI_DK: DK, GR_LT: GR,
                                 GR: GR_DK, ST_LT: DI}.get(v, v))
        return c
    return draw


def t_walltop(seed, side):
    """Curtain wall seen from above: walkway flags + parapet on the outside."""
    def draw():
        rng = random.Random(seed)
        c = Canvas(MT, MT, ST_LT)
        for y in range(0, MT, 4):
            c.hline(0, MT, y, ST)
            off = rng.randrange(6)
            for x in range(off, MT, 6):
                c.vline(x, y, y + 4, ST)
        if side == "w":
            c.rect(0, 0, 5, MT, ST_HI)
            c.vline(5, 0, MT, ST_DK)
            c.vline(MT - 1, 0, MT, DK)
        elif side == "e":
            c.rect(11, 0, MT, MT, ST_HI)
            c.vline(10, 0, MT, ST_DK)
            c.vline(0, 0, MT, ST_DK)
        else:  # south
            c.rect(0, 11, MT, MT, ST_HI)
            c.hline(0, MT, 10, ST_DK)
            c.hline(0, MT, 0, ST_DK)
            for x in (2, 10):
                c.rect(x, 12, x + 4, MT, ST_LT)
        return c
    return draw


def t_hay(seed):
    """Straw bale: rounded golden block, twine bands, lit top."""
    def draw():
        rng = random.Random(seed)
        c = dirt(seed)()
        c.ellipse(8, 14, 7, 2, DI_DK)
        c.rect(1, 5, 15, 14, SAND)
        c.rect(1, 2, 15, 5, ST_HI)
        for y in range(2, 14):
            for x in range(1, 15):
                if rng.random() < 0.3:
                    c.set(x, y, DI_LT if y >= 5 else SAND)
        for x in (4, 11):
            c.vline(x, 2, 14, WD_DK)
        c.hline(1, 15, 5, DI_LT)
        c.vline(1, 2, 14, DK)
        c.vline(14, 2, 14, DK)
        c.vline(13, 5, 14, DI)
        c.hline(2, 14, 1, DK)
        c.hline(2, 14, 14, DK)
        return c
    return draw


def t_barrel(seed):
    def draw():
        c = dirt(seed)()
        c.ellipse(8, 13, 7, 2.5, DI_DK)
        c.rect(2, 3, 14, 14, WD)
        c.vline(2, 3, 14, DK)
        c.vline(13, 3, 14, DK)
        c.vline(3, 3, 14, WD_DK)
        c.vline(11, 4, 13, WD_DK)
        c.hline(2, 14, 5, ST_DK)
        c.hline(2, 14, 11, ST_DK)
        c.ellipse(8, 3, 6, 2, WD_DK)
        c.ellipse(8, 3, 5, 1.4, WD)
        c.hline(2, 14, 14, DK)
        return c
    return draw


def s_trough():
    c = Canvas(32, 16)
    c.blit(dirt(71)(), 0, 0)
    c.blit(dirt(72)(), 16, 0)
    c.rect(2, 4, 30, 14, WD)
    c.rect(4, 6, 28, 12, rgb_idx_water())
    c.hline(5, 27, 7, ST_HI)
    c.hline(2, 30, 4, WD_DK)
    c.hline(2, 30, 13, WD_DK)
    c.vline(2, 4, 14, DK)
    c.vline(29, 4, 14, DK)
    c.hline(2, 30, 3, DK)
    c.hline(2, 30, 14, DK)
    return c


def rgb_idx_water():
    return ST_DK


def s_rack():
    """Weapon rack against the wall: spears, a red/white shield."""
    c = Canvas(32, 16)
    c.blit(t_wall_shadow(81)(), 0, 0)
    c.blit(t_wall_shadow(82)(), 16, 0)
    c.rect(1, 9, 31, 11, WD)
    c.hline(1, 31, 11, DK)
    for x in (3, 8, 13, 18):
        c.vline(x, 0, 14, WD)
        c.set(x, 0, ST_HI)
        c.vline(x + 1, 1, 14, WD_DK)
    c.vline(2, 9, 15, WD_DK)
    c.vline(29, 9, 15, WD_DK)
    # shield leaning on the rack
    for j in range(11):
        half = 4 if j < 6 else max(0, 4 * (11 - j) // 5)
        for x in range(25 - half, 25 + half):
            c.set(x, 2 + j, ST_HI if 3 <= j <= 4 else RED)
    c.set(24, 5, WD)
    return c


def s_door():
    """Chapel door in the keep: round arch, oak leaves, iron straps."""
    rng = random.Random(91)
    c = Canvas(32, 32)
    ashlar(c, 0, 0, 32, 22, rng)
    t = t_wall_base(92)()
    c.blit(t, 0, 16)
    c.blit(t, 16, 16)
    c.ellipse(16, 10, 10, 9, ST_HI)
    c.rect(6, 10, 26, 31, ST_HI)
    c.ellipse(16, 11, 8, 8, BK)
    c.rect(8, 11, 24, 31, BK)
    c.ellipse(16, 12, 7, 7, WD_DK)
    c.rect(9, 12, 23, 31, WD_DK)
    for x in range(10, 23):
        if x != 16:
            c.vline(x, 8 if 12 <= x <= 20 else 11, 31, WD if x % 3 else WD_DK)
    c.vline(16, 6, 31, DK)
    for y in (15, 23):
        c.hline(9, 23, y, ST_DK)
    c.set(14, 19, ST_HI)
    c.set(18, 19, ST_HI)
    # carved cross in the keystone
    c.vline(16, 0, 4, RED)
    c.hline(15, 18, 1, RED)
    c.hline(6, 27, 31, DK)
    return c


def s_banner():
    c = Canvas(16, 32)
    rng = random.Random(95)
    ashlar(c, 0, 0, 16, 22, rng)
    t = t_wall_base(96)()
    c.blit(t, 0, 16)
    c.hline(1, 15, 1, WD_DK)
    c.set(0, 1, DK)
    c.set(15, 1, DK)
    c.rect(3, 2, 13, 22, RED)
    c.vline(3, 2, 22, DK)
    c.vline(12, 2, 22, DK)
    c.vline(11, 2, 22, WD_DK)
    c.rect(4, 9, 12, 13, ST_HI)
    c.hline(4, 12, 13, ST_LT)
    for x in range(3, 13, 2):  # swallow-tailed fringe
        c.set(x, 22, RED)
        c.set(x, 23, DK)
    c.hline(3, 13, 22, DK)
    c.set(4, 22, RED)
    c.set(8, 23, RED)
    c.set(8, 24, DK)
    return c


def s_gate():
    """Gate passage through the south wall (open, dark, portcullis up)."""
    c = Canvas(32, 16)
    c.blit(dirt(97, worn=True)(), 0, 0)
    c.blit(dirt(98, worn=True)(), 16, 0)
    c.rect(0, 9, 32, 16, DK)
    c.rect(2, 10, 30, 16, BK)
    for x in range(3, 30, 3):
        c.set(x, 10, ST_DK)
    c.hline(0, 32, 9, ST_DK)
    return c


def s_gatepost(side):
    def draw():
        c = t_walltop(99, "s")()
        c.rect(0, 0, MT, MT, ST)
        ashlar(c, 0, 0, MT, MT, random.Random(100))
        if side == "w":
            c.vline(MT - 1, 0, MT, DK)
            c.vline(MT - 2, 0, MT, ST_HI)
        else:
            c.vline(0, 0, MT, DK)
            c.vline(1, 0, MT, ST_DK)
        return c
    return draw


def t_pell_base(seed):
    def draw():
        c = dirt(seed, worn=True)()
        c.ellipse(8, 12, 7, 3, DI_DK)
        c.ellipse(8, 12, 4, 2, DK)
        for x, y in ((3, 9), (12, 14), (14, 10), (2, 14)):
            c.set(x, y, SAND)
        return c
    return draw


W, H = 20, 15
PELLS = ((4, 7), (7, 5), (5, 10))      # metatile cells (pell actors stand here)
RING = (10, 6, 16, 10)                 # sand ring x0, y0, x1, y1 (inclusive)


def build_layout():
    reg = {}

    def add(name, draw, flags=0):
        reg.setdefault(name, (draw, flags))
        return name

    dirts = [add(f"dirt{i}", dirt(10 + i)) for i in range(4)]
    tufty = [add(f"tuft{i}", dirt(20 + i, tufts=2)) for i in range(2)]
    worn = [add(f"worn{i}", dirt(30 + i, worn=True)) for i in range(2)]
    cren = [add(f"cren{i}", t_crenel(40 + i), SOLID) for i in range(2)]
    wall = [add(f"wall{i}", t_wall(50 + i), SOLID) for i in range(2)]
    slit = add("slit", t_wall(55, slit=True), SOLID)
    wbase = [add(f"wbase{i}", t_wall_base(60 + i), SOLID) for i in range(2)]
    wshad = [add(f"wshad{i}", t_wall_shadow(65 + i)) for i in range(2)]
    top_w = add("top_w", t_walltop(70, "w"), SOLID)
    top_e = add("top_e", t_walltop(71, "e"), SOLID)
    top_s = add("top_s", t_walltop(72, "s"), SOLID)
    hay = add("hay", t_hay(73), SOLID)
    barrel = add("barrel", t_barrel(74), SOLID)
    pbase = [add(f"pell{i}", t_pell_base(75 + i), SOLID) for i in range(3)]
    gp_w = add("gatepost_w", s_gatepost("w"), SOLID)
    gp_e = add("gatepost_e", s_gatepost("e"), SOLID)

    rng = random.Random(1096)
    g = [[rng.choice(dirts + dirts + worn) for _ in range(W)] for _ in range(H)]

    for x in range(W):
        g[0][x] = cren[x % 2]
        g[1][x] = slit if x in (8, 12, 16) else wall[x % 2]
        g[2][x] = wbase[x % 2]
        g[3][x] = wshad[x % 2]
        g[H - 1][x] = top_s
    for y in range(H):
        if y >= 1:
            g[y][0] = top_w
            g[y][W - 1] = top_e
    for y in range(4, H - 1):
        g[y][1] = tufty[y % 2] if y % 3 else g[y][1]
        g[y][W - 2] = tufty[(y + 1) % 2] if y % 4 else g[y][W - 2]
    x0, y0, x1, y1 = RING
    for y in range(y0, y1 + 1):
        for x in range(x0, x1 + 1):
            e = ("n" if y == y0 else "") + ("s" if y == y1 else "") + \
                ("w" if x == x0 else "") + ("e" if x == x1 else "")
            g[y][x] = add(f"sand_{e or 'c'}{(x + y) % 2}", sand(200 + len(e) * 7 + (x + y) % 2, e))
    for y in (y0, y1):
        for x in range(x0 + 1, x1):
            g[y][x] = add(f"rope_h{y == y1}", rope_h(220 + (y == y1)))
    for x in (x0, x1):
        for y in range(y0 + 1, y1):
            g[y][x] = add(f"rope_v{x == x1}", rope_v(230 + (x == x1)))
    for x, y in ((x0, y0), (x1, y0), (x0, y1), (x1, y1)):
        g[y][x] = add(f"post{x}{y}", post(240 + x + y), SOLID)
    for i, (x, y) in enumerate(PELLS):
        g[y][x] = pbase[i]
    for (x, y) in ((1, 11), (1, 12), (2, 12)):
        g[y][x] = hay
    for (x, y) in ((18, 11), (18, 12), (17, 12)):
        g[y][x] = barrel
    g[H - 1][8] = gp_w
    g[H - 1][11] = gp_e

    def struct(name, draw, ox, oy, w, h, flags):
        canvas = draw()
        for j in range(h):
            for i in range(w):
                n = f"{name}@{i},{j}"
                reg.setdefault(n, ((lambda cv=canvas, i=i, j=j:
                                    cv.crop(i * MT, j * MT, MT, MT)), flags))
                g[oy + j][ox + i] = n

    struct("door", s_door, 3, 1, 2, 2, SOLID)
    struct("bannerA", s_banner, 10, 1, 1, 2, SOLID)
    struct("bannerB", s_banner, 14, 1, 1, 2, SOLID)
    struct("rack", s_rack, 6, 3, 2, 1, SOLID)
    struct("trough", s_trough, 16, 3, 2, 1, SOLID)
    struct("gate", s_gate, 9, H - 1, 2, 1, SOLID)
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
    write_png(os.path.join(GFX, "a1_tiles.png"), sheet, TILE_PAL)

    out = ["/* Generated by tools/gen_act1_art.py -- do not edit. */",
           '#include "map_act1_layout.h"', "",
           "const u16 g_a1MetaLayer[A1_MAP_W * A1_MAP_H] = {"]
    for row in g:
        out.append("    " + ",".join(f"{cells[n]:2d}" for n in row) + ",")
    out.append("};\n")
    out.append(f"/* MTF_* flags per metatile id; ids index a1_tiles.png ({SHEET_COLS} per row). */")
    out.append("const u8 g_a1Collision[A1_META_COUNT] = {")
    for i, n in enumerate(names):
        out.append(f"    {reg[n][1]}, /* {i:2d} {n} */")
    out.append("};")
    with open(os.path.join(ROOT, "data/maps/map_act1_layout.c"), "w") as f:
        f.write("\n".join(out) + "\n")
    with open(os.path.join(ROOT, "data/maps/map_act1_layout.h"), "w") as f:
        f.write(f"""/* Generated by tools/gen_act1_art.py -- do not edit. */
#ifndef CRUSADE_MAP_ACT1_LAYOUT_H
#define CRUSADE_MAP_ACT1_LAYOUT_H

#include <tonc_types.h>

#define A1_MAP_W {W}
#define A1_MAP_H {H}
#define A1_META_COUNT {len(names)}
#define A1_SHEET_COLS {SHEET_COLS}

extern const u16 g_a1MetaLayer[A1_MAP_W * A1_MAP_H];
extern const u8  g_a1Collision[A1_META_COUNT];

#endif
""")
    return full, len(names)


if __name__ == "__main__":
    sheet, pal = gen_knights()
    write_png("/tmp/knight_sheet.png", sheet, pal[:16])
    write_png("/tmp/knight_sheet_b.png", sheet, pal[16:])
    pell, ppal = gen_pell()
    write_png("/tmp/pell.png", pell, ppal)
    (ida, ip), (wi, wp) = gen_people()
    write_png("/tmp/ida.png", ida, ip)
    write_png("/tmp/wicher.png", wi, wp)
    full, n = gen_tiles()
    write_png("/tmp/a1_full.png", full, TILE_PAL)
    print(f"{n} metatiles")
