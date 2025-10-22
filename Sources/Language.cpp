#include "Language.hpp"
#include <CTRPluginFramework.hpp>

namespace CTRPluginFramework {
    Language* Language::instance = nullptr;

    Language* Language::getInstance() {
        if (!instance)
            instance = new Language();
        return instance;
    }

    std::vector<Language::LangHeader> Language::listAvailableLanguages(const std::string &filePath) {
        std::vector<LangHeader> langs;
        File f;

        if (File::Open(f, filePath, File::READ) != File::SUCCESS) {
            f.Close();
            return langs;
        }

        u32 langCount = 0;
        if (f.Read(&langCount, sizeof(langCount)) != File::SUCCESS) {
            f.Close();
            return langs;
        }

        langs.reserve(langCount);

        for (u32 i = 0; i < langCount; ++i) {
            u8 shortLen = 0, fullLen = 0;
            f.Read(&shortLen, sizeof(shortLen));

            std::string shortName(shortLen, '\0');
            f.Read(&shortName[0], shortLen);

            f.Read(&fullLen, sizeof(fullLen));
            std::string fullName(fullLen, '\0');
            f.Read(&fullName[0], fullLen);

            u32 offset = 0, size = 0;
            f.Read(&offset, sizeof(offset));
            f.Read(&size, sizeof(size));

            langs.push_back({shortName, fullName, offset, size});
        }

        f.Close();
        return langs;
    }

    bool Language::loadFromBinary(const std::string &file, const std::string &lang) {
        filePath = file;
        currentLang = lang;

        File f;
        if (File::Open(f, file, File::READ) != File::SUCCESS) {
            f.Close();
            return false;
        }

        // Header [uint32 langCount]
        u32 langCount = 0;
        if (f.Read(&langCount, sizeof(langCount)) != File::SUCCESS) {
            f.Close();
            return false;
        }

        std::vector<LangHeader> headers;
        headers.reserve(langCount);

        for (u32 i = 0; i < langCount; ++i) {
            u8 shortLen = 0, fullLen = 0;
            f.Read(&shortLen, sizeof(shortLen));

            std::string shortName(shortLen, '\0');
            f.Read(&shortName[0], shortLen);

            f.Read(&fullLen, sizeof(fullLen));
            std::string fullName(fullLen, '\0');
            f.Read(&fullName[0], fullLen);

            u32 offset = 0, size = 0;
            f.Read(&offset, sizeof(offset));
            f.Read(&size, sizeof(size));

            headers.push_back({shortName, fullName, offset, size});
        }

        // Search target language
        const LangHeader* target = nullptr;
        for (auto &h : headers) {
            if (h.shortName == lang) {
                target = &h;
                break;
            }
        }

        if (!target) {
            f.Close();
            // Language not found
            return false;
        }

        // Read language block
        f.Seek(target->offset, File::SET);
        std::vector<u8> buffer(target->size);
        if (f.Read(buffer.data(), target->size) != File::SUCCESS) {
            f.Close();
            return false;
        }
        f.Close();

        const u8* ptr = buffer.data();
        const u8* end = ptr + buffer.size();

        if (ptr + sizeof(u32) > end)
            return false;

        u32 entryCount = *reinterpret_cast<const u32*>(ptr);
        ptr += sizeof(u32);

        translations.clear();
        translations.reserve(entryCount);

        for (u32 j = 0; j < entryCount && ptr < end; ++j) {
            if (ptr + sizeof(u16) > end)
                break;
            u16 keyLen = *reinterpret_cast<const u16*>(ptr);
            ptr += sizeof(u16);

            if (ptr + keyLen > end)
                break;
            std::string key(reinterpret_cast<const char*>(ptr), keyLen);
            ptr += keyLen;

            if (ptr + sizeof(u16) > end)
                break;
            u16 valLen = *reinterpret_cast<const u16*>(ptr);
            ptr += sizeof(u16);

            if (ptr + valLen > end)
                break;
            std::string val(reinterpret_cast<const char*>(ptr), valLen);
            ptr += valLen;

            translations.push_back({key, val});
        }

        loaded = true;
        return true;
    }

    std::string Language::get(const std::string &key) const {
        if (!loaded)
            return "[not loaded]";

        for (auto &pair : translations) {
            if (pair.key == key)
                return pair.value;
        }

        return "[" + key + "]";
    }
}
