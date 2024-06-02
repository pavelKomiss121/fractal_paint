
#define _USE_MATH_DEFINES

#include <SFML/Graphics.hpp>
#include <cmath>
#include <vector>
#include <string>
#include <algorithm>
#include <locale>
#include <chrono>
#include <iostream>
#include <sstream>
#include <unordered_map>
#include <windows.h>

/*размер экрана*/
const int SCREEN_WIDTH = 1000;
const int SCREEN_HEIGHT = 800;

const float start_x_const = SCREEN_WIDTH / 2;
const float start_y_const = SCREEN_HEIGHT / 2;

bool less = false;
bool animation = false;
bool start_animation = false;
bool first = false;
bool paint = false;

char koch = 'q';
char serp = 'w';
char stick = 'e';
char pentogram = 'a';
char stick_2 = 'd';
char stick_3 = 'h';
char stick_4 = 'g';

/*размер меню*/
const int MENU_WIDTH = 400;                         // Ширина меню
const int MENU_HEIGHT = SCREEN_HEIGHT;              // Высота меню
const int MENU_LEFT = SCREEN_WIDTH - MENU_WIDTH;    // Позиция меню по горизонтали
const int MENU_TOP = 0;                             // Позиция меню по вертикали
const int TEXT_LEFT = MENU_LEFT + 100;              // Позиция текста по горизонтали

/*запуск счетчика времени для мигающей полоски ввода*/
auto start = std::chrono::high_resolution_clock::now();

/*создание переменной для угла*/
double angle_rad = 1; // Угол в радианах (60 градусов)


/*данные черепашки (ккординаты, кол-во итераций, угол поворота)*/
struct TurtleState {
    double x, y;
    double angle;
    int iteration;
};

std::vector<TurtleState> turtle_states;

/* Функция используется для отрисовки линии, соответствующей состоянию черепахи*/
void restore_state(sf::RenderWindow& window, int index, double& x, double& y, double& angle, double stitch_length, int branch, int i) {

    TurtleState& state = turtle_states[index];          // Получаем ссылку на текущее состояние черепахи
    if (angle == 0) {
        angle = -90;
    }
    // Создаем линию с заданными координатами и углом
    sf::Vertex line[2] = {
        sf::Vertex(sf::Vector2f(x, y), sf::Color(255,255,255)), // Начало линии
        sf::Vertex(sf::Vector2f(x + stitch_length * std::cos(angle), y + stitch_length * std::sin(angle)), sf::Color(255, 255, 255)) // Конец линии
    };

    // Рисуем линию в окне
    window.draw(line, 2, sf::Lines);
    if (first) {

        window.display();
    }
    // Обновляем текущие координаты черепахи, перемещая ее на длину стежка в направлении угла
    x += stitch_length * std::cos(angle);
    y += stitch_length * std::sin(angle);
}

/* Функция process_string обрабатывает строку с командами для управления черепахой*/
void process_string(sf::RenderWindow& window, const std::string& turtle_trip, double stitch_length, double start_x, double start_y, int branch) {
    double x = start_x;
    double y = start_y;
    double angle = turtle_states.empty() ? 0 : turtle_states.back().angle;
    size_t i = 0; // Изменено на size_t
    std::vector<TurtleState> saved_states;

    for (char symbol : turtle_trip) {
        switch (symbol) {
        case 'F':
            restore_state(window, static_cast<int>(turtle_states.size() - 1), x, y, angle, stitch_length, branch, i);
            break;
        case 'S':
            x += stitch_length * std::cos(angle);
            y += stitch_length * std::sin(angle);
            break;
        case '+':
            angle += angle_rad;
            break;
        case '-':
            angle -= angle_rad;
            break;
        case '[':
            saved_states.push_back(TurtleState{ x, y, angle, static_cast<int>(turtle_states.size()) });
            break;
        case ']':
            if (!saved_states.empty()) {
                TurtleState saved = saved_states.back();
                saved_states.pop_back();
                x = saved.x;
                y = saved.y;
                angle = saved.angle;
                turtle_states.resize(saved.iteration);
            }
            break;
        }
        i += 1;
    }
    first = false;
    if (!turtle_states.empty()) {
        turtle_states.back().x = x;
        turtle_states.back().y = y;
        turtle_states.back().angle = angle;
    }
}


/* Функция l_system обрабатывает строку, создавая путь черепашки*/
std::string l_system(const std::string& axiom, const std::vector<std::string>& rules, int iterations) {

    std::string turtle_trip = axiom;                // Начинаем с аксиомы (начальной строки)

    for (int i = 0; i < iterations; ++i) {

        std::string next;               //переменная для создани строки
        int index;                      //Индекс правила в векторе rules

        //Проходим по каждому символу в текущей строке turtle_trip
        for (int i = 0; i < turtle_trip.length(); i++) {
            std::string find(1, turtle_trip[i]);            //Считываем символ из пути черепашки

            // Ищем этот символ в векторе правил
            auto it = std::find(rules.begin(), rules.end(), find);


            if (it != rules.end()) {                        //Если символ найден в rules
                index = std::distance(rules.begin(), it);   //Получаем индекс правила

                next += rules.at(index + 1);                //Формируем путь черепашки, соответствующую правилу (индекс + 1, так как правила идут парами)

                /*В моей программе правила составляются парами, четные числа: строка, к которой необходимо применить правило,
                нечетное число: правило для предыдущей строки в векторе*/
            }
            else {

                next += turtle_trip[i];                     //Если символ не найден в rules, добавляем его без изменений
            }

        }

        turtle_trip = next;             // Добавлем к полному пути черепашки ее пройденный только что путь
    }
    return turtle_trip;
}

