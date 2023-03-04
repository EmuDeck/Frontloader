#!/bin/bash

cd "${0%/*}" || return
. ../scripts/Logging.sh

function construct()
{
  info "Hello World"
  info "SERVICE_DIR=$SERVICE_DIR"
  info "Parameters: $*"
}

function destruct()
{
  info "Hello World"
  info "SERVICE_DIR=$SERVICE_DIR"
  info "Parameters: $*"
}

function launch()
{
  info "Hello World"
  info "SERVICE_DIR=$SERVICE_DIR"
  info "Parameters: $*"
}

function exit()
{
  info "Hello World"
  info "SERVICE_DIR=$SERVICE_DIR"
  info "Parameters: $*"
}