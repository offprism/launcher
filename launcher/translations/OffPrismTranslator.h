#pragma once

#include <QHash>
#include <QTranslator>

// OffPrism: built-in translations for strings added by OffPrism. These strings are not part of the
// Prism Launcher translation files that are downloaded at runtime, so they are shipped with the launcher.
class OffPrismTranslator : public QTranslator {
   public:
    explicit OffPrismTranslator(const QString& langCode);

    QString translate(const char* context, const char* sourceText, const char* disambiguation, int n) const override;
    bool isEmpty() const override { return m_strings.isEmpty(); }

   private:
    QHash<QString, QString> m_strings;
};
