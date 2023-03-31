# Frontloader

A frontend loader for the Valve Steam Deck, based off of the [pegasus emulator frontend](https://github.com/mmatyas/pegasus-frontend)

Essentially, Frontloader sits in between gamescope and steam and allows for booting into frontends other than steam, like for example, ES-DE, Desktop mode, or even [OpenGamepadUI](https://github.com/ShadowBlip/OpenGamepadUI)

## Installation

### Gui (Recommended)
the installer can be downloaded [here](https://github.com/EmuDeck/Frontloader/releases/latest/download/install_frontloader.desktop)


### Command Line (Advanced)
run this command on your steam deck either in a terminal/ssh or within a script

```bash
curl -s https://raw.githubusercontent.com/EmuDeck/Frontloader/master/install.sh | bash
```

or if you like to live on the edge and use the latest commit, use this one

```bash
curl -s https://raw.githubusercontent.com/EmuDeck/Frontloader/master/install_canary.sh | bash
```

to uninstall, delete the `Frontloader` folder in your home directory as well as the `devkit-game` folder in your home directory,
or just run this command that does it for you

```bash
curl -s https://raw.githubusercontent.com/EmuDeck/Frontloader/master/uninstall.sh | bash
```