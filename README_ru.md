<div align="center">

<img width="160" height="160" align="center" src="./docs/logo.png" alt="Логотип OffPrism">

<h1>
<a href="https://github.com/offprism/launcher">OffPrism</a>
</h1>

Лаунчер Minecraft с **офлайн-входом, аккаунтами Ely.by и authlib-injector**,<br>
отдельными сборками, модами и модпаками с **Modrinth, CurseForge, FTB, Technic и ATLauncher** в один клик

Форк [Prism Launcher](https://prismlauncher.org/) — **не** одобрен его командой и **не** связан с ней

<p align="center">
<a href="./README.md">English</a> | <strong>Русский</strong>
</p>

<div>

[![Последний релиз](https://img.shields.io/github/v/release/offprism/launcher?label=Release&style=for-the-badge&color=8a8a8a)](https://github.com/offprism/launcher/releases/latest)
[![Загрузки](https://img.shields.io/github/downloads/offprism/launcher/total?label=Downloads&style=for-the-badge&color=8a8a8a)](https://github.com/offprism/launcher/releases)
[![Звёзды](https://img.shields.io/github/stars/offprism/launcher?label=Stars&style=for-the-badge&color=8a8a8a)](https://github.com/offprism/launcher/stargazers)
![Без рекламы](https://img.shields.io/badge/ads-none-8a8a8a?style=for-the-badge)

</div>

</div>

## Возможности

- **Игра офлайн** без аккаунта Microsoft — достаточно выбрать ник
- **Аккаунты Ely.by** — скин виден в лаунчере и **в игре** без модов (поддерживается двухфакторная защита)
- **Аккаунты authlib-injector** для любого совместимого сервера скинов — LittleSkin, Blessing Skin и других; [authlib-injector](https://github.com/yushijinhun/authlib-injector) уже встроен в лаунчер
- Аккаунты Microsoft тоже работают
- **Тема Glass** — тёмная полупрозрачная тема с размытием фона в Windows 11
- **Сборки (экземпляры)** — у каждой свои моды, ресурспаки, шейдеры, миры и настройки
- **Каталог модов** Modrinth и CurseForge с автоматической установкой зависимостей и **обновлением всех модов** в один клик
- **Модпаки из пяти каталогов** — Modrinth, CurseForge, FTB, Technic и ATLauncher
- **Обновление лаунчера в один клик** прямо из релизов этого репозитория
- Сборки для **Windows (x64 и ARM64), Linux и macOS** (Intel и Apple Silicon)

## Сравнение

| Возможность                                    | OffPrism          | Prism Launcher  |
|------------------------------------------------|-------------------|-----------------|
| Офлайн-режим без аккаунта Microsoft            | ✅                | ❌              |
| Аккаунты Ely.by со скином в игре               | ✅                | ❌              |
| Аккаунты authlib-injector (LittleSkin, ...)    | ✅                | ❌              |
| Аккаунты Microsoft                             | ✅                | ✅              |
| Полупрозрачная тема Glass                      | ✅                | ❌              |
| Отдельные сборки                               | ✅                | ✅              |
| Встроенный каталог Modrinth и CurseForge       | ✅                | ✅              |
| Модпаки FTB, Technic и ATLauncher              | ✅                | ✅              |
| Без рекламы и телеметрии                       | ✅                | ✅              |
| Основан на                                     | Prism Launcher    | PolyMC          |

## Установка

### Стабильные релизы

Скачайте OffPrism на странице [GitHub Releases](https://github.com/offprism/launcher/releases/latest):

| Система                         | Файл                                         |
|---------------------------------|----------------------------------------------|
| Windows                         | `OffPrism-Windows-MSVC-Setup-<версия>.exe`        |
| Windows (портативная)           | `OffPrism-Windows-MSVC-Portable-<версия>.zip`     |
| Windows на ARM                  | `OffPrism-Windows-MSVC-arm64-Setup-<версия>.exe`  |
| Windows на ARM (портативная)    | `OffPrism-Windows-MSVC-arm64-Portable-<версия>.zip` |
| Linux                           | `OffPrism-Linux-x86_64.AppImage`             |
| Linux (портативная)             | `OffPrism-Linux-Qt6-Portable-<версия>.tar.gz` |
| macOS (Intel и Apple Silicon)   | `OffPrism-macOS-<версия>.dmg`                |

После установки лаунчер обновляется сам из этого репозитория.

Сборки не подписаны: Windows SmartScreen может попросить подтверждение («Подробнее» → «Выполнить в любом случае»), а на macOS при первом запуске откройте приложение через правый клик → «Открыть».

### Тестовые сборки

Каждый коммит в `main` собирается в [GitHub Actions](https://github.com/offprism/launcher/actions), сборки прикреплены к каждому запуску как артефакты. Они не для обычных пользователей и могут быть сломаны.

## Сообщество и поддержка

Нашли ошибку или хотите новую функцию? Создайте issue в [GitHub Issues](https://github.com/offprism/launcher/issues). Pull request'ы приветствуются.

## Сборка из исходников

OffPrism собирается так же, как Prism Launcher: CMake, Qt 6.8+, компилятор C++20, JDK 17 и [vcpkg](https://vcpkg.io/) для остальных библиотек.

```bash
git clone --recursive https://github.com/offprism/launcher.git
cd launcher
cmake --preset windows_msvc   # или linux, macos_universal, windows_mingw
cmake --build --preset windows_msvc --config Release
```

Точные шаги для каждой платформы — в [`.github/workflows/build.yml`](.github/workflows/build.yml). Название релиза и версия задаются в корневом `CMakeLists.txt` (`Launcher_VERSION_*` и `Launcher_VERSION_CODENAME`).

## Авторы

- [offprism](https://github.com/offprism) — разработчик
- Участники [Prism Launcher](https://github.com/PrismLauncher/PrismLauncher), PolyMC и MultiMC — лаунчер, на котором основан этот форк

## Прочее

<ul>
  <li>Мы <strong>НЕ</strong> связаны с командой <a href="https://prismlauncher.org/">Prism Launcher</a>. Пожалуйста, не отправляйте им отчёты об ошибках этой сборки.</li>
  <li>Мы <strong>НЕ</strong> собираем ваши данные.</li>
  <li>Пароли аккаунтов Ely.by и authlib-injector никогда не сохраняются — хранится только токен сессии.</li>
  <li>Мы <strong>ОТКРЫТЫ</strong> для участия в разработке.</li>
</ul>

## Лицензия

[![License](https://img.shields.io/badge/license-GPL--3.0-8a8a8a?style=for-the-badge)](LICENSE)

OffPrism распространяется под лицензией GNU General Public License v3.0, унаследованной от Prism Launcher. Подробнее — в [LICENSE](LICENSE) и [COPYING.md](COPYING.md). authlib-injector распространяется под собственной [лицензией AGPL-3.0](https://github.com/yushijinhun/authlib-injector/blob/develop/LICENSE).
