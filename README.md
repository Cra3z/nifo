# Nifo 3D模型查看器

Nifo 是一款使用 Qt6 与 C++20 编写的 3D模型查看器

[![License: MIT](https://img.shields.io/badge/License-MIT-blue.svg)](https://opensource.org/licenses/MIT)
[![Language](https://img.shields.io/badge/language-C++-blue.svg)](https://isocpp.org/)
[![Standard](https://img.shields.io/badge/c%2B%2B-20-blue.svg)](https://en.wikipedia.org/wiki/C%2B%2B20)
---
要求显卡驱动支持OpenGL 4.5  

Nifo仍处于开发中, 目前已实现的功能:
* 模型导入
* 场景树
* 物体属性编辑
* 物体选择
* Phong、Blinn-Phong光照
* MRT

一些功能的演示:  

![nifo-import-3d-model](screenshot/nifo-import-3d-model.png)  

![nifo-demo1](screenshot/nifo-demo1.png)  

![nifo-demo2](screenshot/nifo-demo2.png)  

---

## 构建 Nifo
请确保你已经安装Qt6并且已设置好环境变量Qt6_DIR(Qt6_DIR应当是你所使用的Qt6工具链目录下的lib目录下的cmake目录, 例如D:/Qt/6.7.0/msvc2019_64/lib/cmake)

该项目依赖的其它第三方库通过[vcpkg](https://github.com/Microsoft/vcpkg)管理, 如果你没有vcpkg请先获取它
```shell
git clone https://github.com/Microsoft/vcpkg.git
```

使用CMake构建该项目(其中%VCPKG_ROOT%表示你的vcpkg的安装目录)
```shell
cmake -S . -B build -DCMAKE_TOOLCHAIN_FILE=%VCPKG_ROOT%/scripts/buildsystems/vcpkg.cmake

cmake --build build
```

---  

## 依赖的第三方库

* [assimp](https://github.com/assimp/assimp)   5.3.1
* [entt](https://github.com/skypjack/entt)  3.13.1
* [glm](https://github.com/g-truc/glm) 1.0.1
* [spdlog](https://github.com/gabime/spdlog)    1.13.0
* [tbb](https://github.com/oneapi-src/oneTBB)  2021.11.0

---  

## License

Nifo is released under the [MIT](https://opensource.org/licenses/MIT) license. See LICENSE file.