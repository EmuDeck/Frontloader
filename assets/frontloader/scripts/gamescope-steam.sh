#!/bin/bash

##
## Session globals
##
export SDL_VIDEO_MINIMIZE_ON_FOCUS_LOSS=0

export HOMETEST_DESKTOP=1
export HOMETEST_DESKTOP_SESSION=plasma

export STEAM_USE_MANGOAPP=1
export MANGOHUD_CONFIGFILE=$(mktemp /tmp/mangohud.XXXXXXXX)

export STEAM_USE_DYNAMIC_VRS=1
export RADV_FORCE_VRS_CONFIG_FILE=$(mktemp /tmp/radv_vrs.XXXXXXXX)

# Plop GAMESCOPE_MODE_SAVE_FILE into $XDG_CONFIG_HOME (defaults to ~/.config).
export GAMESCOPE_MODE_SAVE_FILE="${XDG_CONFIG_HOME:-$HOME/.config}/gamescope/modes.cfg"

# There is no way to set a color space for an NV12
# buffer in Wayland. And the color management protocol that is
# meant to let this happen is missing the color range...
# So just workaround this with an ENV var that Remote Play Together
# and Gamescope will use for now.
export GAMESCOPE_NV12_COLORSPACE=k_EStreamColorspace_BT601

# Make path to gamescope mode save file.
mkdir -p "$(dirname "$GAMESCOPE_MODE_SAVE_FILE")"
touch "$GAMESCOPE_MODE_SAVE_FILE"
echo "Making Gamescope Mode Save file at \"$GAMESCOPE_MODE_SAVE_FILE\""

# Initially write no_display to our config file
# so we don't get mangoapp showing up before Steam initializes
# on OOBE and stuff.
mkdir -p "$(dirname "$MANGOHUD_CONFIGFILE")"
echo "no_display" > "$MANGOHUD_CONFIGFILE"

# Prepare our initial VRS config file
# for dynamic VRS in Mesa.
mkdir -p "$(dirname "$RADV_FORCE_VRS_CONFIG_FILE")"
echo "1x1" > "$RADV_FORCE_VRS_CONFIG_FILE"

# Let's try this across the board to see if it breaks anything
# Helps performance in HZD, Cyberpunk, at least
# Expose 8 physical cores, instead of 4c/8t
export WINE_CPU_TOPOLOGY=8:0,1,2,3,4,5,6,7

# To expose vram info from radv's patch we're including
export WINEDLLOVERRIDES=dxgi=n

# Workaround for steam getting killed immediatly during reboot
# See: jupiter/tasks/-/issues/280
export STEAMOS_STEAM_REBOOT_SENTINEL="/tmp/steamos-reboot-sentinel"
if [[ -e "$STEAMOS_STEAM_REBOOT_SENTINEL" ]]; then
	rm -f "$STEAMOS_STEAM_REBOOT_SENTINEL"
	/usr/bin/steamos-polkit-helpers/steamos-reboot-now
	sudo -n reboot
fi

# Workaround for steam getting killed immediatly during shutdown
# Same idea as reboot sentinel above
export STEAMOS_STEAM_SHUTDOWN_SENTINEL="/tmp/steamos-shutdown-sentinel"
if [[ -e "$STEAMOS_STEAM_SHUTDOWN_SENTINEL" ]]; then
	rm -f "$STEAMOS_STEAM_SHUTDOWN_SENTINEL"
	/usr/bin/steamos-polkit-helpers/steamos-poweroff-now
	sudo -n poweroff
fi

# Enable dynamic backlight, we have the kernel patch to disable events
export STEAM_ENABLE_DYNAMIC_BACKLIGHT=1

# Enabled fan control toggle in steam
export STEAM_ENABLE_FAN_CONTROL=1

# Enable volume key management via steam for this session
export STEAM_ENABLE_VOLUME_HANDLER=1

# Have SteamRT's xdg-open send http:// and https:// URLs to Steam
export SRT_URLOPEN_PREFER_STEAM=1

# Disable automatic audio device switching in steam, now handled by wireplumber
export STEAM_DISABLE_AUDIO_DEVICE_SWITCHING=1

# Enable support for xwayland isolation per-game in Steam
export STEAM_MULTIPLE_XWAYLANDS=1

# We have the Mesa integration for the fifo-based dynamic fps-limiter
export STEAM_GAMESCOPE_DYNAMIC_FPSLIMITER=1

# Support for gamescope tearing with GAMESCOPE_ALLOW_TEARING atom
export STEAM_GAMESCOPE_HAS_TEARING_SUPPORT=1

# We have gamma/degamma exponent support
export STEAM_GAMESCOPE_COLOR_TOYS=1

# We have NIS support
export STEAM_GAMESCOPE_NIS_SUPPORTED=1

# Enable tearing controls in steam
export STEAM_GAMESCOPE_TEARING_SUPPORTED=1

# Enable VRR controls in steam
#export STEAM_GAMESCOPE_VRR_SUPPORTED=1

