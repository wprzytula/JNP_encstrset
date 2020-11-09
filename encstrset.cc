#include "encstrset.h"

#include <unordered_map>
#include <unordered_set>
#include <climits>
#include <cassert>

#ifdef NDEBUG
    const bool debug = false;
#else
    const bool debug = true;
#endif

namespace {
    using encstrset = std::unordered_set<std::string>;

    std::unordered_map<unsigned long, encstrset>& sets() {
        static std::unordered_map<unsigned long, encstrset> sets;
        return sets;
    }

    unsigned long obtainID() {
        static unsigned long lastGiven = -1;
        unsigned long proposedID = lastGiven + 1;
        while (proposedID != lastGiven) {
            if (sets().find(proposedID) == sets().end()) {
                lastGiven = proposedID;
                return proposedID;
            }
            ++proposedID;
        }
        assert(false); // the scarcely probable case that there already exist MAX_ULONG sets
    }

    //  Returns string containing <value> encrypted using <key>
    //  if <key> is not NULL and not empty, else <value>.
    std::string encrypt(const char *value, const char *key) {
        if (debug) {
            assert(value != nullptr);
        }
        std::string result;
        int posVal = 0;
        char currVal = value[posVal];
        if (key == nullptr || *key == '\0') {
            return value;
        }
        int posKey = 0;
        char currKey = key[posKey];
        while (currVal != '\0') {
            char encrypted = currVal ^ currKey;
            result += encrypted;
            currVal = value[++posVal];
            currKey = key[++posKey];
            if (currKey == '\0') {
                posKey = 0;
                currKey = key[posKey];
            }
        }
        return result;
    }


    void printSet(unsigned long id) {
        std::cerr << "set #" << id;
    }

    // Prints <encrypted> in form "cypher "HEX HEX..."".
    void printEncrypted(const std::string& encrypted) {
        size_t size = encrypted.size();
        std::cerr << "cypher \"";
        for (size_t i = 0; i < size; i++) {
            std::cerr << std::uppercase << std::hex << (int)encrypted[i];
            if (i + 1 < size) {
                std::cerr << ' ';
            }
        }
        std::cerr << '"';
    }

    void printArguments() {}
    void printArguments(unsigned long id) {
        std::cerr << id;
    }
    void printArguments(unsigned long id, const char *value, const char *key) {
        std::cerr << id << ", ";
        if (value == nullptr) {
            std::cerr << "NULL";
        }
        else {
            std::cerr << '"' << value << '"';
        }
        std::cerr << ", ";
        if (key == nullptr) {
            std::cerr << "NULL";
        }
        else {
            std::cerr << '"' << key << '"';
        }
    }

    void printArguments(unsigned long id1, unsigned long id2) {
        std::cerr << id1 << ", " << id2;
    }
}

#define printFunctionSelfInfo(args...) \
    if (debug) { \
        printCallSpecs(args); \
        std::cerr << __func__ << ": "; \
    }

#define initialValueCheck(value) \
    if (value == nullptr) { \
        if (debug) { \
            std::cerr << "invalid value (NULL)" << \
            std::endl; \
        } \
        return false; \
    }

#define printCallSpecs(args...) \
    std::cerr << __func__ << '('; \
    printArguments(args); \
    std::cerr << ')' << std::endl;

#define printSetMessage(id, message) \
    if (debug) { \
       printSet(id); \
       std::cerr << message << std::endl; \
    }

#define printSetCipherMessage(id, cipher, message) \
    if (debug) { \
        printSet(id); \
        std::cerr << ", "; \
        printEncrypted(encrypted); \
        std::cerr << message << std::endl; \
    }


namespace jnp1 {
    unsigned long encstrset_new() {
        printFunctionSelfInfo();
        unsigned long id = obtainID();
        sets()[id];
        printSetMessage(id, " created");
        return id;
    }

