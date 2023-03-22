#!/bin/bash

function info()
{
  echo "[i]" "$*"
}

function warning()
{
  echo "[w]" "$*"
}

function error()
{
  echo "[e]" "$*"
  return 1
}