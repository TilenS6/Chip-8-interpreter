```
git clone --recursive https://github.com/TilenS6/SDL2-mega_utils-Template.git
```

# Windows:
- Modify tasks.json, launch.json
- You need include/SDL2/
- You need lib/
- You don't need lib_linux

# Linux:
- Modify tasks.json, launch.json
- You don't need include/SDL2/
- You don't need lib/
- You may need lib_linux/
- Setup:

```bash
sudo apt install libsdl2-dev libsdl2-image-dev libsdl2-ttf-dev
```