    void encstrset_clear(unsigned long id) {
        printFunctionSelfInfo(id);
        auto it = sets().find(id);
        if (it != sets().end()) {
            it->second.clear();
            printSetMessage(id, " cleared");
        } else printSetMessage(id, " does not exist");
    }

    void encstrset_delete(unsigned long id) {
        printFunctionSelfInfo(id);
        auto it = sets().find(id);
        if (it != sets().end()) {
            sets().erase(it);
            printSetMessage(id, " deleted");
        } printSetMessage(id, " does not exist");
    }

    size_t encstrset_size(unsigned long id) {
        printFunctionSelfInfo(id);
        auto it = sets().find(id);
        if (it != sets().end()) {
            if (debug) {
                printSet(id);
                std::cerr << " contains " << it->second.size() << " element(s)"
                          << std::endl;
            }
            return it->second.size();
        } else {
            printSetMessage(id, " does not exist");
            return 0;
        }
    }

    bool encstrset_insert(unsigned long id, const char *value, const char *key) {
        printFunctionSelfInfo(id, value, key);
        initialValueCheck(value);
        auto it = sets().find(id);
        if (it != sets().end()) {
            std::string encrypted = encrypt(value, key);
            if (it->second.find(encrypted) == it->second.end()) {
                it->second.insert(encrypted);
                printSetCipherMessage(id, encrypted, " inserted");
                return true;
            }
            printSetCipherMessage(id, encrypted, " was already present");
            return false;
        }
        printSetMessage(id, " does not exist");
        return false;
    }

    bool encstrset_remove(unsigned long id, const char *value, const char *key) {
        printFunctionSelfInfo(id, value, key);
        initialValueCheck(value);
        auto it = sets().find(id);
        if (it != sets().end()) {
            std::string encrypted = encrypt(value, key);
            if (it->second.find(encrypted) != it->second.end()) {
                it->second.erase(encrypted);
                printSetCipherMessage(id, encrypted, " removed");
                return true;
            }
            printSetCipherMessage(id, encrypted, " was not present");
            return false;
        }
        printSetMessage(id, " does not exist");
        return false;
    }

    bool encstrset_test(unsigned long id, const char *value, const char *key) {
        printFunctionSelfInfo(id, value, key);
        initialValueCheck(value);
        auto it = sets().find(id);
        if (it != sets().end()) {
            std::string encrypted = encrypt(value, key);
            if (debug) {
                printSet(id);
                std::cerr << ", ";
                printEncrypted(encrypted);
            }
            if (it->second.find(encrypted) != it->second.end()) {
                if (debug) {
                    std::cerr << " is present" << std::endl;
                }
                return true;
            }
            if (debug) {
                std::cerr << " is not present" << std::endl;
            }
            return false;
        }
        printSetMessage(id, " does not exist");
        return false;
    }

    void encstrset_copy(unsigned long srcId, unsigned long dstId) {
        printFunctionSelfInfo()
        auto srcIt = sets().find(srcId), dstIt = sets().find(dstId);
        if (srcIt == sets().end()) {
            printSetMessage(srcId, " does not exist");
        }
        else if (dstIt == sets().end()) {
            printSetMessage(dstId, " does not exist");
        }
        else {
            for (auto it = srcIt->second.begin(); it != srcIt->second.end(); ++it) {
                if (debug && it != srcIt->second.begin()) {
                    std::cerr << __func__ << ": ";
                }
                if (dstIt->second.find(*it) == dstIt->second.end()) {
                    dstIt->second.insert(*it);
                    if (debug) {
                        printEncrypted(*it);
                        std::cerr << " copied from ";
                        printSet(srcId);
                        std::cerr << " to ";
                        printSet(dstId);
                        std::cerr << std::endl;
                    }
                } else if (debug) {
                    std::cerr << "copied ";
                    printEncrypted(*it);
                    std::cerr << " was already present in ";
                    printSet(dstId);
                    std::cerr << std::endl;
                }
            }
        }
    }
}
