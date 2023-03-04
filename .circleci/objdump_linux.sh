#! /bin/bash

set -o errexit
set -o nounset
set -o xtrace

${1}objdump -p installdir/frontloader | grep 'NEEDED' | sort