/* Создания экрана менюшки*/
void show_menu(sf::RenderWindow& window, int& iterations, double& angle, char& choice, std::string& axiom, std::vector<std::string>& rules) {
    sf::Font font;
    if (!font.loadFromFile("arial.ttf")) {
        return;
    }
    //Создание всех текстов
    sf::Text title("Меню", font, 90);
    title.setPosition(SCREEN_WIDTH / 2 - title.getLocalBounds().width / 2, 50);
    title.setFillColor(sf::Color::White);

    sf::Text iterations_text("Введите количество итераций: ", font, 25);
    iterations_text.setPosition(SCREEN_WIDTH / 4, SCREEN_HEIGHT / 4);
    iterations_text.setFillColor(sf::Color::White);

    sf::Text angle_text("Введите угол (в градусах): ", font, 25);
    angle_text.setPosition(SCREEN_WIDTH / 4, SCREEN_HEIGHT / 4 + 50);
    angle_text.setFillColor(sf::Color::White);

    sf::Text axiom_text("Введите аксиому: ", font, 25);
    axiom_text.setPosition(SCREEN_WIDTH / 4, SCREEN_HEIGHT / 4 + 100);
    axiom_text.setFillColor(sf::Color::White);

    sf::Text rules_text("Введите правила\n\n(формат ввода правил __ X:FX++F,Y:FF+F): ", font, 25);
    rules_text.setPosition(SCREEN_WIDTH / 4, SCREEN_HEIGHT / 4 + 150);
    rules_text.setFillColor(sf::Color::White);

    sf::Text input_text_angle("__", font, 25);
    sf::Text input_text("__", font, 25);

    sf::Text input_text_axiom("__", font, 25);
    sf::Text input_text_rules("__", font, 25);

    input_text.setPosition(2.75 * SCREEN_WIDTH / 4, SCREEN_HEIGHT / 4);
    input_text.setFillColor(sf::Color::White);

    input_text_angle.setPosition(2.75 * SCREEN_WIDTH / 4, SCREEN_HEIGHT / 4 + 50);
    input_text_angle.setFillColor(sf::Color::White);

    input_text_axiom.setPosition(2.75 * SCREEN_WIDTH / 4, SCREEN_HEIGHT / 4 + 100);
    input_text_axiom.setFillColor(sf::Color::White);

    input_text_rules.setPosition(2.75 * SCREEN_WIDTH / 4, SCREEN_HEIGHT / 4 + 150);
    input_text_rules.setFillColor(sf::Color::White);

    sf::Text fractal_choice_("1.   Фрактал Коха (q) +++\n2.   Треугольник Серпинского(w) +++\n3.   Веточка #1(e) +++\n4.   Кривая Леви(r) ---\n5.   Тройная кривая дракона(t) ---\n6.   Веточка #2 (u) ---\n7.   Кривая дракона(i) ---\n8.   Интересная фигура(o) ---\n9.   Кривая Хилберта(p) ---\n10.   Фрактал Пентаграммы (a) ---\n11.   Веточка #3 (d) +++\n12.   Кривая крест (g) ---\n13.   Веточка #4 (h) +++", font, 15);

    fractal_choice_.setPosition(SCREEN_WIDTH / 4 + 25, SCREEN_HEIGHT / 4 + 350);
    fractal_choice_.setFillColor(sf::Color::White);

    sf::Text fractal_choice("Либо выберите предустановленный фрактал: ", font, 25);
    fractal_choice.setPosition(SCREEN_WIDTH / 4, SCREEN_HEIGHT / 4 + 300);
    fractal_choice.setFillColor(sf::Color::White);


    sf::Texture texture;
    if (!texture.loadFromFile("background.jpg")) {
        return;
    }
    sf::Sprite background(texture);

    window.clear();
    window.draw(background);
    window.draw(title);
    window.draw(iterations_text);
    window.draw(angle_text);
    window.draw(fractal_choice);
    window.draw(fractal_choice_);
    window.draw(input_text);
    window.draw(input_text_angle);
    window.draw(input_text_rules);
    window.draw(input_text_axiom);

    window.display();

    std::string input_angle;
    std::string input_iteration;
    std::string input_axiom;
    std::string input_rules;
    bool is_typing_iterations = true;
    bool is_typing_angle = false;
    bool is_typing_axiom = false;
    bool is_typing_rules = false;

    bool one_type_iteration = false;
    bool one_type_angle = false;
    bool one_type_fractal_choice = false;
    bool one_type_axiom = false;
    bool one_type_rules = false;

    //Засекаем время для пиликанья строки ввода
    auto start = std::chrono::high_resolution_clock::now();

    while (window.isOpen()) {

        //Считаем разницу во времени между началом и текущим временем 
        auto current_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(current_time - start);

        //Если нужно вводить в 1 поле ввода
        if (!one_type_iteration) {
            //каждые 0,5 секунд пропадает
            if (duration.count() > 500) {
                input_text.setString("");
                //еще через 0,5 появляется снова
                if (duration.count() > 1000) {
                    input_text.setString("__");
                    //обновляется время начала
                    start = std::chrono::high_resolution_clock::now();
                }

            }
        }

        //Если нужно вводить в 2 поле ввода
        else if (is_typing_angle) {
            //пока не сделали ни одного нажатия при вводе угла
            if (!one_type_angle) {
                if (duration.count() > 500) {
                    input_text_angle.setString("");
                    if (duration.count() > 1000) {
                        input_text_angle.setString("__");
                        start = std::chrono::high_resolution_clock::now();
                    }

                }
            }
        }

        //Если нужно вводить в 3 поле ввода
        else if (is_typing_axiom) {
            if (!one_type_axiom) {
                if (duration.count() > 500) {
                    input_text_axiom.setString("");
                    if (duration.count() > 1000) {
                        input_text_axiom.setString("__");
                        start = std::chrono::high_resolution_clock::now();
                    }

                }
            }
        }

        //Если нужно вводить в 4 поле ввода
        else if (is_typing_rules) {
            if (!one_type_rules) {
                if (duration.count() > 500) {
                    input_text_rules.setString("");
                    if (duration.count() > 1000) {
                        input_text_rules.setString("__");
                        start = std::chrono::high_resolution_clock::now();
                    }

                }
            }
        }


        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                std::exit(0);
                return;
            }
            //выбор готовых пресетов
            if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Q) {
                    choice = koch;
                    return;
                }
                else if (event.key.code == sf::Keyboard::W) {
                    choice = serp;
                    return;
                }
                else if (event.key.code == sf::Keyboard::E) {
                    choice = 'e';
                    return;
                }
                else if (event.key.code == sf::Keyboard::R) {
                    choice = 'r';
                    return;
                }
                else if (event.key.code == sf::Keyboard::T) {
                    choice = 't';
                    return;
                }
                else if (event.key.code == sf::Keyboard::U) {
                    choice = 'u';
                    return;
                }
                else if (event.key.code == sf::Keyboard::I) {
                    choice = 'i';
                    return;
                }
                else if (event.key.code == sf::Keyboard::O) {
                    choice = 'o';
                    return;
                }
                else if (event.key.code == sf::Keyboard::P) {
                    choice = 'p';
                    return;
                }
                else if (event.key.code == sf::Keyboard::A) {
                    choice = 'a';
                    return;
                }
                else if (event.key.code == sf::Keyboard::D) {
                    choice = 'd';
                    return;
                }
                else if (event.key.code == sf::Keyboard::H) {
                    choice = 'h';
                    return;
                }
                else if (event.key.code == sf::Keyboard::G) {
                    choice = 'g';
                    return;
                }
                else if (event.key.code == sf::Keyboard::J) {
                    choice = 'j';
                    return;
                }
            }

            //Ввод текста в поля ввода
            if (event.type == sf::Event::TextEntered) {
                if (event.text.unicode == 13) { // Enter
                    if (is_typing_iterations) {
                        try {
                            iterations = std::stoi(input_iteration);
                            is_typing_iterations = false;
                            is_typing_angle = true;
                            input_text.setString(input_iteration);


                        }
                        catch (...) {
                            input_iteration.clear();
                            input_text.setString("Неверный формат. Попробуйте снова.");
                        }
                    }
                    else if (is_typing_angle) {
                        try {
                            angle = std::stod(input_angle);
                            input_text_angle.setString(input_angle);
                            is_typing_angle = false;
                            is_typing_axiom = true;

                        }
                        catch (...) {
                            input_angle.clear();
                            input_text_angle.setString("Неверный формат. Попробуйте снова.");
                        }
                    }
                    else if (is_typing_axiom) {
                        try {
                            axiom = input_axiom;
                            input_text_axiom.setString(input_axiom);
                            is_typing_axiom = false;
                            is_typing_rules = true;

                        }
                        catch (...) {
                            input_axiom.clear();
                            input_text_axiom.setString("Неверный формат. Попробуйте снова.");
                        }
                    }
                    else if (is_typing_rules) {
                        try {
                            std::istringstream stream(input_rules);
                            std::string token;
                            std::vector<std::string> parts;

                            while (std::getline(stream, token, ':')) {
                                std::istringstream token_stream(token);
                                std::string sub_token;
                                while (std::getline(token_stream, sub_token, ',')) {
                                    parts.push_back(sub_token);
                                }
                            }
                            for (const auto& part : parts) {
                                rules.push_back(part);
                            }

                            input_text_rules.setString(input_rules);
                            is_typing_rules = false;

                            return;
                        }
                        catch (...) {
                            input_rules.clear();
                            input_text_rules.setString("Неверный формат. Попробуйте снова.");
                        }
                    }

                }
                else if (event.text.unicode == 8) { // Backspace
                    if (!input_angle.empty() && is_typing_angle) {
                        input_angle.pop_back();
                        input_text_angle.setString(input_angle);
                    }
                    else if (!input_iteration.empty() && is_typing_iterations) {
                        input_iteration.pop_back();
                        input_text.setString(input_iteration);
                    }
                    else if (!input_rules.empty() && is_typing_rules) {
                        input_rules.pop_back();
                        input_text_rules.setString(input_rules);
                    }
                    else if (!input_axiom.empty() && is_typing_axiom) {
                        input_axiom.pop_back();
                        input_text_axiom.setString(input_axiom);
                    }

                }
                else if (event.text.unicode >= 32 && event.text.unicode <= 126) {
                    if (is_typing_iterations) {
                        one_type_iteration = true;
                        input_iteration += static_cast<char>(event.text.unicode);
                        input_text.setString(input_iteration);
                    }
                    else if (is_typing_angle) {
                        one_type_angle = true;
                        input_angle += static_cast<char>(event.text.unicode);
                        input_text_angle.setString(input_angle);
                    }
                    else if (is_typing_axiom) {
                        one_type_axiom = true;
                        input_axiom += static_cast<char>(event.text.unicode);
                        input_text_axiom.setString(input_axiom);
                    }
                    else if (is_typing_rules) {
                        one_type_rules = true;
                        input_rules += static_cast<char>(event.text.unicode);
                        input_text_rules.setString(input_rules);
                    }
                }
            }
        }
        window.clear();
        window.draw(background);
        window.draw(rules_text);
        window.draw(axiom_text);
        window.draw(title);
        window.draw(iterations_text);
        window.draw(angle_text);
        window.draw(fractal_choice);
        window.draw(fractal_choice_);
        window.draw(input_text_angle);
        window.draw(input_text_rules);
        window.draw(input_text_axiom);
        window.draw(input_text);

        window.display();
    }
}

