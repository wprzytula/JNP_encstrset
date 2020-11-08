#include "encstrset.h"

#include <unordered_map>
#include <unordered_set>
#include <climits>
#include <cassert>

#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCUnusedGlobalDeclarationInspection"
#ifdef NDEBUG
    const bool debug = false;
#else
    const bool debug = true;
#endif

namespace {
    using encstrset = std::unordered_set<std::string>;
    std::unordered_map<unsigned long, encstrset> sets;

    // TODO: Podaje nastêpny numer id lub, je¿eli skoñczy siê zakres, szuka wolnego numeru.
    unsigned long obtainID() {
        static unsigned long lastGiven = -1;
        unsigned long proposedID = lastGiven + 1;
        while (proposedID != lastGiven) {
            if (sets.find(proposedID) == sets.end()) {
                lastGiven = proposedID;
                return proposedID;
            }
            ++proposedID;
        }
        assert(false); // the scarcely probable case that there already exist MAX_ULONG maps
    }

    // Szyfruje value kluczem (niepustym) key.
    std::string encrypt(const char *value, const char *key) {
        assert(value != nullptr);
        std::string result;
        int posVal = 0;
        char currVal = value[posVal];
        if (key == nullptr) {
            while (currVal != '\0') {
                result += currVal;
                currVal = value[++posVal];
            }
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

    // Wypisuje encrypted w postaci "cypher "HEX HEX..."".
    void printEncrypted(std::string encrypted) {
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

    // Wypisuje id w postaci "set #id".
    void printSet(unsigned long id) {
        std::cerr << "set #" << id;
    }

    void printDoesNotExist(unsigned long id) {
        printSet(id);
        std::cerr << " does not exist" << std::endl;
    }

    void printArguments(const char *) {}
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
    if (debug) {\
        printCallSpecs(args);\
        std::cerr << __func__ << ": ";\
    }

#define initialValueCheck(value) \
    if (value == nullptr) {\
        if (debug) {\
            std::cerr << "invalid value (NULL)" <<\
            std::endl;\
        }\
    return false;\
    }

#define printCallSpecs(args...) std::cerr << __func__ << '(';\
                                printArguments(args);\
                                std::cerr << ')' << std::endl;

extern "C" {
    unsigned long encstrset_new() {
        if (debug) {
            printCallSpecs("");
            std::cerr << __func__ << ": ";
        }
        unsigned long id = obtainID();
        sets[id];
        if (debug) {
            printSet(id);
            std::cerr << " created" << std::endl;
        }
        return id;
    }

    void encstrset_clear(unsigned long id) {
        printFunctionSelfInfo(id);
        auto it = sets.find(id);
        if (it != sets.end()) {
            it->second.clear();
            if (debug) {
                printSet(id);
                std::cerr << " cleared" << std::endl;
            }
        } else if (debug) {
            printDoesNotExist(id);
        }
    }

    void encstrset_delete(unsigned long id) {
        printFunctionSelfInfo(id);
        auto it = sets.find(id);
        if (it != sets.end()) {
            sets.erase(it);
            if (debug) {
                printSet(id);
                std::cerr << " deleted" << std::endl;
            }
        } else if (debug) {
            printDoesNotExist(id);
        }
    }

    size_t encstrset_size(unsigned long id) {
        printFunctionSelfInfo(id);
        auto it = sets.find(id);
        if (it != sets.end()) {
            if (debug) {
                printSet(id);
                std::cerr << " contains " << it->second.size() << " element(s)"
                          << std::endl;
            }
            return it->second.size();
        } else {
            if (debug) {
                printDoesNotExist(id);

                return 0;
            }
        }
    }

    bool encstrset_insert(unsigned long id, const char *value, const char *key) {
        printFunctionSelfInfo(id, value, key);
        initialValueCheck(value);
        auto it = sets.find(id);
        if (it != sets.end()) {
            std::string encrypted = encrypt(value, key);
            if (it->second.find(encrypted) == it->second.end()) {
                it->second.insert(encrypted);
                if (debug) {
                    printSet(id);
                    std::cerr << ", ";
                    printEncrypted(encrypted);
                    std::cerr << " inserted" << std::endl;
                }
                return true;
            }
        }
        printDoesNotExist(id);
        return false;
    }

    bool encstrset_remove(unsigned long id, const char *value, const char *key) {
        printFunctionSelfInfo(id, value, key);
        initialValueCheck(value);
        auto it = sets.find(id);
        if (it != sets.end()) {
            std::string encrypted = encrypt(value, key);
            if (it->second.find(encrypted) != it->second.end()) {
                it->second.erase(encrypted);
                if (debug) {
                    printSet(id);
                    std::cerr << ", ";
                    printEncrypted(encrypted);
                    std::cerr << " removed" << std::endl;
                }
                return true;
            }
        }
        printDoesNotExist(id);
        return false;
    }

    bool encstrset_test(unsigned long id, const char *value, const char *key) {
        printFunctionSelfInfo(id, value, key);
        initialValueCheck(value);
        auto it = sets.find(id);
        if (it != sets.end()) {
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
        printDoesNotExist(id);
        return false;
    }

    void encstrset_copy(unsigned long srcId, unsigned long dstId) {
        printCallSpecs(srcId, dstId);
        auto srcIt = sets.find(srcId), dstIt = sets.find(dstId);
        if (srcIt == sets.end()) {
            std::cerr << __func__ << ": ";
            printDoesNotExist(srcId);
        }
        else if (dstIt == sets.end()) {
            std::cerr << __func__ << ": ";
            printDoesNotExist(dstId);
        }
        else {
            for (auto it = srcIt->second.begin(); it != srcIt->second.end(); ++it) {
                if (debug) {
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

#pragma clang diagnostic pop