pSNES: Portable Snes9x
===========================

![](https://github.com/Cpasjuste/pemu/raw/master/psnes/data/screenshot.png)


### The main goals of this [snes9x](https://github.com/snes9xgit/snes9x) port are:

- have a basic but user friendly user interface on non windows devices
- use a recent rendering interface for speed and effects support
- cross-platform and code portability
- easy updates of snes9x core (no modifications to the original sources)

### Currently supported devices

- Linux (sdl1, sdl2)
- Nintendo Switch (libnx, sdl2)
- Sony Ps Vita (vitasdk, vita2d)
- Sony PS4

### Capabilities
- Support save states
- Support cheats (put a "rom_name.cht"  file with game genie codes in cheat folder)
- Support 2x, 3x, fit, fit 4/3 and fullscreen hardware linear scaling
- Support shaders effects

### Romset
- The pSNES database is built against the no-intro romset "Nintendo - Super Nintendo Entertainment System (2017-05-29).zip". The best is to use this romset with pSNES.

### Preview images and videos
- pSNES support preview images and videos (put files in roms media folder: png files goes in "roms/media/mixrbv2", mp4 files in "roms/media/video"). Please note that the vita port doesn't support video previews.
You can download a near fullset mixrbv2 and video package here:
    - preview images: [mixrbv2](http://files.mydedibox.fr/files/dev/pemu/psnes/psnes_mixrbv2.zip) (1.3GB)
    - preview videos: [video](http://files.mydedibox.fr/files/dev/pemu/psnes/psnes_video.zip) (9.2GB)

### Cheat format
- Example for "Super Mario World (USA).cht" game genie codes
   ```
   cheat
     name: UnlimitedJumps
     code: 3E2C-AF6F
     enable

   cheat
     name: 99Lives
     code: 14B4-6F07
     enable
   ```

### Installation (switch)
- copy "psnes" directory to "/switch/" directory on sdcard
- copy roms to "/switch/psnes/roms/" directory on sdcard

### Installation (vita)
- copy "psnes" directory to "ux0:/data/" directory on sdcard
- copy roms to "/ux0/data/psnes/roms/" directory on sdcard

### Installation (ps4)
- create "/data/psnes" directory on hdd
- copy roms to "/data/psnes/roms" directory on hdd

### Usage (switch)
- Press (+) in the rom list to configure main options and default roms options
- Press (-) in the rom list to configure per rom options
- Press (+) AND (-) when in emulation to bring the options menu

### Thanks
- ScHlAuChi
- devkitPro and libnx people: yellows8, plutoo, profi200, WntrMute ....
- ScHlAuChi again :)
