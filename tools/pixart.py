"""Tiny indexed-colour pixel canvas + PNG writer (no PIL needed).

Used by the tools/gen_*_art.py generators that author the source PNGs in
graphics/; grit then converts those PNGs to C at build time.
"""
import struct
import zlib


def rgb(hexstr):
    hexstr = hexstr.lstrip("#")
    return tuple(int(hexstr[i:i + 2], 16) for i in (0, 2, 4))


def gba_round(c):
    """Snap an RGB888 colour to what the GBA's BGR555 can show."""
    return tuple((v >> 3) << 3 | (v >> 5) for v in c)


class Canvas:
    def __init__(self, w, h, fill=0):
        self.w, self.h = w, h
        self.px = [[fill] * w for _ in range(h)]

    def get(self, x, y):
        if 0 <= x < self.w and 0 <= y < self.h:
            return self.px[y][x]
        return None

    def set(self, x, y, c):
        if c is not None and 0 <= x < self.w and 0 <= y < self.h:
            self.px[y][x] = c

    def rect(self, x0, y0, x1, y1, c):
        """Fills [x0,x1) x [y0,y1)."""
        for y in range(max(0, y0), min(self.h, y1)):
            for x in range(max(0, x0), min(self.w, x1)):
                self.px[y][x] = c

    def hline(self, x0, x1, y, c):
        self.rect(x0, y, x1, y + 1, c)

    def vline(self, x, y0, y1, c):
        self.rect(x, y0, x + 1, y1, c)

    def ellipse(self, cx, cy, rx, ry, c):
        for y in range(int(cy - ry) - 1, int(cy + ry) + 2):
            for x in range(int(cx - rx) - 1, int(cx + rx) + 2):
                dx = (x + 0.5 - cx) / rx
                dy = (y + 0.5 - cy) / ry
                if dx * dx + dy * dy <= 1.0:
                    self.set(x, y, c)

    def stamp(self, art, legend, ox=0, oy=0, flip=False):
        """Draws ASCII art; legend maps chars to palette indices, '.'
        (or any char missing from legend) is skipped."""
        for y, row in enumerate(art):
            if flip:
                row = row[::-1]
            for x, ch in enumerate(row):
                if ch in legend:
                    self.set(ox + x, oy + y, legend[ch])

    def blit(self, src, ox, oy, sx=0, sy=0, w=None, h=None, key=None):
        w = src.w if w is None else w
        h = src.h if h is None else h
        for y in range(h):
            for x in range(w):
                c = src.px[sy + y][sx + x]
                if key is None or c != key:
                    self.set(ox + x, oy + y, c)

    def crop(self, x, y, w, h):
        out = Canvas(w, h)
        out.blit(self, 0, 0, x, y, w, h)
        return out

    def flipped(self):
        out = Canvas(self.w, self.h)
        for y in range(self.h):
            out.px[y] = self.px[y][::-1]
        return out

    def outline(self, ink, skip=(0,), where=None):
        """Adds a 1px outline of `ink` around every non-`skip` pixel (4-way)
        onto pixels that are currently in `skip`."""
        src = [row[:] for row in self.px]
        for y in range(self.h):
            for x in range(self.w):
                if src[y][x] not in skip:
                    continue
                if where and not where(x, y):
                    continue
                for dx, dy in ((1, 0), (-1, 0), (0, 1), (0, -1)):
                    nx, ny = x + dx, y + dy
                    if 0 <= nx < self.w and 0 <= ny < self.h and \
                            src[ny][nx] not in skip and src[ny][nx] != ink:
                        self.px[y][x] = ink
                        break


def write_png(path, canvas, palette):
    """Writes an 8-bit indexed PNG with `palette` (list of RGB tuples)."""
    def chunk(tag, data):
        c = struct.pack(">I", len(data)) + tag + data
        return c + struct.pack(">I", zlib.crc32(tag + data) & 0xFFFFFFFF)

    raw = b"".join(b"\x00" + bytes(row) for row in canvas.px)
    plte = b"".join(bytes(gba_round(c)) for c in palette)
    png = b"\x89PNG\r\n\x1a\n"
    png += chunk(b"IHDR", struct.pack(">IIBBBBB", canvas.w, canvas.h, 8, 3, 0, 0, 0))
    png += chunk(b"PLTE", plte)
    png += chunk(b"IDAT", zlib.compress(raw, 9))
    png += chunk(b"IEND", b"")
    with open(path, "wb") as f:
        f.write(png)
