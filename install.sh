#!/bin/bash

mkdir -p ~/Frontloader
pushd ~/Frontloader || exit

IFS=$' \t\r\n'

tag=$(curl --silent https://api.github.com/repos/EmuDeck/Frontloader/releases | jq -r 'map(select(.prerelease==false)) | first | .tag_name')

file="Frontloader-$tag-SteamDeck-Release.tar.xz"

download="https://github.com/EmuDeck/Frontloader/releases/download/$tag/$file"

curl -OL "$download"



tar xf "$file"
rm "$file"
mkdir -p ~/devkit-game
cat << EOF > ~/devkit-game/devkit-steam
#!/bin/bash
if [ ! -f ~/Frontloader/frontloader ]; then
  steam -steamdeck -steamos3 -steampal -gamepadui
else
  ~/Frontloader/frontloader
fi
EOF
chmod +x ~/devkit-game/devkit-steam
popd || exit