/* Функция готовых пресетов*/
void script_fractal(sf::RenderWindow& window, char choice, int& iterations, double& angle, std::string& axiom, std::vector<std::string>& rules, double& stitch_length) {

    switch (choice) {
    case 'q':
        axiom = "F--F--F";
        //сначала добавляем символ, к которому необходимо применить правило
        rules.push_back("F");
        //правило для символа
        rules.push_back("F+F--F+F");
        stitch_length = 5;
        iterations = 4;
        angle = 60;

        break;
    case 'w':
        axiom = "FXF--FF--FF";
        rules.push_back("F");
        rules.push_back("FF");

        rules.push_back("X");
        rules.push_back("--FXF++FXF++FXF--");
        stitch_length = 6;
        iterations = 5;
        angle = 60;
        break;
    case 'i':

        axiom = "FX";
        rules.push_back("Y");
        rules.push_back("-FX-Y");

        rules.push_back("X");
        rules.push_back("X+YF+");
        stitch_length = 5;
        iterations = 11;
        angle = 90;
        break;
    case 'r':
        axiom = "F";
        rules.push_back("F");
        rules.push_back("+F--F+");
        stitch_length = 7;
        iterations = 8;
        angle = 45;
        break;
    case 't':
        axiom = "FX+FX+FX";
        rules.push_back("Y");
        rules.push_back("-FX-Y");

        rules.push_back("X");
        rules.push_back("X+YF+");
        stitch_length = 8;
        iterations = 8;
        angle = 90;
        break;
    case 'u':

        axiom = "X";
        rules.push_back("X");
        rules.push_back("F-[[X]+X]+F[+FX]-X");

        rules.push_back("F");
        rules.push_back("FF");
        stitch_length = 6;
        iterations = 5;
        angle = 25;
        break;
    case 'e':

        stitch_length = 12;
        axiom = "F";
        rules.push_back("F");
        rules.push_back("F[+F]F[-F][F]");
        iterations = 5;
        angle = 20;
        break;
    case 'o':


        axiom = "F+F+F+F";
        rules.push_back("F");
        rules.push_back("F+S-FF+F+FF+FS+FF-S+FF-F-FF-FS-FFF");
        rules.push_back("S");
        rules.push_back("SSSSSS");
        stitch_length = 5;
        iterations = 3;
        angle = 90;
        break;

    case 'p':

        //Кривая Хилберта
        stitch_length = 8;
        axiom = "A";
        rules.push_back("A");
        rules.push_back("−BF+AFA+FB−");
        rules.push_back("B");
        rules.push_back("+AF−BFB−FA+");
        iterations = 6;
        angle = 90;
        break;

    case 'a':

        //Фрактал Пентаграммы
        axiom = "F++F++F++F++F";
        rules.push_back("F");
        rules.push_back("F+F++F-F-F");
        stitch_length = 12;
        iterations = 4;
        angle = 72;
        break;

    case 'd':

        //Кривая крест
        /*axiom = "F";
        rules.push_back("F");
        rules.push_back("F+F−F−F+F");

        stitch_length = 6;
        iterations = 7;
        angle = 90;*/


        axiom = "Y";
        rules.push_back("X");
        rules.push_back("X[-FFF][+FFF]FX");

        rules.push_back("Y");
        rules.push_back("YFX[+Y][-Y]");

        stitch_length = 12;
        iterations = 5;
        angle = 25;
        break;

    case 'g':


        axiom = "F";
        rules.push_back("F");
        rules.push_back("F+F−F−F+F");

        stitch_length = 12;
        iterations = 5;
        angle = 90;


        break;
    case 'h':

        //Кривая крест
        /*axiom = "F";
        rules.push_back("F");
        rules.push_back("F+F−F−F+F");

        stitch_length = 6;
        iterations = 7;
        angle = 90;*/


        axiom = "F";
        rules.push_back("F");
        rules.push_back("F[+FF][-FF]F[-F][+F]F");


        stitch_length = 11;
        iterations = 4;
        angle = 36;
        break;

    case 'j':


        //Кривая крест
        /*axiom = "F";
        rules.push_back("F");
        rules.push_back("F+F−F−F+F");

        stitch_length = 6;
        iterations = 7;
        angle = 90;*/


        axiom = "F";
        rules.push_back("F");
        rules.push_back("FFF[+FFF+FFF+FFF]");


        stitch_length = 8;
        iterations = 4;
        angle = 90;
        break;
    default:
        break;
    }
}

