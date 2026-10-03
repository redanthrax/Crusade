#!/usr/bin/env bash
# Launch the ROM in mGBA at native GBA aspect (240x160, integer scaled).
# On Hyprland the window is floated, sized to SCALE x GBA, and made opaque
# so tiling doesn't stretch the image. Usage: scripts/playtest.sh [scale]
set -euo pipefail
cd "$(dirname "$0")/.."

SCALE="${1:-2}"
ROM=crusade.gba
CFG="$HOME/.config/mgba/config.ini"

# mGBA rewrites config.ini on exit, so (re)assert the aspect settings.
mkdir -p "$(dirname "$CFG")"
touch "$CFG"
grep -q '^\[ports.qt\]' "$CFG" || printf '[ports.qt]\n' >> "$CFG"
for kv in lockAspectRatio=1 lockIntegerScaling=1 resampleVideo=0; do
    k="${kv%%=*}"
    if grep -q "^$k=" "$CFG"; then
        sed -i "s/^$k=.*/$kv/" "$CFG"
    else
        sed -i "/^\[ports.qt\]/a $kv" "$CFG"
    fi
done

[ -f "$ROM" ] || make
prev=""
if command -v hyprctl >/dev/null && [ -n "${HYPRLAND_INSTANCE_SIGNATURE:-}" ]; then
    prev=$(hyprctl activewindow -j | python3 -c "import json,sys; print(json.load(sys.stdin).get('address',''))")
fi
setsid mgba -1 "$ROM" >/tmp/mgba.log 2>&1 &

if command -v hyprctl >/dev/null && [ -n "${HYPRLAND_INSTANCE_SIGNATURE:-}" ]; then
    addr=""
    for _ in $(seq 20); do
        sleep 0.25
        addr=$(hyprctl clients -j | python3 -c \
            "import json,sys; c=[c for c in json.load(sys.stdin) if c['class']=='mgba']; print(c[-1]['address'] if c else '')")
        [ -n "$addr" ] && break
    done
    if [ -n "$addr" ]; then
        w=$((240 * SCALE)); h=$((160 * SCALE + 30)) # +30 for mGBA's menu bar
        sel="address:$addr"
        hyprctl repl "return hl.dispatch(hl.dsp.window.float({window='$sel', action='enable'}))" >/dev/null
        hyprctl repl "return hl.dispatch(hl.dsp.window.resize({window='$sel', x=$w, y=$h}))" >/dev/null
        hyprctl repl "return hl.dispatch(hl.dsp.window.set_prop({window='$sel', prop='opaque', value='1'}))" >/dev/null
        # Hand focus back to the launching terminal so stray keypresses
        # (e.g. Enter) don't reach the game; click mGBA to start playing.
        if [ -n "$prev" ]; then
            hyprctl repl "return hl.dispatch(hl.dsp.focus({window='address:$prev'}))" >/dev/null
        fi
    fi
fi
