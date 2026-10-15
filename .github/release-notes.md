## OffPrism Piano v1.0.3

### New

- **GitHub as a third source for mods, resource packs and shader packs.** The download dialogs now have a GitHub tab next to Modrinth and CurseForge.
  - Projects are found by their GitHub topics and sorted by stars.
  - Every suitable file of a release becomes a version; the mod loader (Fabric, Forge, NeoForge, Quilt) is taken from the file name, so a release with a jar per loader gives you the right one automatically. Server plugins and tools (Paper, Spigot, CLI…) are skipped.
  - Downloads are verified against the SHA-256 digest that GitHub publishes.

### Good to know

- Without signing in, GitHub allows about 60 requests per hour and 10 searches per minute.
- Some projects publish their files only on Modrinth or CurseForge — for them the GitHub tab shows "No valid version found".
- Mods from GitHub are plain files: the launcher does not track their updates or install their dependencies.

---

## OffPrism Piano v1.0.3 (RU)

- **GitHub — третий источник модов, ресурспаков и шейдеров.** В окнах загрузки появилась вкладка GitHub рядом с Modrinth и CurseForge.
  - Проекты ищутся по темам GitHub и сортируются по звёздам.
  - Каждый подходящий файл релиза становится версией; загрузчик (Fabric, Forge, NeoForge, Quilt) определяется по имени файла, поэтому из релиза с файлом под каждый загрузчик выбирается нужный. Серверные плагины и утилиты (Paper, Spigot, CLI…) пропускаются.
  - Скачанные файлы проверяются по SHA-256, который публикует GitHub.
- Без входа GitHub позволяет около 60 запросов в час и 10 поисков в минуту.
- Некоторые проекты выкладывают файлы только на Modrinth или CurseForge — для них на вкладке GitHub будет «No valid version found».
- Моды с GitHub — обычные файлы: лаунчер не следит за их обновлениями и не ставит зависимости.