/* Создания экрана настроек*/
class SettingsScreen {
public:
    SettingsScreen() : window(sf::VideoMode(SCREEN_WIDTH, SCREEN_HEIGHT), "Установленные настройки") {
        font.loadFromFile("arial.ttf");
        text.setFont(font);
        text.setCharacterSize(30);
        text.setFillColor(sf::Color::White);
        text.setPosition(SCREEN_WIDTH / 4, SCREEN_HEIGHT / 3.5);

        text_settings.setFont(font);
        text_settings.setCharacterSize(60);
        text_settings.setFillColor(sf::Color::White);
        text_settings.setPosition(SCREEN_WIDTH / 3.5, 50);

        text_enter.setFont(font);
        text_enter.setCharacterSize(20);
        text_enter.setFillColor(sf::Color::White);
        text_enter.setPosition(SCREEN_WIDTH / 3.5, SCREEN_HEIGHT - 80);

        text_animation.setFont(font);
        text_animation.setCharacterSize(25);
        text_animation.setFillColor(sf::Color::White);
        text_animation.setPosition(SCREEN_WIDTH / 4, SCREEN_HEIGHT - 300);

    }

    void setSettings(const std::string& axiom, const std::vector<std::string>& rules, int iterations, float angle) {

        std::string settingsText = "Аксиома: " + axiom + "\n";
        std::string rules_text;
        for (size_t i = 0; i < rules.size(); ++i) {
            rules_text += rules[i];
            if (i % 2 == 0) {
                rules_text += ":";
            }
            else {
                rules_text += "\n                  ";
            }
        }
        settingsText += "Правила: " + rules_text + "\n";
        settingsText += "Кол-во итераций: " + std::to_string(iterations) + "\n";
        settingsText += "Угол поворота: " + std::to_string(angle) + "\n\n";

        text.setString(settingsText);
        text_settings.setString("НАСТРОЙКИ");
        text_enter.setString("ДЛЯ ПРОДОЛЖЕНИЯ НАЖМИТЕ: ENTER");


    }

