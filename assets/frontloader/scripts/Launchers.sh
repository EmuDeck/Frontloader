#!/bin/bash

cd "${0%/*}" || exit
. ../scripts/Events.sh
. ../scripts/EmuDeck.sh

function Desktop()
{
  onLaunch Desktop

  steamos-session-select plasma

  touch /tmp/front-desktop
}

function ES-DE()
{
  onLaunch ES-DE
    "$(EmulationFolder)/tools/EmulationStation-DE-x64_SteamDeck.AppImage"
  onExit ES-DE
}

function SteamOS()
{
  onLaunch SteamOS
    gamescope-session-steam
  onExit SteamOS
}