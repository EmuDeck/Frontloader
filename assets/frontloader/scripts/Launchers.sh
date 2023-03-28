#!/bin/bash
cd "${0%/*}" || return
. ../scripts/Logging.sh

function launchSteam()
{
  info "Starting nested gamescope for command $* with steam integration"
  ../scripts/gamescope-steam.sh "$@"
}

function launchOther()
{
  info "Starting nested gamescope for command $*"
  ../scripts/gamescope-other.sh "$@"
}