    void display() {
        sf::Texture texture;
        if (!texture.loadFromFile("background.jpg")) {
            return;
        }
        sf::Sprite background(texture);

        while (window.isOpen()) {
            sf::Event event;
            while (window.pollEvent(event)) {
                if (event.type == sf::Event::Closed)
                    std::exit(0);
                if (event.type == sf::Event::KeyPressed) {
                    if (event.key.code == sf::Keyboard::Enter) {
                        window.close();
                    }
                }
                if (event.type == sf::Event::KeyPressed) {
                    if (event.key.code == sf::Keyboard::C) {
                        if (!first)
                            first = true;
                        else first = false;
                    }
                }
            }

            std::string text_animation_ = "";
            text_animation_ += "Вам нужна анимация отрисовки?:\n   Если да, нажмите C\n";
            if (!first) {
                text_animation_ += "Анимация отрисовки выключена...\n";
            }
            else text_animation_ += "Анимация отрисовки включена!\n";
            text_animation.setString(text_animation_);



            window.clear();
            window.draw(background);
            window.draw(text);
            window.draw(text_settings);
            window.draw(text_enter);
            window.draw(text_animation);
            window.display();
        }
    }

private:
    sf::RenderWindow window;
    sf::Text text;
    sf::Text text_settings;
    sf::Text text_enter;
    sf::Text text_animation;
    sf::Font font;

};

void processFractal(sf::RenderWindow& window, std::string& axiom, std::vector<std::string>& rules, int iterations, double stitch_length, float start_x, float start_y) {
    static std::vector<std::string> iteration_saves;
    if (iteration_saves.empty()) {
        iteration_saves.push_back(axiom);
    }
    for (int i = iteration_saves.size(); i < iterations; i++) {
        iteration_saves.push_back(l_system(iteration_saves[iteration_saves.size() - 1], rules, 1));
    }

    turtle_states.push_back({ start_x, start_y, 0, 1 });
    process_string(window, iteration_saves[iterations - 1], stitch_length, start_x, start_y, 1);
}

void displayInstructions() {
    std::cout << "Для работы:\nВ менюшке можно ввести свои данные для создания фрактала, \nлибо нажать на соответствующий символ для создания скрипт. фрактала (на англ раскладке)\n\nПотом открывается окно настроек, там описаны правила, аксиома, итерации\n\nПотом будет окно с фракталом\n\nУправление:\n  стрелочки - движение фрактала\n  z - увеличение стежка, х - уменьшение стежка\n  с-увеличение кол-ва итераций, v - уменьшение кол-ва итераций";
    std::cout << "\n\nИ еще пару слов, сейчас нажатия на z, x зациклены, то есть когда длина стежка больше 3, то он возвращается на размер 1 вроде (это я просто для снежинки коха сделал)\n\nЧтобы получить снежинку Коха с бесконечным скролом нужно нажать 3 раза на 'C', потом просто нажимать на z";
    std::cout << "\n\nТакже я хочу реализовать: \n  плавное приближение, \n  на экране фрактала вывод глубины вложенности, названия фрактала, вывод кол-ва итераций";
}

void settings_manage(sf::RenderWindow& window, float rect_minus, int SCREEN_WIDTH, int SCREEN_HEIGHT, bool manage)
{

    sf::RectangleShape rectangle;
    sf::Color color(33, 33, 33);
    int rect_x = SCREEN_WIDTH / 3;
    int rect_y = SCREEN_HEIGHT / 3;

    rectangle.setOutlineThickness(4);
    rectangle.setOutlineColor(sf::Color(10, 10, 10));
    rectangle.setSize(sf::Vector2f(rect_x, rect_y));
    rectangle.setFillColor(color);
    rectangle.setPosition(SCREEN_WIDTH - rect_x - 50, SCREEN_HEIGHT - rect_y - 50);

    sf::Font font;

    sf::Text text_manage;
    font.loadFromFile("arial.ttf");
    text_manage.setFont(font);
    text_manage.setCharacterSize(16);
    text_manage.setFillColor(sf::Color::White);
    text_manage.setPosition(SCREEN_WIDTH - rect_x - 40, SCREEN_HEIGHT - rect_y - 40);
    //text_manage.setStyle(sf::Text::Bold); // Устанавливаем жирный стиль
    text_manage.setString("Q / E - запуск / прерывание анимации\n\nКолесико мыши - увеличить / уменьшить\n\nA,W,S,D - движение\n\nC / V - увеличить / уменьшить\n            кол-во итераций\n\nR - вернуть изначальный размер стежка");



    window.draw(rectangle);

    window.draw(text_manage);

}