# When set to 1, a toggle will show up in the steamui to control whether dynamic refresh rate is applied to the steamui
export STEAM_GAMESCOPE_DYNAMIC_REFRESH_IN_STEAM_SUPPORTED=0

# Don't wait for buffers to idle on the client side before sending them to gamescope
export vk_xwayland_wait_ready=false

# Set refresh rate range and enable refresh rate switching
export STEAM_DISPLAY_REFRESH_LIMITS=40,60

# Let steam know it can unmount drives without superuser privileges
export STEAM_ALLOW_DRIVE_UNMOUNT=1

# We no longer need to set GAMESCOPE_EXTERNAL_OVERLAY from steam, mangoapp now does it itself
export STEAM_DISABLE_MANGOAPP_ATOM_WORKAROUND=1

# Enable horizontal mangoapp bar
export STEAM_MANGOAPP_HORIZONTAL_SUPPORTED=1

# Set input method modules for Qt/GTK that will show the Steam keyboard
export QT_IM_MODULE=steam
export GTK_IM_MODULE=Steam

# Make Qt apps use the styling and behaviour of the desktop session
# This fixes some missing icons and unreadable text with Qt desktop apps in gamescope
export QT_QPA_PLATFORM_THEME=kde

# TODO!
# Bring this back when gamescope side is more complete
#
# Remove vsync handling from Xwayland, we handle it in gamescope
#export vblank_mode=0
#export MESA_VK_WSI_PRESENT_MODE=immediate

# To play nice with the short term callback-based limiter for now
export GAMESCOPE_LIMITER_FILE=$(mktemp /tmp/gamescope-limiter.XXXXXXXX)

export XCURSOR_THEME=steam

# Workaround for Steam login issue while Steam client change propagates out of Beta
touch ~/.steam/root/config/SteamAppData.vdf || true

export STEAM_UPDATEUI_PNG_BACKGROUND=/usr/share/steamos/steamos.png

ulimit -n 524288

# Create run directory file for startup and stats sockets
#   shellcheck disable=SC2030 # (broken warning)
tmpdir="$([[ -n ${XDG_RUNTIME_DIR+x} ]] && mktemp -p "$XDG_RUNTIME_DIR" -d -t gamescope.XXXXXXX)"
socket="${tmpdir:+$tmpdir/startup.socket}"
stats="${tmpdir:+$tmpdir/stats.pipe}"
# Fail early if we don't have a proper runtime directory setup
#   shellcheck disable=SC2031 # (broken warning)
if [[ -z $tmpdir || -z ${XDG_RUNTIME_DIR+x} ]]; then
	echo >&2 "!! Failed to find run directory in which to create stats session sockets (is \$XDG_RUNTIME_DIR set?)"
	exit 0
fi

# 1048576 = 1M - passing it like that omits the 'M' suffix - xargs removes whitespace
free_disk_space_megs=$(df ~/ --output=avail -B1048576 | sed -n '2 p' | xargs)
minimum_free_disk_space_needed_megs=500

