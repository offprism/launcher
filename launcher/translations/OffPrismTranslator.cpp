#include "OffPrismTranslator.h"

namespace {
struct Entry {
    const char* context;
    const char* source;
    const char* translation;
};

// clang-format off
const Entry s_russian[] = {
    // Accounts page
    { "AccountListPage", "Add &Ely.by", "Добавить &Ely.by" },
    { "AccountListPage", "Add a&uthlib-injector", "Добавить a&uthlib-injector" },
    { "AccountListPage", "Log in to any authlib-injector skin server (LittleSkin, Blessing Skin...)",
      "Войти на любой сервер скинов authlib-injector (LittleSkin, Blessing Skin...)" },
    { "AccountListPage",
      "Welcome!\nIf you're new here, you can select the \"Add Ely.by\" button to log in with Ely.by (with your skin), or \"Add Offline\" to play with any nickname.",
      "Добро пожаловать!\nЕсли вы здесь впервые, нажмите «Добавить Ely.by», чтобы войти через Ely.by (со своим скином), или «Добавить автономную», чтобы играть с любым ником." },
    { "AccountList", "Ely.by", "Ely.by" },
    { "AccountList", "authlib-injector", "authlib-injector" },

    // Login dialog
    { "ElyByLoginDialog", "Add authlib-injector account", "Добавить аккаунт authlib-injector" },
    { "ElyByLoginDialog",
      "Log in to any <a href=\"https://github.com/yushijinhun/authlib-injector\">authlib-injector</a> compatible skin server (for example LittleSkin or a Blessing Skin server). The skin from that server will be shown in the launcher and in game.",
      "Войдите на любой сервер скинов, совместимый с <a href=\"https://github.com/yushijinhun/authlib-injector\">authlib-injector</a> (например, LittleSkin или сервер Blessing Skin). Скин с этого сервера будет виден в лаунчере и в игре." },
    { "ElyByLoginDialog", "Server:", "Сервер:" },
    { "ElyByLoginDialog", "Add Ely.by account", "Добавить аккаунт Ely.by" },
    { "ElyByLoginDialog",
      "Log in with your <a href=\"https://ely.by\">Ely.by</a> account. Your skin from Ely.by will be shown in the launcher and in game.",
      "Войдите с аккаунтом <a href=\"https://ely.by\">Ely.by</a>. Ваш скин с Ely.by будет виден в лаунчере и в игре." },
    { "ElyByLoginDialog", "Nickname or e-mail", "Ник или e-mail" },
    { "ElyByLoginDialog", "E-mail or nickname", "E-mail или ник" },
    { "ElyByLoginDialog", "Login:", "Логин:" },
    { "ElyByLoginDialog", "Password:", "Пароль:" },
    { "ElyByLoginDialog", "Only if two-factor authentication is enabled", "Только если включена двухфакторная защита" },
    { "ElyByLoginDialog", "2FA code:", "Код 2FA:" },
    { "ElyByLoginDialog", "No account yet? <a href=\"https://account.ely.by/register\">Register on Ely.by</a>",
      "Ещё нет аккаунта? <a href=\"https://account.ely.by/register\">Зарегистрируйтесь на Ely.by</a>" },
    { "ElyByLoginDialog", "Log in", "Войти" },
    { "ElyByLoginDialog", "Please enter your login and password.", "Введите логин и пароль." },
    { "ElyByLoginDialog", "Please enter the address of the authlib-injector server.", "Введите адрес сервера authlib-injector." },
    { "ElyByLoginDialog", "Contacting the server...", "Подключение к серверу..." },
    { "ElyByLoginDialog", "Could not reach the server: %1", "Не удалось связаться с сервером: %1" },
    { "ElyByLoginDialog", "This address is not an authlib-injector server.", "По этому адресу нет сервера authlib-injector." },
    { "ElyByLoginDialog", "Logging in...", "Вход..." },

    // Authentication steps
    { "YggdrasilAuthStep", "Refreshing %1 session", "Обновление сессии %1" },
    { "YggdrasilAuthStep", "Logging in to %1", "Вход в %1" },
    { "YggdrasilAuthStep", "No session is stored. Please log in again.", "Сессия не сохранена. Войдите заново." },
    { "YggdrasilAuthStep",
      "This account is protected with two-factor authentication. Enter the code from your authenticator app.",
      "Аккаунт защищён двухфакторной аутентификацией. Введите код из приложения-аутентификатора." },
    { "YggdrasilAuthStep", "%1 returned an unexpected response.", "%1 вернул неожиданный ответ." },
    { "YggdrasilAuthStep", "This account has no Minecraft profile (character) on %1.", "У этого аккаунта нет профиля (персонажа) Minecraft на %1." },
    { "YggdrasilAuthStep", "Logged in to %1", "Вход в %1 выполнен" },
    { "AuthlibProfileStep", "Fetching profile skin.", "Загрузка скина профиля." },
    { "AuthlibProfileStep", "Got profile", "Профиль получен" },

    // Launch
    { "LaunchController",
      "In order to play Minecraft, you must add at least one account (Ely.by, Offline or Microsoft). Would you like to open the account manager to add an account now?",
      "Чтобы играть в Minecraft, добавьте хотя бы одну учётную запись (Ely.by, автономную или Microsoft). Открыть управление учётными записями сейчас?" },
    { "LauncherPartLaunch", "authlib-injector.jar could not be found, skins of the Ely.by account will not work in game.",
      "Не найден authlib-injector.jar — скины аккаунта Ely.by в игре работать не будут." },

    // Standard Qt buttons (the Qt translation files are not shipped with the launcher)
    { "QPlatformTheme", "Cancel", "Отмена" },
    { "QPlatformTheme", "Close", "Закрыть" },
    { "QPlatformTheme", "Apply", "Применить" },
    { "QPlatformTheme", "Reset", "Сбросить" },
    { "QPlatformTheme", "Help", "Справка" },
    { "QPlatformTheme", "Save", "Сохранить" },
    { "QPlatformTheme", "Discard", "Не сохранять" },
    { "QPlatformTheme", "&Yes", "&Да" },
    { "QPlatformTheme", "Yes to &All", "Да для &всех" },
    { "QPlatformTheme", "&No", "&Нет" },
    { "QPlatformTheme", "N&o to All", "Н&ет для всех" },
    { "QPlatformTheme", "Abort", "Прервать" },
    { "QPlatformTheme", "Retry", "Повторить" },
    { "QPlatformTheme", "Ignore", "Пропустить" },
    { "QPlatformTheme", "Restore Defaults", "По умолчанию" },
    { "QShortcut", "Ctrl", "Ctrl" },

    // Themes
    { "QObject", "Glass (translucent)", "Glass (полупрозрачная)" },
    { "QObject", "Dark theme with semi-transparent windows.", "Тёмная тема с полупрозрачными окнами." },
};
// clang-format on

QString key(const char* context, const char* source)
{
    return QString::fromUtf8(context) + QChar(0x1f) + QString::fromUtf8(source);
}
}  // namespace

OffPrismTranslator::OffPrismTranslator(const QString& langCode)
{
    if (!langCode.startsWith("ru"))
        return;
    for (const auto& entry : s_russian) {
        m_strings.insert(key(entry.context, entry.source), QString::fromUtf8(entry.translation));
    }
}

QString OffPrismTranslator::translate(const char* context, const char* sourceText, [[maybe_unused]] const char* disambiguation,
                                      [[maybe_unused]] int n) const
{
    if (!context || !sourceText)
        return {};
    return m_strings.value(key(context, sourceText));
}