void create_border_rectangles_and_text(sf::RenderWindow& window, sf::Color color, float rect_minus, int SCREEN_WIDTH, int SCREEN_HEIGHT, bool manage, char choice)
{
    // Создание прямоугольников для границ
    sf::RectangleShape rectangle_up;
    rectangle_up.setSize(sf::Vector2f(SCREEN_WIDTH, rect_minus + 10));
    rectangle_up.setFillColor(color);
    rectangle_up.setPosition(0, 0);

    sf::RectangleShape rectangle_down;
    rectangle_down.setSize(sf::Vector2f(SCREEN_WIDTH, rect_minus));
    rectangle_down.setFillColor(color);
    rectangle_down.setPosition(0, SCREEN_HEIGHT - rect_minus);

    sf::RectangleShape rectangle_left;
    rectangle_left.setSize(sf::Vector2f(rect_minus, SCREEN_HEIGHT));
    rectangle_left.setFillColor(color);
    rectangle_left.setPosition(0, 0);

    sf::RectangleShape rectangle_right;
    rectangle_right.setSize(sf::Vector2f(rect_minus, SCREEN_HEIGHT));
    rectangle_right.setFillColor(color);
    rectangle_right.setPosition(SCREEN_WIDTH - rect_minus, 0);

    sf::Font font;
    sf::Text text_name;
    font.loadFromFile("arial.ttf");
    text_name.setFont(font);
    text_name.setCharacterSize(30);
    text_name.setFillColor(sf::Color::White);
    text_name.setPosition(SCREEN_WIDTH / 2.0f - 150, rect_minus / 2.5);
    text_name.setStyle(sf::Text::Bold); // Устанавливаем жирный стиль
    text_name.setString("Отрисовка фрактала");

    sf::Text text_animation;
    font.loadFromFile("arial.ttf");
    text_animation.setFont(font);
    text_animation.setCharacterSize(20);
    text_animation.setFillColor(sf::Color::White);
    text_animation.setPosition(SCREEN_WIDTH / 10.0f, SCREEN_HEIGHT - rect_minus / 1.5);
    text_animation.setStyle(sf::Text::Bold); // Устанавливаем жирный стиль
    if (choice == koch || choice == serp || choice == stick || choice == stick_3 || choice == stick_4 || choice == stick_2) {
        if (animation) {
            text_animation.setString("Анимация включена");
        }
        else text_animation.setString("Анимация выключена");
    }
    else text_animation.setString("Анимация недоступна");


    sf::Text text_manage;
    font.loadFromFile("arial.ttf");
    text_manage.setFont(font);
    text_manage.setCharacterSize(20);
    text_manage.setFillColor(sf::Color::White);
    text_manage.setPosition(SCREEN_WIDTH / 2.0f + 130, SCREEN_HEIGHT - rect_minus / 1.5);
    //text_manage.setStyle(sf::Text::Bold); // Устанавливаем жирный стиль
    if (!manage)
        text_manage.setString("Для показа управления нажать: Tab");
    else text_manage.setString("Cкрыть управление нажать: Tab");


    window.draw(rectangle_up);
    window.draw(rectangle_down);
    window.draw(rectangle_left);
    window.draw(rectangle_right);
    window.draw(text_animation);
    window.draw(text_name);
    window.draw(text_manage);

    if (manage) {
        settings_manage(window, rect_minus, SCREEN_WIDTH, SCREEN_HEIGHT, manage);
    }

}

void hot_keys(sf::RenderWindow& window, double& stitch_length, int& iterations, float& directionX, float& directionY, int& count, int rect_minus, bool& manage) {
    sf::Event event;


    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed)
            window.close();



        if (event.type == sf::Event::MouseWheelMoved) {
            if (event.mouseWheel.delta > 0) {
                stitch_length += 0.2;
            }
            else if (event.mouseWheel.delta < 0) {
                if (stitch_length > 0.2) {
                    stitch_length -= 0.2;
                }
            }
        }
        if (event.type == sf::Event::KeyPressed) {
            if (event.key.code == sf::Keyboard::A) {
                directionX = -1;
            }
            else if (event.key.code == sf::Keyboard::D) {
                directionX = 1;
            }
            else if (event.key.code == sf::Keyboard::W) {
                directionY = -1;
            }
            else if (event.key.code == sf::Keyboard::S) {
                directionY = 1;
            }
            else if (event.key.code == sf::Keyboard::R) {
                if (stitch_length > 1) {
                    less = true;
                    stitch_length -= 0.5;
                }

            }

            else if (event.key.code == sf::Keyboard::C) {
                iterations += 1;
            }
            else if (event.key.code == sf::Keyboard::V) {
                iterations -= 1;
            }
            else if (event.key.code == sf::Keyboard::Q) {
                animation = true;
            }
            else if (event.key.code == sf::Keyboard::E) {
                animation = false;
                start_animation = false;
            }

            else if (event.key.code == sf::Keyboard::Tab) {
                if (manage) {
                    manage = false;
                }
                else {
                    manage = true;
                }
            }
        }
        if (event.type == sf::Event::KeyReleased) {
            if ((event.key.code == sf::Keyboard::A && directionX == -1) ||
                (event.key.code == sf::Keyboard::D && directionX == 1)) {
                directionX = 0;
            }
            if ((event.key.code == sf::Keyboard::W && directionY == -1) ||
                (event.key.code == sf::Keyboard::S && directionY == 1)) {
                directionY = 0;
            }
        }
    }
}

