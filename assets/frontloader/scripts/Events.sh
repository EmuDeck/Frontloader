#!/bin/bash
cd "${0%/*}" || return
. ../scripts/Logging.sh

export EVENTS=("construct" "destruct" "launch" "exit")
export FIELDS=("SERVICE_NAME" "SERVICE_VERSION")
export SERVICES="../services/*"


function onLaunch()
{
  info "Starting $1"
  callEventHandlers "launch" "$@"
  return 0
}

function onExit()
{
  info "Exiting $1"
  callEventHandlers "exit" "$@"
  return 0
}

function onConstruct()
{
  info "Constructing..."

  if [ -f /tmp/front-desktop ]; then
    onExit Desktop
    rm /tmp/front-desktop
  fi
  callEventHandlers "construct" "$@"

  return 0
}

function onDestruct()
{
  info "Destructing..."
  callEventHandlers "destruct" "$@"
  return 0
}

function callEventHandlers()
{
  cd "${0%/*}" || exit
  local event="$1"
  for service in $SERVICES; do
    (
      . "$service"/events.sh
      . "$service"/meta.sh
      case $event in
      construct)
        info "Constructing $SERVICE_NAME version $SERVICE_VERSION"
        ;;
      destruct)
        info "Destructing $SERVICE_NAME version $SERVICE_VERSION"
        ;;
      launch)
        info "Running launch handler for $SERVICE_NAME version $SERVICE_VERSION"
        ;;
      exit)
        info "Running exit handler for $SERVICE_NAME version $SERVICE_VERSION"
        ;;
      esac
      export SERVICE_DIR=$service
      pushd "$SERVICE_DIR" > /dev/null || exit
      "$event" "${@:2}"
      popd > /dev/null || exit
      unset SERVICE_DIR
    )
  done
}