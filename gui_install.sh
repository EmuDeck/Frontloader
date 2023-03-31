#!/bin/bash

zen_nospam() {
  zenity 2> >(grep -v 'Gtk' >&2) "$@"
}

if ! command -v jq &> /dev/null
then
    echo "JQ could not be found, please install it"
    echo "Info on how to install it can be found at https://stedolan.github.io/jq/download/"
    exit
fi

if ! curl -Is https://github.com | head -1 | grep 200 > /dev/null
then
    echo "Github appears to be unreachable, you may not be connected to the internet"
    exit 1
fi

if [[ -f "${HOME}/Frontloader/frontloader" ]] ; then
    BRANCH=$(zen_nospam --title="Frontloader Installer" --width=360 --height=170 --list --radiolist --text "Select Option:" --hide-header --column "Buttons" --column "Choice" --column "Info" TRUE "release" "(Recommended option)" FALSE "prerelease" "(May be unstable)" FALSE "uninstall frontloader" "")
else
    BRANCH=$(zen_nospam --title="Frontloader Installer" --width=300 --height=100 --list --radiolist --text "Select Branch:" --hide-header --column "Buttons" --column "Choice" --column "Info" TRUE "release" "(Recommended option)" FALSE "prerelease" "(May be unstable)" )
fi
if [[ $? -eq 1 ]] || [[ $? -eq 5 ]]; then
    exit 1
fi

if [ "$BRANCH" == "uninstall frontloader" ] ; then
    (
    echo "50" ; echo "# Removing Frontloader Files" ;
    rm -rf ~/Frontloader
    rm -rf ~/devkit-game

    echo "100" ; echo "# Uninstall finished, installer can now be closed";
    ) |
    zen_nospam --progress \
  --title="Frontloader Installer" \
   --width=300 --height=100 \
  --text="Uninstalling..." \
  --percentage=0 \
  --no-cancel
  exit 1
fi
(
echo "15" ; echo "# Creating file structure" ;

mkdir -p ~/Frontloader
pushd ~/Frontloader || exit

echo "30" ; echo "# Finding latest $BRANCH";

IFS=$' \t\r\n'

if [ "$BRANCH" == "release" ]; then
  tag=$(curl --silent https://api.github.com/repos/EmuDeck/Frontloader/releases | jq -r 'map(select(.prerelease==false)) | first | .tag_name')
elif [ "$BRANCH" == "prerelease" ]; then
    tag=$(curl --silent https://api.github.com/repos/EmuDeck/Frontloader/releases | jq -r 'map(select(.prerelease==true)) | first | .tag_name')
fi

file="Frontloader-$tag-SteamDeck-Release.tar.xz"

download="https://github.com/EmuDeck/Frontloader/releases/download/$tag/$file"

echo "45" ; echo "# Installing version tag" ;

curl -OL "$download" 2>&1 | stdbuf -oL tr '\r' '\n' | sed -u 's/^ *\([0-9][0-9]*\).*\( [0-9].*$\)/\1\n#Download Speed\:\2/' | zen_nospam --progress --title "Downloading Frontloader" --text="Download Speed: 0" --width=300 --height=100 --auto-close --no-cancel

tar xf "$file"
rm "$file"

echo "80" ; echo "# Setting up devkit redirection" ;

mkdir -p ~/devkit-game
cat << EOF > ~/devkit-game/devkit-steam
#!/bin/bash
~/Frontloader/frontloader
EOF
chmod +x ~/devkit-game/devkit-steam
popd || exit

echo "100" ; echo "# Install finished, installer can now be closed";

) |
zen_nospam --progress \
  --title="Frontloader Installer" \
  --width=300 --height=100 \
  --text="Installing..." \
  --percentage=0 \
  --no-cancel # not actually sure how to make the cancel work properly, so it's just not there unless someone else can figure it out

if [ "$?" = -1 ] ; then
        zen_nospam --title="Frontloader Installer" --width=150 --height=70 --error --text="Download interrupted."
fi