void animation_func(sf::RenderWindow& window, float& start_x, float& start_y, char choice, int& iterations, double& stitch_length) {
    if (animation) {
        //фрактал к центру


        if (!start_animation) {
            if ((abs(start_x - start_x_const) > 3) || (abs(start_y - start_y_const) > 3)) {
                if (start_x > start_x_const) {
                    start_x -= 6.0;
                    if (abs(start_x - start_x_const) <= 6) {
                        start_x = start_x_const;
                    }
                }
                else {
                    start_x += 6.0;
                    if (abs(start_x - start_x_const) <= 6) {
                        start_x = start_x_const;
                    }
                }

                if (start_y > start_y_const) {
                    start_y -= 6.0;
                    if (abs(start_y - start_y_const) <= 6) {
                        start_y = start_y_const;
                    }
                }
                else {
                    start_y += 6.0;
                    if (abs(start_y - start_y_const) <= 6) {
                        start_y = start_y_const;
                    }
                }

            }
            //фрактал к зацикленному размеру
            else
                if ((stitch_length != 2) && !start_animation) {
                    if (stitch_length > 2)
                        stitch_length -= 0.5;
                    if (stitch_length < 2)
                        stitch_length += 0.5;
                    if (abs(stitch_length - 2) < 0.5) {
                        stitch_length = 2;
                    }

                }
                else  start_animation = true;
        }

        if (start_animation) {

            if (choice == koch) {
                if (iterations != 7) {
                    if (iterations > 7) iterations -= 1;
                    else iterations += 1;
                    Sleep(100);
                }
                int y_need = start_y_const + 100;
                int x_need = start_x_const + 150;
                if ((abs(start_x - x_need) > 3) || (abs(start_y - y_need) > 3)) {
                    if (start_x > x_need) {
                        start_x -= 6.0;
                        if (abs(start_x - x_need) <= 6) {
                            start_x = x_need;
                        }
                    }
                    else {
                        start_x += 6.0;
                        if (abs(start_x - x_need) <= 6) {
                            start_x = x_need;
                        }
                    }

                    if (start_y > y_need) {
                        start_y -= 6.0;
                        if (abs(start_y - y_need) <= 6) {
                            start_y = y_need;
                        }
                    }
                    else {
                        start_y += 6.0;
                        if (abs(start_y - y_need) <= 6) {
                            start_y = y_need;
                        }
                    }

                }


                float coeff = 3 / stitch_length;
                stitch_length += 0.1 / coeff;

                if (stitch_length > 2.9 && stitch_length < 3.1) {
                    stitch_length = 1;
                }
                Sleep(10);
            }
            if (choice == serp) {
                if (iterations != 9) {
                    if (iterations > 9) iterations -= 1;
                    else iterations += 1;
                    Sleep(100);
                }
                int y_need = start_y_const + 200;
                int x_need = start_x_const + 200;
                if ((abs(start_x - x_need) > 3) || (abs(start_y - y_need) > 3)) {
                    if (start_x > x_need) {
                        start_x -= 6.0;
                        if (abs(start_x - x_need) <= 6) {
                            start_x = x_need;
                        }
                    }
                    else {
                        start_x += 6.0;
                        if (abs(start_x - x_need) <= 6) {
                            start_x = x_need;
                        }
                    }

                    if (start_y > y_need) {
                        start_y -= 6.0;
                        if (abs(start_y - y_need) <= 6) {
                            start_y = y_need;
                        }
                    }
                    else {
                        start_y += 6.0;
                        if (abs(start_y - y_need) <= 6) {
                            start_y = y_need;
                        }
                    }

                }


                float coeff = 3 / stitch_length;
                stitch_length += 0.1 / coeff;
                if (stitch_length > 2.9 && stitch_length < 3.1) {
                    stitch_length = 1.5;
                }
                Sleep(10);
            }
            if (choice == stick) {
                if (iterations != 8) {
                    if (iterations > 8) iterations -= 1;
                    else iterations += 1;
                    Sleep(100);
                }
                int y_need = start_y_const + 100;
                int x_need = start_x_const;
                if ((abs(start_x - x_need) > 3) || (abs(start_y - y_need) > 3)) {
                    if (start_x > x_need) {
                        start_x -= 6.0;
                        if (abs(start_x - x_need) <= 6) {
                            start_x = x_need;
                        }
                    }
                    else {
                        start_x += 6.0;
                        if (abs(start_x - x_need) <= 6) {
                            start_x = x_need;
                        }
                    }

                    if (start_y > y_need) {
                        start_y -= 6.0;
                        if (abs(start_y - y_need) <= 6) {
                            start_y = y_need;
                        }
                    }
                    else {
                        start_y += 6.0;
                        if (abs(start_y - y_need) <= 6) {
                            start_y = y_need;
                        }
                    }

                }

                float coeff = 6 / stitch_length;
                stitch_length += 0.2 / coeff;
                if (stitch_length > 6.7 && stitch_length < 6.9) {
                    stitch_length = 3.4;

                }
                Sleep(1);
            }

            if (choice == stick_2) {
                if (iterations != 10) {
                    if (iterations > 10) iterations -= 1;
                    else iterations += 1;
                    Sleep(100);
                    stitch_length = 1;
                }
                int y_need = start_y_const + 100;
                int x_need = start_x_const;
                if ((abs(start_x - x_need) > 3) || (abs(start_y - y_need) > 3)) {
                    if (start_x > x_need) {
                        start_x -= 6.0;
                        if (abs(start_x - x_need) <= 6) {
                            start_x = x_need;
                        }
                    }
                    else {
                        start_x += 6.0;
                        if (abs(start_x - x_need) <= 6) {
                            start_x = x_need;
                        }
                    }

                    if (start_y > y_need) {
                        start_y -= 6.0;
                        if (abs(start_y - y_need) <= 6) {
                            start_y = y_need;
                        }
                    }
                    else {
                        start_y += 6.0;
                        if (abs(start_y - y_need) <= 6) {
                            start_y = y_need;
                        }
                    }

                }

                float coeff = 4 / stitch_length;
                stitch_length += 0.2 / coeff;
                if (stitch_length > 1.55 && stitch_length < 1.65) {
                    stitch_length = 0.8;

                }
                Sleep(1);
            }
            if (choice == stick_3) {
                if (iterations != 6) {
                    if (iterations > 6) iterations -= 1;
                    else iterations += 1;
                    Sleep(100);
                    stitch_length = 1;
                }
                int y_need = start_y_const + 100;
                int x_need = start_x_const;
                if ((abs(start_x - x_need) > 3) || (abs(start_y - y_need) > 3)) {
                    if (start_x > x_need) {
                        start_x -= 6.0;
                        if (abs(start_x - x_need) <= 6) {
                            start_x = x_need;
                        }
                    }
                    else {
                        start_x += 6.0;
                        if (abs(start_x - x_need) <= 6) {
                            start_x = x_need;
                        }
                    }

                    if (start_y > y_need) {
                        start_y -= 6.0;
                        if (abs(start_y - y_need) <= 6) {
                            start_y = y_need;
                        }
                    }
                    else {
                        start_y += 6.0;
                        if (abs(start_y - y_need) <= 6) {
                            start_y = y_need;
                        }
                    }

                }

                float coeff = 6 / stitch_length;
                stitch_length += 0.2 / coeff;
                if (stitch_length > 6.0 && stitch_length < 6.2) {
                    stitch_length = 2;

                }
                Sleep(1);
            }


        }

    }

}


