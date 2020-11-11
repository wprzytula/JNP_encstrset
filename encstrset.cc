#include "encstrset.h"

#include <unordered_map>
#include <unordered_set>
#include <climits>
#include <cassert>
#include <iostream>
#include <iomanip>

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

    // Returns an unused ID number.
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
        assert(false); // The scarcely probable case that there already exist MAX_ULONG sets.
        return 0;
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
        std::cerr << "cypher \"" << std::setfill('0') << std::uppercase << std::hex;
        for (size_t i = 0; i < size; i++) {
            std::cerr << std::setw(2) << (int)encrypted[i];
            if (i + 1 < size) {
                std::cerr << ' ';
            }
        }
        std::cerr.copyfmt(std::ios(nullptr));  // Resets cerr flags to default.
        std::cerr << '"';
    }

    // Prints various amounts of arguments.
    void printArguments() {}
    void printArguments(unsigned long id) {
        std::cerr << id;
    }
    void printArguments(unsigned long id1, unsigned long id2) {
        std::cerr << id1 << ", " << id2;
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
}

// Macros for printing debug information.
#define printFunctionSelfInfo() \
    if (debug) { \
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
    if (debug) { \
        std::cerr << __func__ << '('; \
        printArguments(args); \
        std::cerr << ')' << std::endl; \
    }

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

#define printWasCopied(cipher, srcId, dstId) \
    if (debug) { \
        printEncrypted(cipher); \
        std::cerr << " copied from "; \
        printSet(srcId); \
        std::cerr << " to "; \
        printSet(dstId); \
        std::cerr << std::endl; \
    }

#define printCopiedAlreadyPresent(cipher, dstId) \
    if (debug) { \
        std::cerr << "copied "; \
        printEncrypted(cipher); \
        std::cerr << " was already present in "; \
        printSet(dstId); \
        std::cerr << std::endl; \
    }

namespace jnp1 {
    unsigned long encstrset_new() {
        printCallSpecs();
        printFunctionSelfInfo();
        unsigned long id = obtainID();
        sets()[id];
        printSetMessage(id, " created");
        return id;
    }

    void encstrset_clear(unsigned long id) {
        printCallSpecs(id);
        printFunctionSelfInfo();
        auto it = sets().find(id);
        if (it != sets().end()) {
            it->second.clear();
            printSetMessage(id, " cleared");
        } else printSetMessage(id, " does not exist");
    }

    void encstrset_delete(unsigned long id) {
        printCallSpecs(id);
        printFunctionSelfInfo();
        auto it = sets().find(id);
        if (it != sets().end()) {
            sets().erase(it);
            printSetMessage(id, " deleted");
        } else printSetMessage(id, " does not exist");
    }

    size_t encstrset_size(unsigned long id) {
        printCallSpecs(id);
        printFunctionSelfInfo();
        auto it = sets().find(id);
        if (it != sets().end()) {
            std::string message =
                    " contains " + std::to_string(it->second.size()) + " element(s)";
            printSetMessage(id, message);
            return it->second.size();
        } else {
            printSetMessage(id, " does not exist");
            return 0;
        }
    }

    bool encstrset_insert(unsigned long id, const char *value, const char *key) {
        printCallSpecs(id, value, key);
        printFunctionSelfInfo();
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
        printCallSpecs(id, value, key);
        printFunctionSelfInfo();
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
        printCallSpecs(id, value, key);
        printFunctionSelfInfo();
        initialValueCheck(value);
        auto it = sets().find(id);
        if (it != sets().end()) {
            std::string encrypted = encrypt(value, key);
            if (it->second.find(encrypted) != it->second.end()) {
                printSetCipherMessage(id, encrypted, " is present");
                return true;
            }
            printSetCipherMessage(id, encrypted, " is not present");
            return false;
        }
        printSetMessage(id, " does not exist");
        return false;
    }

    void encstrset_copy(unsigned long srcId, unsigned long dstId) {
        printCallSpecs(srcId, dstId);
        auto srcIt = sets().find(srcId), dstIt = sets().find(dstId);
        if (srcIt == sets().end()) {
            printFunctionSelfInfo();
            printSetMessage(srcId, " does not exist");
        }
        else if (dstIt == sets().end()) {
            printFunctionSelfInfo();
            printSetMessage(dstId, " does not exist");
        }
        else {
            for (auto it = srcIt->second.begin(); it != srcIt->second.end(); ++it) {
                printFunctionSelfInfo();
                if (dstIt->second.find(*it) == dstIt->second.end()) {
                    dstIt->second.insert(*it);
                    printWasCopied(*it, srcId, dstId);
                } else printCopiedAlreadyPresent(*it, dstId);
            }
        }
    }
}
