#ifndef LANGUAGE_HPP
#define LANGUAGE_HPP

#include <string>
#include <vector>
#include <CTRPluginFramework.hpp>

namespace CTRPluginFramework {
    class Language {
    private:
        struct Entry {
            std::string key;
            std::string value;
        };

        struct LangHeader {
            std::string shortName;
            std::string fullName;
            u32 offset;
            u32 size;

            bool operator==(const std::string& other) const {
                return shortName == other;
            }
        };

        std::vector<Entry> translations;
        std::string currentLang;
        std::string filePath;
        bool loaded = false;
        static Language *instance;

    public:
        static Language* getInstance();
        static std::vector<LangHeader> listAvailableLanguages(const std::string &filePath);
        bool loadFromBinary(const std::string& filePath, const std::string& lang);
        std::string get(const std::string& key) const;
        const std::string& getCurrentLang() const { return currentLang; }
    };
}
#endif