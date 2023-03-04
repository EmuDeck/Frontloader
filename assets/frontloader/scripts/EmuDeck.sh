#!/bin/bash

function EmulationFolder() {
  if [ -d "$HOME/Emulation" ]; then
    echo "$HOME/Emulation"
  elif [ -d "/var/run/media/mmcblk0p1/Emulation" ]; then
    echo "/var/run/media/mmcblk0p1/Emulation"
  else
    return 1
  fi
}