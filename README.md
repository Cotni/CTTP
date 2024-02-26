# CTTP

CTTP is a [Pulsar](https://github.com/MelgMKW/Pulsar)-based Mario Kart Wii custom track testing environment.
Along with all Pulsar features, it provides additional debug ones:

- A performance monitor who measures various game processes. From bottom to top: total frame time, SceneManager::Draw, GPU, SceneManager::calc, physics, objects, items, effects. 
- A debug panel, which displays useful race information (such as checkpoints, kcl etc...).
Both of these can be disabled offline via settings, but will forced ON online.
- Item cheats offline. All non-triple items are available.
- Ghost saving has been altered. If a cheated (via items) ghost is created, it'll save and replay normally, with the same cheats as the one used during the race.
- Visual ITPT and Checkpoint display.

On Dolphin only:
- SZS hotswap. regular tracks SZS can be dumped in "Dolphin Emulator\Wii\Course" and will replace vanilla files, even if the game is already running.
- Easy ghost insertion. The mod should be run once, and then ghosts should be put in "Dolphin Emulator\Wii\shared2\Pulsar\CTTP\Ghosts" with any file name under 13 characters. They'll display in-game over their correct vanilla track.

Any Pulsar config will work on CTTP, but only config.pul should be changed. If you want to build your own config, use the "Build Config Only" of the Pulsar Pack Creator Software.

CTTP code.pul can only be built by including Pulsar's engine. 
