#include <SFML/Graphics.hpp>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <regex>
#include <unordered_map>
#include <string>
#include <unordered_set>

using namespace std;

const std::unordered_set<std::string> c_keywords = {
    "auto", "break", "case", "char", "const", "continue", "default",
    "do", "double", "else", "enum", "extern", "float", "for", "goto",
    "if", "inline", "int", "long", "register", "restrict", "return",
    "short", "signed", "sizeof", "static", "struct", "switch", "typedef",
    "union", "unsigned", "void", "volatile", "while", "_Alignas", "_Alignof",
    "_Atomic", "_Bool", "_Complex", "_Generic", "_Imaginary", "_Noreturn",
    "_Static_assert", "_Thread_local"
};

struct Variable {
    std::string name;
    int value;
    bool isPointer = false;
    std::string pointsTo;
};


// ------------------------------
// ANÁLISIS LÉXICO
// ------------------------------
std::vector<std::string> lexer(const std::string& code) {
    std::vector<std::string> tokens;
    std::regex tokenRegex(R"([\w_]+|\\|==|!=|<=|>=|&&|\|\||[{}();=*&,+\-\/<>])");
    auto begin = std::sregex_iterator(code.begin(), code.end(), tokenRegex);
    auto end = std::sregex_iterator();

    for (auto it = begin; it != end; ++it) {
        tokens.push_back(it->str());
    }

    return tokens;
}

// ------------------------------
// ANÁLISIS SINTÁCTICO BÁSICO
// ------------------------------

std::vector<Variable> parser(const std::vector<std::string>& tokens) {
    std::vector<Variable> vars;
    size_t i = 0;

    while (i < tokens.size()) {
        if (tokens[i] == "int") {
            bool isPtr = false;

            if (tokens[i+1] == "*") {
                isPtr = true;
                ++i;
            }

            std::string varName = tokens[i+1];
            i += 2;

            if (i < tokens.size() && tokens[i] == "=") {
                if (i+1 < tokens.size()) {
                    if (tokens[i+1] == "&") {
                        Variable v;
                        v.name = varName;
                        v.isPointer = true;
                        v.pointsTo = tokens[i+2];
                        v.value = 0;
                        vars.push_back(v);
                        i += 3; // = &nombre ;
                    } else {
                        try {
                            int value = std::stoi(tokens[i+1]);
                            Variable v;
                            v.name = varName;
                            v.value = value;
                            v.isPointer = false;
                            vars.push_back(v);
                            i += 2; // = valor ;
                        } catch (...) {
                            // No es un valor válido
                            i++;
                        }
                    }
                } else {
                    i++;
                }
            } else {
                // Solo declaración sin asignación: int x;
                Variable v;
                v.name = varName;
                v.value = 0;
                v.isPointer = isPtr;
                vars.push_back(v);
            }
        } else {
            ++i;
        }
    }

    return vars;
}

void drawMemory(sf::RenderWindow& window, const std::vector<Variable>& vars, const sf::Font& font) {
    window.clear(sf::Color::White);

    sf::RectangleShape heapRect(sf::Vector2f(1200, 100));
    heapRect.setPosition(50, 50);
    heapRect.setFillColor(sf::Color(180, 180, 180));
    heapRect.setOutlineThickness(2);
    heapRect.setOutlineColor(sf::Color::Black);
    window.draw(heapRect);

    sf::Text heapLabel("Heap", font, 24);
    heapLabel.setFillColor(sf::Color::Black);
    heapLabel.setPosition(50, 20);
    window.draw(heapLabel);

    sf::RectangleShape stackRect(sf::Vector2f(1200, 100));
    stackRect.setPosition(50, 200);
    stackRect.setFillColor(sf::Color(200, 200, 200));
    stackRect.setOutlineThickness(2);
    stackRect.setOutlineColor(sf::Color::Black);
    window.draw(stackRect);

    sf::Text stackLabel("Stack", font, 24);
    stackLabel.setFillColor(sf::Color::Black);
    stackLabel.setPosition(50, 170);
    window.draw(stackLabel);

    std::unordered_map<std::string, uintptr_t> addressMap;
    for (const auto& v : vars) {
        addressMap[v.name] = reinterpret_cast<uintptr_t>(&v);
    }

    int xStart = 70;
    int yVar = 220;
    int boxSize = 40;
    int margin = 20;

    for (const auto& v : vars) {
        int width = v.isPointer ? 2 * boxSize : boxSize;
        sf::RectangleShape rect(sf::Vector2f(width, boxSize));
        rect.setPosition(xStart, yVar);
        rect.setFillColor(v.isPointer ? sf::Color(255, 255, 150) : sf::Color(150, 255, 150));
        rect.setOutlineColor(sf::Color::Black);
        rect.setOutlineThickness(2);
        window.draw(rect);

        sf::Text valText("", font, 20);
        valText.setFillColor(sf::Color::Black);

        if (v.isPointer) {
            std::stringstream ss;
            ss << "0x" << std::hex << (addressMap[v.pointsTo] & 0xFFFF);
            valText.setString(ss.str());
        } else {
            valText.setString(std::to_string(v.value));
        }
        sf::FloatRect bounds = valText.getLocalBounds();
        valText.setOrigin(bounds.width / 2, bounds.height / 2);
        valText.setPosition(xStart + width / 2, yVar + boxSize / 2 - 5);
        window.draw(valText);

        sf::Text nameText(v.name, font, 18);
        nameText.setFillColor(sf::Color::Black);
        sf::FloatRect nameBounds = nameText.getLocalBounds();
        nameText.setOrigin(nameBounds.width / 2, 0);
        nameText.setPosition(xStart + width / 2, yVar + boxSize + 5);
        window.draw(nameText);

        xStart += width + margin;
    }

    window.display();
}

int main() {
    cout << "hola" << endl;
    std::ifstream file("../entrada.txt");
    if (!file.is_open()) {
        std::cerr << "No se pudo abrir el archivo de entrada.\n";
        return 1;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string code = buffer.str();

    std::vector<std::string> tokens = lexer(code);
    std::vector<Variable> variables = parser(tokens);

    sf::RenderWindow window(sf::VideoMode(1300, 400), "Visualizador de Stack y Heap");
    sf::Font font;
    if (!font.loadFromFile("/home/asus/.local/share/fonts/Roboto-Thin.ttf")) {
        std::cerr << "No se pudo cargar la fuente.\n";
        return 1;
    }

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed ||
                (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)) {
                window.close();
            }
        }

        drawMemory(window, variables, font);
    }
    return 0;
}