if [[ "$free_disk_space_megs" -lt "$minimum_free_disk_space_needed_megs" ]]; then
	echo >&2 "gamescope-session: not enough disk space to proceed, trying to find game to delete"

	find ~/.local/share/Steam/steamapps/common/ -mindepth 1 -maxdepth 1 -type d -printf "%T@ %p\0" | sort -n -z | while IFS= read -r -d $'\0' line; do
		timestamp=${line%% *}
                game_folder=${line#* }

		[[ -d $game_folder ]]  || continue

		acf=$(grep -F -- "$(basename -- "$game_folder")" ~/.local/share/Steam/steamapps/*.acf | grep \"installdir\" | cut -d: -f1)
		[[ -e "$acf" ]] || continue

		echo >&2 "gamescope-session: deleting $(basename "$game_folder")"
		appid=$(basename "$acf" | cut -d_ -f2 | cut -d. -f1)

		# TODO leave a note for Steam to display UI to explain what happened, if this logic stays
		# intentionally leave compatdata; could be unclouded save files there
		rm -rf --one-file-system -- "$game_folder" "$acf" ~/.local/share/Steam/steamapps/shadercache/"$appid"

		free_disk_space_megs=$(df ~/ --output=avail -B1048576 | sed -n '2 p' | xargs)
		[[ "$free_disk_space_megs" -lt "$minimum_free_disk_space_needed_megs" ]] || break
	done
fi

export GAMESCOPE_STATS="$stats"
mkfifo -- "$stats"
mkfifo -- "$socket"

# Attempt to claim global session if we're the first one running (e.g. /run/1000/gamescope)
linkname="gamescope-stats"
#   shellcheck disable=SC2031 # (broken warning)
sessionlink="${XDG_RUNTIME_DIR:+$XDG_RUNTIME_DIR/}${linkname}" # Account for XDG_RUNTIME_DIR="" (notfragileatall)
lockfile="$sessionlink".lck
exec 9>"$lockfile" # Keep as an fd such that the lock lasts as long as the session if it is taken
if flock -n 9 && rm -f "$sessionlink" && ln -sf "$tmpdir" "$sessionlink"; then
	# Took the lock.  Don't blow up if those commands fail, though.
	echo >&2 "Claimed global gamescope stats session at \"$sessionlink\""
else
	echo >&2 "!! Failed to claim global gamescope stats session"
fi

gamescope \
	--generate-drm-mode fixed \
	--xwayland-count 2 \
	-w 1280 -h 800 --fullscreen \
	--default-touch-mode 4 \
	--hide-cursor-delay 3000 \
	--max-scale 2 \
	--fade-out-duration 200 \
	-e -R "$socket" -T "$stats" \
	-O '*',eDP-1 \
	--cursor-hotspot 5,3 --cursor /usr/share/steamos/steamos-cursor.png \
	&
gamescope_pid="$!"

OLD_DISPLAY=$DISPLAY

if read -r -t 3 response_x_display response_wl_display <> "$socket"; then
	export DISPLAY="$response_x_display"
	export GAMESCOPE_WAYLAND_DISPLAY="$response_wl_display"
	# We're done!
else
	kill -9 "$gamescope_pid"
	wait
	exit 0
	# SDDM will restart us
fi

xbindkeys -f /etc/xbindkeysrc

# Input method support
/usr/bin/ibus-daemon -d -r --panel=disable --emoji-extension=disable

(while true; do
    /usr/lib/hwsupport/power-button-handler.py
done) &


short_session_tracker_file="/tmp/steamos-short-session-tracker"
short_session_duration=60
short_session_count_before_reset=3
SECONDS=0

short_session_count=$(< "$short_session_tracker_file" wc -l)

if [[ "$short_session_count" -ge "$short_session_count_before_reset" ]]; then
	# TODO could detect if we're in the devkit case below and undo that instead
	# instead of resetting the wrong thing
	# also could print full list of files and sizes in the Steam dir and save as a diagnostic report
	echo >&2 "gamescope-session: detected broken Steam, bootstrapping from OS copy..."
	mkdir -p ~/.local/share/Steam
	# remove some caches and stateful things known to cause Steam to fail to start if corrupt
	rm -rf --one-file-system ~/.local/share/Steam/config/widevine
	# restore clean copy of binaries from RO partition
	tar xf /usr/lib/steam/bootstraplinux_ubuntu12_32.tar.xz -C ~/.local/share/Steam
	# rearm
	rm "$short_session_tracker_file"
fi

# This will re-send automount notifications for steam, and has a builtin timeout to wait for steam to come up.
# _eventually_ steam should learn to talk to udisks so we don't have to rube goldberg this info to it.
/usr/bin/steamos-polkit-helpers/steamos-retrigger-automounts

(while true; do
    window=$(DISPLAY=$OLD_DISPLAY xdotool search --class gamescope)

    if [[ -n "$window" ]]; then
        echo >&2 "gamescope-session: $window $DISPLAY $OLD_DISPLAY $(DISPLAY=$OLD_DISPLAY xdotool getwindowclassname $window)"
        DISPLAY=$OLD_DISPLAY xprop -f STEAM_BIGPICTURE 32c -set STEAM_BIGPICTURE 1 -id $window
        break
    fi
done) &
window_pid=$!

mangoapp &
$@

if [[ "$SECONDS" -lt "$short_session_duration" ]]; then
	echo "frog" >> "$short_session_tracker_file"
else
	rm "$short_session_tracker_file"
fi

# Ask gamescope to exit nicely
kill $gamescope_pid
kill $window_pid
# Start a background sleep for five seconds because we don't trust it
sleep 5 &
sleep_pid=$!

# Wait for gamescope or the sleep to finish for timeout purposes
ret=0
wait -n $gamescope_pid $sleep_pid || ret=$?

# If we get a SIGTERM/etc while waiting this happens.  Proceed to kill -9 everything but complain
if [[ $ret = 127 ]]; then
	echo >&2 "gamescope-session: Interrupted while waiting on teardown, force-killing remaining tasks"
fi

# Kill all remaining jobs and warn if unexpected things are in there (should be just sleep_pid, unless gamescope failed
# to exit in time or we hit the interrupt case above)
for job in $(jobs -p); do
	# Warn about unexpected things
	if [[ $ret != 127 && $job = "$gamescope_pid" ]]; then
		echo >&2 "gamescope-session: gamescope timed out while exiting, killing"
	elif [[ $ret != 127 && $job != "$sleep_pid" ]]; then
		echo >&2 "gamescope-session: unexpected background pid $job at teardown: "
		# spew some debug about it
		ps -p "$job" >&2
	fi
	kill -9 "$job"
done

# This should just be waiting on kill -9'd things. Another signal will cause us to give up, but we should be a little
# stubborn about not letting the session die with gamescope holding on to things.
wait