int main() {

    //установка русского языка
    std::locale::global(std::locale("Russian_Russia.1251"));

    displayInstructions();

    sf::RenderWindow menu(sf::VideoMode(SCREEN_WIDTH, SCREEN_HEIGHT), "Меню");
    menu.setFramerateLimit(60);

    int count = 0;
    int iterations;
    double angle;
    char choice = ' ';
    std::string axiom;
    std::vector<std::string> rules;

    double stitch_length = 4.0;
    //Отображение менюшки
    show_menu(menu, iterations, angle, choice, axiom, rules);

    //Установка параметров заскриптованному фракталу (если он не выбран, то не войдет в цикл)
    script_fractal(menu, choice, iterations, angle, axiom, rules, stitch_length);

    menu.close();

    //Отображение настроек
    SettingsScreen settingsScreen;
    settingsScreen.setSettings(axiom, rules, iterations, angle);
    settingsScreen.display();

    sf::RenderWindow window(sf::VideoMode(SCREEN_WIDTH, SCREEN_HEIGHT), "Фракталы");




    window.setFramerateLimit(60);

    //угол в радианах
    angle_rad = angle * M_PI / 180;

    //Скорость движения
    double moveSpeed = 5.0;
    float directionX = 0;
    float directionY = 0;

    //Длина стежка изначальная


    //Координаты начала отрисовки
    float start_x = SCREEN_WIDTH / 2;
    float start_y = SCREEN_HEIGHT / 2;


    int branch = 1; // Ненужная переменная

    //переменная увеличения
    double scale = 1.0;
    double scaleSpeed = 0.1;

    //Вектор, куда записываеются коды для итераций
    std::vector<std::string> iteration_saves;


    sf::Font font;
    //не помню, уже не использую это
    std::pair<std::string, std::pair<int, int>> result;
    result.second.first = -1;
    sf::Color color(10, 10, 10);
    int rect_minus = 60;

    bool manage = false;


    while (window.isOpen()) {

        window.clear(sf::Color(66, 66, 66));

        hot_keys(window, stitch_length, iterations, directionX, directionY, count, rect_minus, manage);

        start_x += 5.0 * directionX;

        start_y += 5.0 * directionY;
        if (less) {
            Sleep(10);
            stitch_length -= 0.5;
            if (stitch_length <= 2)
                less = false;
        }

        animation_func(window, start_x, start_y, choice, iterations, stitch_length);

        if (first)
            create_border_rectangles_and_text(window, color, rect_minus, SCREEN_WIDTH, SCREEN_HEIGHT, manage, choice);
        processFractal(window, axiom, rules, iterations, stitch_length, start_x, start_y);
        if (!first)
            create_border_rectangles_and_text(window, color, rect_minus, SCREEN_WIDTH, SCREEN_HEIGHT, manage, choice);

        window.display();
    }
    return 0;
}


