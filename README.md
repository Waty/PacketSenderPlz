# PacketSenderPlz
A simple bypassless packetsender for Europe MapleStory

## Updating
* Open **MapleStructs.h**
* The second line of the file is **#define EMS**, change it to GMS if that's what you're compiling for
* Update the addys in the correct namespace to the new version using the AOBs provided, and change the **GameVersion** to the version you're compiling for.

PacketSenderPlz should now be updated to the version you want to use it in.

(There are plans to make it update automatically, but for now you'll have to do it manually :wink:)

## Compiling
PacketSenderPlz uses nana as UI toolkit, so you will need to configure that before you're gonna be able to compile the code

Thankfully, nana is included in the [vcpkg](https://github.com/Microsoft/vcpkg/) project from Microsoft, making installation easier then ever:
* Install [vcpkg](https://github.com/Microsoft/vcpkg/) by following the instructions in their README.md
* Let vcpkg download, configure and compile nana for you by executing `.\vcpkg install nana:x86-windows-static`

That's it :smiley: PacketSenderPlz should now compile correctly
