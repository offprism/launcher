<div align="center">

<img width="160" height="160" align="center" src="./docs/logo.png" alt="OffPrism logo">

<h1>
<a href="https://github.com/offprism/launcher">OffPrism</a>
</h1>

A Minecraft launcher with **offline, Ely.by and authlib-injector accounts**,<br>
separate instances and mods and modpacks from **Modrinth, CurseForge, FTB, Technic and ATLauncher** in one click

A fork of [Prism Launcher](https://prismlauncher.org/) — **not** endorsed by or affiliated with its team

<p align="center">
<strong>English</strong> | <a href="./README_ru.md">Русский</a>
</p>

<div>

[![Latest release](https://img.shields.io/github/v/release/offprism/launcher?label=Release&style=for-the-badge&color=8a8a8a)](https://github.com/offprism/launcher/releases/latest)
[![Downloads](https://img.shields.io/github/downloads/offprism/launcher/total?label=Downloads&style=for-the-badge&color=8a8a8a)](https://github.com/offprism/launcher/releases)
[![GitHub Repo stars](https://img.shields.io/github/stars/offprism/launcher?label=Stars&style=for-the-badge&color=8a8a8a)](https://github.com/offprism/launcher/stargazers)
![No ads](https://img.shields.io/badge/ads-none-8a8a8a?style=for-the-badge)

</div>

</div>

## Features

- **Play offline** without a Microsoft account — just pick a nickname
- **Ely.by accounts** with your skin shown in the launcher and **in game**, without any mods (two-factor authentication supported)
- **authlib-injector accounts** for any compatible skin server — LittleSkin, Blessing Skin and others; [authlib-injector](https://github.com/yushijinhun/authlib-injector) ships with the launcher
- Microsoft accounts still work too
- **Glass theme** — a dark translucent theme with the Windows 11 acrylic backdrop
- **Instances** — every build has its own mods, resource packs, shaders, worlds and settings, and never interferes with another
- **Mod catalog** for Modrinth and CurseForge with dependencies installed automatically and **update all** in one click
- **Modpacks from five libraries** — Modrinth, CurseForge, FTB, Technic and ATLauncher
- **One-click launcher updates** straight from this repository's releases
- Builds for **Windows (x64 and ARM64), Linux and macOS** (Intel and Apple Silicon)

## Comparison

| Feature                                        | OffPrism          | Prism Launcher  |
|------------------------------------------------|-------------------|-----------------|
| Offline mode without a Microsoft account       | ✅                | ❌              |
| Ely.by accounts with skins in game             | ✅                | ❌              |
| authlib-injector accounts (LittleSkin, ...)    | ✅                | ❌              |
| Microsoft accounts                             | ✅                | ✅              |
| Translucent glass theme                        | ✅                | ❌              |
| Separate instances                             | ✅                | ✅              |
| Built-in Modrinth & CurseForge catalog         | ✅                | ✅              |
| FTB, Technic and ATLauncher modpacks           | ✅                | ✅              |
| No ads or telemetry                            | ✅                | ✅              |
| Based on                                       | Prism Launcher    | PolyMC          |

## Installation

### Stable releases

Download OffPrism from the [GitHub Releases](https://github.com/offprism/launcher/releases/latest) page:

| System                          | File                                                 |
|---------------------------------|------------------------------------------------------|
| Windows                         | `OffPrism-Windows-MSVC-Setup-<version>.exe`          |
| Windows (portable)              | `OffPrism-Windows-MSVC-Portable-<version>.zip`       |
| Windows on ARM                  | `OffPrism-Windows-MSVC-arm64-Setup-<version>.exe`    |
| Windows on ARM (portable)       | `OffPrism-Windows-MSVC-arm64-Portable-<version>.zip` |
| Linux                           | `OffPrism-Linux-x86_64.AppImage`                     |
| Linux (portable)                | `OffPrism-Linux-Qt6-Portable-<version>.tar.gz`       |
| macOS (Intel and Apple Silicon) | `OffPrism-macOS-<version>.dmg`                       |

Once installed, the launcher updates itself from this repository.

The builds are not code-signed: Windows SmartScreen may ask you to confirm ("More info" → "Run anyway"), and on macOS open the app with right click → "Open" the first time.

### Development builds

Every commit to `main` is built by [GitHub Actions](https://github.com/offprism/launcher/actions); the builds are attached to each run as artifacts. They are not meant for most users and may be broken. You have been warned.

## Community & Support

Found a bug or want a feature? Open an issue in [GitHub Issues](https://github.com/offprism/launcher/issues). Pull requests are welcome.

## Building from source

OffPrism builds exactly like Prism Launcher: CMake, Qt 6.8+, a C++20 compiler, a JDK 17 and [vcpkg](https://vcpkg.io/) for the remaining libraries.

```bash
git clone --recursive https://github.com/offprism/launcher.git
cd launcher
cmake --preset windows_msvc   # or linux, macos_universal, windows_mingw
cmake --build --preset windows_msvc --config Release
```

[`.github/workflows/build.yml`](.github/workflows/build.yml) shows the exact steps for every platform. The release name and version are set in the top-level `CMakeLists.txt` (`Launcher_VERSION_*` and `Launcher_VERSION_CODENAME`).

## Authors

- [offprism](https://github.com/offprism) — developer
- [Prism Launcher](https://github.com/PrismLauncher/PrismLauncher), PolyMC and MultiMC contributors — the launcher this fork is built on

## Other

<ul>
  <li>We <strong>ARE NOT</strong> related to the <a href="https://prismlauncher.org/">Prism Launcher</a> team. Please do not send them bug reports about this build.</li>
  <li>We <strong>ARE NOT</strong> collecting your information.</li>
  <li>Passwords of Ely.by and authlib-injector accounts are never saved — only the session token is.</li>
  <li>We <strong>ARE</strong> open to contributions.</li>
</ul>

## License

[![License](https://img.shields.io/badge/license-GPL--3.0-8a8a8a?style=for-the-badge)](LICENSE)

OffPrism is licensed under the GNU General Public License v3.0, inherited from Prism Launcher. See [LICENSE](LICENSE) and [COPYING.md](COPYING.md) for details. authlib-injector is distributed under its own [AGPL-3.0 license](https://github.com/yushijinhun/authlib-injector/blob/develop/LICENSE).
