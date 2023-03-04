#! /bin/bash

set -o errexit
set -o nounset
set -o xtrace

zip -j dist/frontloader_$(git describe --always)_${QT_TARGET}.zip \
    installdir/pegasus-fe \
    README.md \
    LICENSE.md
