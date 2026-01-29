# Solar System Project

A realistic, interactive 3D solar system visualization built using **OpenGL**, **GLUT**, and **GLU**, featuring textured planets, dynamic day/night cycles, orbiting moons/satellites, Saturn's rings, Venus atmosphere, Earth clouds, skybox background, camera controls, orbit toggling, and smooth animation.

![Solar System Screenshot](screenshots/1.png)
![](screenshots/2.png)
![](screenshots/3.png)


## Features

- Realistic planetary orbits and self-rotation
- Textured planets (8K/4K/2K where available) using **stb_image**
- Special effects:
  - Earth: day/night cycle with city lights + moving clouds + atmosphere glow
  - Saturn: semi-transparent textured rings
  - Venus: glowing atmosphere overlay
  - Moon and two artificial satellites orbiting Earth
- Dynamic skybox (Milky Way)
- Orbit paths visualization (toggle with `B`)
- Free camera movement (keyboard + optional mouse look)
- Automatic orbiting camera mode (toggle with `O`)
- Real-time FPS counter
- Informative HUD showing camera position, object counts, and controls

## Controls

| Key              | Action                              |
|------------------|-------------------------------------|
| **W / X / Z**    | Move camera Up / Down / Zoom Out    |
| **A / D**        | Move camera Left / Right            |
| **S**            | Zoom In                             |
| **R**            | Reset camera position               |
| **O**            | Toggle automatic camera orbit       |
| **M**            | Toggle mouse-look control           |
| **B**            | Toggle orbit lines visibility       |
| Arrow keys / F1-F2 | Alternative camera movement       |

## Requirements

- **C++ compiler** (g++, MSVC, clang)
- **OpenGL** + **GLUT** + **GLU** development libraries
  

## Project Structure<br>
solar-system-project/<br>
├── index.cpp                # The complete source code<br>
├── textures/               # All planet/sky textures go here<br>
│   ├── 8k_sun.jpg<br>
│   ├── 8k_earth.jpg<br>
│   ├── 8k_earth_night.jpg<br>
│   ├── 8k_earth_clouds.jpg<br>
│   ├── 8k_mercury.jpg<br>
│   ├── 2k_mars.jpg<br>
│   ├── 8k_jupiter.jpg<br>
│   ├── 2k_saturn.jpg<br>
│   ├── 2k_saturn_ring_alpha.png<br>
│   ├── 2k_uranus.jpg<br>
│   ├── 2k_neptune.jpg<br>
│   ├── 8k_moon.jpg<br>
│   ├── 8k_stars_milky_way.jpg<br>
│   ├── panneaux.jpg        # Satellite body<br>
│   ├── anttenne.png        # Satellite antenna<br>
│   └── metal.png           # Satellite thrusters/parts<br>
├── screenshots/            <br>
├── stb_image.h<br>
└── README.md<br>


Where to get Textures?
You can download high-quality planet textures from these free sources:

https://www.solarsystemscope.com/textures/


## Known Limitations / Future Improvements

No collision detection or accurate astronomical scale (artistic representation)
Performance may drop with very high-resolution textures on low-end hardware
No shader support (fixed-function pipeline – compatible with old systems)
Possible future additions:
Realistic lighting with normal maps
Comet/asteroid belt
Time acceleration control
Planet name labels in 3D


## Author

Hanine Rouibah
Created in 2025 as part of a Computer Graphics project.

## License
MIT License – feel free to use, modify, and share!
