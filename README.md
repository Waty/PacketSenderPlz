# PacketSenderPlz
A simple bypassless packetsender for Europe MapleStory

## How to compile
1. Dowload all the required code:
 1. Get a fresh copy of the repo, you can do thi using git clone, or the download link GitHub offers
 2. Get the nana v1.0.1 release from [here.](https://github.com/cnjinhao/nana/releases) (you can try newer versions, but compatibility isn't guareanteed.
2. Compile Nana for PacketSenderPlz
 1. Open \build\vc2013\nana.sln
 2. Right mouse click on the **nana** project and select **Properties**
 3. Go to **C/C++ -> General** and add an **Additional Include Directory** of (nana v1.0.1 folder)/include
 4. Go to **C/C++ -> Code Generation** and set **Runtime Library** to **/MTd** for Debug build and **/MT** for Release builds
 5. Close the properties by clicking **OK**
 6. Build release and debug lib using **Build -> Batch Build -> Select All -> Build**
3. Open the PacketSenderPlz folder. (PacketSenderPlz-master\PacketSenderPlz) If you opened the correct folder, you should see some .cpp files
4. Create a folder called **nana**
  1. Copy the include folder from the nana download to it
  2. Create a folder called **libs** in the **nana** folder
  3. Copy the 2 lib files in **\build\bin\vc2013** to the libs folder
5. The PacketSenderPlz project should now compile correctly
