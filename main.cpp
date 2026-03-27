#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <iomanip>

// Pro Windows konzoli a diakritiku
#ifdef _WIN32
#include <windows.h>
#endif

// --- Třída reprezentující záznam o souboru v indexu ---
class FileRecord {
private:
    std::string name;
    int size;

public:
    FileRecord(std::string n, int s) : name(std::move(n)), size(s) {}

    std::string getName() const { return name; }

    void print() const {
            std::cout << "[INDEX] " << std::left << std::setw(20) << name
                  << " | Deklarovaná velikost: " << size << " KB" << std::endl;
    }

    void saveToDB(std::ofstream& dbFile) const {
        if (dbFile) {
            dbFile << name << " " << size << "\n";
        }
    }
};

// --- Hlavní správce logiky ---
class FileManager {
private:
    const std::string dbPath = "files.dat";

public:
    // Přidá záznam do indexu a vytvoří fyzický soubor
    void createAndIndex(const std::string& name, int size) {

        std::ofstream dbFile(dbPath, std::ios::app);
        FileRecord(name, size).saveToDB(dbFile);

        // Vytvoříme prázdný soubor, pokud neexistuje
        std::ofstream actualFile(name, std::ios::app);
        std::cout << ">>> Soubor '" << name << "' byl zaevidován do systému.\n";
    }

    // Vypíše všechny soubory z indexu
    void listAll() const {
        std::ifstream dbFile(dbPath);
        if (!dbFile) {
            std::cout << "Seznam je prázdný (soubor indexu neexistuje).\n";
            return;
        }

        std::cout << "\n--- SEZNAM SOUBORŮ V INDEXU ---\n";
        std::string name;
        int size;

        // nevim proc tady byl if a ne while
        while (dbFile >> name >> size) {
            FileRecord(name, size).print();
        }
        std::cout << "-------------------------------\n";
    }

    // Vyhledá konkrétní soubor v indexu
    void findInIndex(const std::string& searchName) const {
        std::ifstream dbFile(dbPath);
        std::string name;
        int size;
        bool found = false;

        while (dbFile >> name >> size) {
            if (name == searchName) {
                std::cout << "Nalezeno: ";
                FileRecord(name, size).print();
                found = true;
                break;
            }
        }
        if (!found) std::cout << "!!! Soubor '" << searchName << "' nebyl v indexu nalezen.\n";
    }

    bool findInIndex(const std::string& searchName, std::string isFound ) const {
        std::ifstream dbFile(dbPath);
        std::string name;
        int size;
        bool found = false;

        while (dbFile >> name >> size) {
            if (name == searchName) {
                FileRecord(name, size).print();
                found = true;
                break;
            }
        }
        return found;
    }


    // Přidá textový obsah do reálného souboru
    void appendContent(const std::string& fileName) {
        std::ifstream file(fileName);
        std::string text;
        std::cout << "Zadejte text (podpora diakritiky): ";
        std::cin.ignore(); // Odstraní zbylý newline z bufferu
        std::getline(std::cin, text);

        std::ofstream outFile(fileName, std::ios::app); // 2. chyba, spatny zapis
        outFile << text << "\n";
        std::cout << ">>> Text byl úspěšně připsán.\n";
    }

    // Přečte a vypíše obsah reálného souboru
    void readContent(const std::string& fileName) const {
        std::ifstream file(fileName);
        if (!file) {
            std::cout << "!!! Soubor '" << fileName << "' neexistuje nebo je prázdný.\n";
            return;
        }

        std::cout << "\n--- OBSAH SOUBORU: " << fileName << " ---\n";

        // 4. chyba
        std::string line;
        while (std::getline(file, line)) {
            std::cout << line << "\n";
        }
        std::cout << "----------------------------------\n";
    }

    // Vymaže databázi indexu
    void resetIndex() {
        std::ofstream dbFile(dbPath, std::ios::trunc);
        std::cout << ">>> Index souborů byl vymazán.\n";
    }
};

// --- Spuštění aplikace ---
int main() {
    // NASTAVENÍ UTF-8 PRO WINDOWS KONZOLI
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#endif

    FileManager manager;
    int volba = 0;

    while (volba != 6) {
        std::cout << "\n=== OBJEKTOVÝ SPRÁVCE SOUBORŮ ===\n";
        std::cout << "1. Registrovat nový soubor\n";
        std::cout << "2. Zobrazit seznam (index)\n";
        std::cout << "3. Vyhledat soubor v indexu\n";
        std::cout << "4. Připsat text do souboru\n";
        std::cout << "5. Přečíst obsah souboru\n";
        std::cout << "6. Vymazat index a ukončit\n";
        std::cout << "Vaše volba: ";

        if (!(std::cin >> volba)) {
            std::cout << "Neplatný vstup, končím.\n";
            break;
        }

        if (volba == 1) {
            std::string n; int s;
            std::cout << "Název souboru: "; std::cin >> n;
            std::cout << "Velikost (KB): "; std::cin >> s;
            // manager.createAndIndex(n, s);
            FileManager().createAndIndex(n, s);
        }
        else if (volba == 2) manager.listAll();
        else if (volba == 3) {
            std::string searchName;
            std::cout << "Hledaný název: ";
            std::cin.ignore(); // prvni  chyba, odstrani zbyly newline z bufferu
            std::getline(std::cin, searchName);
            manager.findInIndex(searchName);
        }
        else if (volba == 4) {
            std::string n;
            std::cout << "Do kterého souboru chcete psát: "; std::cin >> n;

            // 3. chyba check jestli soubor vubec existuje
            if(manager.findInIndex(n, n) == false) {
                std::cout << "Soubor '" << n << "' neexsituje \n";
            }else {
                manager.appendContent(n);
            }
        }
        else if (volba == 5) {
            std::string n;
            std::cout << "Který soubor chcete číst: "; std::cin >> n;
            manager.readContent(n);
        }
        else if (volba == 6) {
            manager.resetIndex();
        }
    }

    return 0;
}