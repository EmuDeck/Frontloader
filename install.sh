#!/bin/bash

mkdir -p ~/Frontloader
pushd ~/Frontloader || exit

IFS=$' \t\r\n'

assets=$(curl https://api.github.com/repos/EmuDeck/Frontloader/releases | jq -r '.[0].assets[].browser_download_url')

for asset in $assets; do
    curl -OL $asset
done

unzip -o frontloader*.zip
rm frontloader*.zip
mkdir -p ~/devkit-game
cat << EOF > ~/devkit-game/devkit-steam
#!/bin/bash
~/Frontloader/frontloader
EOF
chmod +x ~/devkit-game/devkit-steam
popd || exit
