#include "encstrset.h"

#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <climits>
// wszystkie inne include w .h? albo nie?

extern "C" {
namespace {
    using encstrset = std::unordered_set<std::string>;  // zmieniam zaszyfrowane wartoœci z const char* na string bo chyba ³atwiej
    std::unordered_map<unsigned long, encstrset> sets;
    unsigned long lastID = 0;

    // Podaje nastêpny numer id lub, je¿eli skoñczy siê zakres, szuka wolnego numeru.
    unsigned long obtainID() {
        if (lastID == ULONG_MAX) {
            // szukamy wolnego, co jak nie znajdziemy?
            return lastID;
        } else {
            return lastID++;
        }
    }

    // Szyfruje value kluczem (niepustym) key.
    std::string encrypt(const char *value, const char *key) {
        std::string ret = "";
        int pos_val = 0, pos_key = 0;
        char current_val = value[pos_val], current_key = key[pos_key];
        while (current_val != '\0') {
            char encrypted = current_val ^current_key;
            ret += encrypted;
            current_val = value[++pos_val];
            current_key = key[++pos_key];
            if (current_key == '\0') {
                pos_key = 0;
                current_key = key[pos_key];
            }
        }
        return ret;
    }

/* działa dobrze, wykomentowane bo jest warning że (jeszcze) nie użyte
    // Wypisuje encrypted w postaci "cypher "HEX HEX..."".
    void prettyPrintEncrypted(std::string encrypted)
    {
        size_t size = encrypted.size();
        std::cout << "cypher";
        for (size_t i = 0; i < size; i++)
        {
            std::cout << " " << std::uppercase << std::hex << (int)encrypted[i];
        }
    }

    // Wypisuje id w postaci "set #id".
    void prettyPrintSet(unsigned long id)
    {
        std::cout << "set #" << id;
    }*/
}

// komentarze do tych funkcji tylko w .h?
// sporo kodu siê w tych funkcjach powtarza ale nie wiem czy mo¿emy coœ z tym zrobiæ

unsigned long encstrset_new() {
    unsigned long id = obtainID();
    encstrset ess;
    sets.emplace(id, ess); // emplace czy insert czy jeszcze coœ innego?
    return id;
}

void encstrset_clear(unsigned long id) {
    auto it = sets.find(id);
    if (it != sets.end()) {
        it->second.clear();
    }
}

void encstrset_delete(unsigned long id) {
    auto it = sets.find(id);
    if (it != sets.end()) {
        encstrset_clear(id);
        sets.erase(it);
    }
}

size_t encstrset_size(unsigned long id) {
    auto it = sets.find(id);
    return it == sets.end() ? 0 : it->second.size();
}

bool encstrset_insert(unsigned long id, const char *value, const char *key) {
    auto it = sets.find(id);
    if (it != sets.end()) {
        std::string encrypted = encrypt(value, key);
        if (it->second.find(encrypted) == it->second.end()) {
            it->second.insert(encrypted);
            return true;
        }
    }
    return false;
}

bool encstrset_remove(unsigned long id, const char *value, const char *key) {
    auto it = sets.find(id);
    if (it != sets.end()) {
        std::string encrypted = encrypt(value, key);
        if (it->second.find(encrypted) != it->second.end()) {
            it->second.erase(encrypted);
            return true;
        }
    }
    return false;
}

bool encstrset_test(unsigned long id, const char *value, const char *key) {
    auto it = sets.find(id);
    if (it != sets.end()) {
        std::string encrypted = encrypt(value, key);
        if (it->second.find(encrypted) != it->second.end()) {
            return true;
        }
    }
    return false;
}

void encstrset_copy(unsigned long src_id, unsigned long dst_id) {
    auto src_it = sets.find(src_id), dst_it = sets.find(dst_id);
    if (src_it != sets.end() && dst_it != sets.end()) {
        for (auto it = src_it->second.begin();
             it != src_it->second.end(); it++) {
            dst_it->second.insert(*it);
        }
    }
}
}
