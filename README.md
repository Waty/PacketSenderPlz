# PacketSenderPlz
A simple bypassless packetsender for Europe MapleStory

## Compiling
* Dowload all the required code:
 * Get a fresh copy of the repo, you can do thi using git clone, or the download link GitHub offers
 * Get the nana v1.0.1 release from [here.](https://github.com/cnjinhao/nana/releases) (you can try newer versions, but compatibility isn't guareanteed.
* Compile Nana for PacketSenderPlz
 * Open \build\vc2013\nana.sln
 * Right mouse click on the **nana** project and select **Properties**
 * Go to **C/C++ -> General** and add an **Additional Include Directory** of (nana v1.0.1 folder)/include
 * Go to **C/C++ -> Code Generation** and set **Runtime Library** to **/MTd** for Debug build and **/MT** for Release builds
 * Close the properties by clicking **OK**
 * Build release and debug lib using **Build -> Batch Build -> Select All -> Build**
* Open the PacketSenderPlz folder. (PacketSenderPlz-master\PacketSenderPlz) If you opened the correct folder, you should see some .cpp files
* Create a folder called **nana**
  * Copy the include folder from the nana download to it
  * Create a folder called **libs** in the **nana** folder
  * Copy the 2 lib files in **\build\bin\vc2013** to the libs folder
* The PacketSenderPlz project should now compile correctly
