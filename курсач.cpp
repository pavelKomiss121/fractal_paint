
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

/*размер экрана*/
const int SCREEN_WIDTH = 1000;          
const int SCREEN_HEIGHT = 800;

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
void restore_state(sf::RenderWindow& window, int index, double& x, double& y, double& angle, double stitch_length, int branch, std::pair<std::string, std::pair<int, int>> result, int i) {
    
    TurtleState& state = turtle_states[index];          // Получаем ссылку на текущее состояние черепахи

    // Создаем линию с заданными координатами и углом
    sf::Vertex line[2] = {
        sf::Vertex(sf::Vector2f(x, y), sf::Color(255,255,255)), // Начало линии
        sf::Vertex(sf::Vector2f(x + stitch_length * std::cos(angle), y + stitch_length * std::sin(angle)), sf::Color(255, 255, 255)) // Конец линии
    };
    // Рисуем линию в окне
    window.draw(line, 2, sf::Lines);

    // Обновляем текущие координаты черепахи, перемещая ее на длину стежка в направлении угла
    x += stitch_length * std::cos(angle);
    y += stitch_length * std::sin(angle);
}


/* Функция process_string обрабатывает строку с командами для управления черепахой*/
void process_string(sf::RenderWindow& window, const std::string& turtle_trip, double stitch_length, double start_x, double start_y, int branch, std::pair<std::string, std::pair<int, int>> result) {
    
    // Начальные координаты черепахи
    double x = start_x;
    double y = start_y;

    // Начальный угол поворота черепахи, если turtle_states не пустой, то берем последний угол, иначе 0
    double angle = turtle_states.empty() ? 0 : turtle_states.back().angle;

    // Счетчик итераций (не помню зачем)
    int i = 0;
    // Проходим по каждому символу в строке s
    for (char symbol : turtle_trip) {
        // В зависимости от символа выполняем соответствующую команду
        switch (symbol) {
        case 'F':               // Команда 'F' означает движение вперед на длину стежка
            restore_state(window, turtle_states.size() - 1, x, y, angle, stitch_length, branch, result, i);
            break;
        case '+':               // Команда '+' означает поворот направо на угол ANGLE
            angle += angle_rad;
            break;
        case '-':               // Команда '-' означает поворот налево на угол ANGLE
            angle -= angle_rad;
            break;
        }
        i += 1;
    }

    // Если turtle_states не пустой, обновляем последнее состояние черепахи
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

    sf::Text fractal_choice_("1.   Фрактал Коха (a)\n2.   Треугольник Серпинского(b)\n3.   Кривая дракона(c)\n4.   Кривая Леви(d)\n5.   Тройная кривая дракона(e)", font, 25);
       
    fractal_choice_.setPosition(SCREEN_WIDTH / 4+25, SCREEN_HEIGHT / 4 + 350);
    fractal_choice_.setFillColor(sf::Color::White);

    sf::Text fractal_choice("Либо выберите предустановленный фрактал (a-e): ", font, 25);
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
                if (event.key.code == sf::Keyboard::A) {
                    choice = 'a';
                    return;
                }
                else if (event.key.code == sf::Keyboard::B) {
                    choice = 'b';
                    return;
                }
                else if (event.key.code == sf::Keyboard::C) {
                    choice = 'c';
                    return;
                }
                else if (event.key.code == sf::Keyboard::D) {
                    choice = 'd';
                    return;
                }
                else if (event.key.code == sf::Keyboard::E) {
                    choice = 'e';
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
void script_fractal(sf::RenderWindow& window, char choice, int& iterations, double& angle, std::string& axiom, std::vector<std::string>& rules) {

    switch (choice) {
    case 'a':
        axiom = "F--F--F";
        //сначала добавляем символ, к которому необходимо применить правило
        rules.push_back("F");
        //правило для символа
        rules.push_back("F+F--F+F");

        iterations = 4;
        angle = 60;

        break;
    case 'b':
        axiom = "FXF--FF--FF";
        rules.push_back("F");
        rules.push_back("FF");

        rules.push_back("X");
        rules.push_back("--FXF++FXF++FXF--");

        iterations = 7;
        angle = 60;
        break;
    case 'c':

        axiom = "FX";
        rules.push_back("Y");
        rules.push_back("-FX-Y");

        rules.push_back("X");
        rules.push_back("X+YF+");
        iterations = 9;
        angle = 90;
        break;
    case 'd':
        axiom = "F";
        rules.push_back("F");
        rules.push_back("+F--F+");

        iterations = 10;
        angle = 45;
        break;
    case 'e':
        axiom = "FX+FX+FX";
        rules.push_back("Y");
        rules.push_back("-FX-Y");

        rules.push_back("X");
        rules.push_back("X+YF+");
        iterations = 7;
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
        text.setPosition(SCREEN_WIDTH / 4 , SCREEN_HEIGHT/2.5);

        text_settings.setFont(font);
        text_settings.setCharacterSize(60);
        text_settings.setFillColor(sf::Color::White);
        text_settings.setPosition(SCREEN_WIDTH / 3.5 , 50);

        text_enter.setFont(font);
        text_enter.setCharacterSize(20);
        text_enter.setFillColor(sf::Color::White);
        text_enter.setPosition(SCREEN_WIDTH / 3.5, SCREEN_HEIGHT-80);
        
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
                rules_text += "  ";
            }
        }
        settingsText += "Правила построения: " + rules_text + "\n";
        settingsText += "Кол-во итераций: " + std::to_string(iterations) + "\n";
        settingsText += "Угол поворота: " + std::to_string(angle) + "\n";
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
            }

            window.clear();
            window.draw(background);
            window.draw(text);
            window.draw(text_settings);
            window.draw(text_enter);
            
            window.display();
        }
    }

private:
    sf::RenderWindow window;
    sf::Text text;
    sf::Text text_settings;
    sf::Text text_enter;
    sf::Font font;

};


std::pair<std::string, std::pair<int, int>> findLongestRepeatedSubstring(const std::string& str) {
    int n = str.length();
    std::unordered_map<std::string, std::pair<int, int>> substrCount;
    std::pair<std::string, std::pair<int, int>> longestRepeated("", { 0, 0 });

    for (int len = 1; len <= n; ++len) {
        for (int i = 0; i <= n - len; ++i) {
            std::string substr = str.substr(i, len);
            if (substrCount.find(substr) != substrCount.end()) {
                if (len > longestRepeated.second.first) {
                    longestRepeated = { substr, {len, i} };
                }
            }
            else {
                substrCount[substr] = { len, i };
            }
        }
    }

    return longestRepeated;
}

int main() {

    //установка русского языка
    std::locale::global(std::locale("Russian_Russia.1251"));

    std::cout << "Для работы:\nВ менюшке можно ввести свои данные для создания фрактала, \nлибо нажать на соответствующий символ для создания скрипт. фрактала (на англ раскладке)\n\nПотом открывается окно настроек, там описаны правила, аксиома, итерации\n\nПотом будет окно с фракталом\n\nУправление:\n  стрелочки - движение фрактала\n  z - увеличение стежка, х - уменьшение стежка\n  с-увеличение кол-ва итераций, v - уменьшение кол-ва итераций";
    std::cout << "\n\nИ еще пару слов, сейчас нажатия на z, x зациклены, то есть когда длина стежка больше 3, то он возвращается на размер 1 вроде (это я просто для снежинки коха сделал)\n\nЧтобы получить снежинку Коха с бесконечным скролом нужно нажать 3 раза на 'C', потом просто нажимать на z";
    std::cout << "\n\nТакже я хочу реализовать: \n  плавное приближение, \n  на экране фрактала вывод глубины вложенности, названия фрактала, вывод кол-ва итераций";
    
    sf::RenderWindow menu(sf::VideoMode(SCREEN_WIDTH, SCREEN_HEIGHT), "Меню");
    menu.setFramerateLimit(60);

    int count = 0;
    int iterations;
    double angle;
    char choice=' ';
    std::string axiom;
    std::vector<std::string> rules;

    //Отображение менюшки
    show_menu(menu, iterations, angle, choice, axiom, rules);
    
    //Установка параметров заскриптованному фракталу (если он не выбран, то не войдет в цикл)
    script_fractal(menu, choice, iterations, angle, axiom, rules);
    
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
    int directionX = 0;
    int directionY = 0;

    //Длина стежка изначальная
    double stitch_length = 1.0;

    //Координаты начала отрисовки
    double start_x = SCREEN_WIDTH/2;
    double start_y = SCREEN_HEIGHT/2 ;

    
    int branch = 1; // Ненужная переменная

    //переменная увеличения
    double scale = 1.0;
    double scaleSpeed = 0.1;

    //Вектор, куда записываеются коды для итераций
    std::vector<std::string> iteration_saves;

    //не помню, уже не использую это
    std::pair<std::string, std::pair<int, int>> result;
    result.second.first = -1;

while (window.isOpen()) {
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed)
            window.close();

        // Обработка нажатий клавиш стрелок
        if (event.type == sf::Event::KeyPressed) {
            if (event.key.code == sf::Keyboard::Left) {
                directionX = -1;
            }
            else if (event.key.code == sf::Keyboard::Right) {
                directionX = 1;
            }
            else if (event.key.code == sf::Keyboard::Up) {
                directionY = -1;
            }
            else if (event.key.code == sf::Keyboard::Down) {
                directionY = 1;
            }

            /*if (event.type == sf::Event::MouseWheelMoved) {
                if (event.mouseWheel.delta > 0) {
                    stitch_length += 0.2;
                }
                else if (event.mouseWheel.delta < 0) {
                    stitch_length -= 0.2;
                }
            }*/

            else if (event.key.code == sf::Keyboard::Z) {       //увеличение
                //stitch_length += (stitch_length + 0.2 * count) * 0.2;
                stitch_length += 0.2;
                if (stitch_length > 2.9 && stitch_length < 3.1) {
                    stitch_length = 1;
                    count += 1;
                }
               
            }
            else if (event.key.code == sf::Keyboard::X) {       //уменьшение
                if (stitch_length > 0.2) {
                    stitch_length -= 0.2;
                }
                //настройка для снежинки коха
                if (count != 0 && stitch_length > 0.9 && stitch_length < 1.1) {
                    stitch_length = 3;
                    count -= 1;
                }
            }

            else if (event.key.code == sf::Keyboard::C) { // Масштабирование внутрь
                iterations += 1;
                
            }
            else if (event.key.code == sf::Keyboard::V) { // Масштабирование наружу
                iterations -= 1;
            }
        }

        // Обработка отпускания клавиш стрелок
        if (event.type == sf::Event::KeyReleased) {
            if ((event.key.code == sf::Keyboard::Left && directionX == -1) ||
                (event.key.code == sf::Keyboard::Right && directionX == 1)) {
                directionX = 0;
            }
            if ((event.key.code == sf::Keyboard::Up && directionY == -1) ||
                (event.key.code == sf::Keyboard::Down && directionY == 1)) {
                directionY = 0;
            }
        }
    }

    // Обновление начальных координат фрактала
    start_x += moveSpeed * directionX;
    start_y += moveSpeed * directionY;

    window.clear(sf::Color::Black);

    //std::string output = l_system(axiom, rules, iterations);
    if (iteration_saves.empty()) {
        iteration_saves.push_back(axiom);
    }
    for (int i = iteration_saves.size(); i < iterations; i++) {
        iteration_saves.push_back(l_system(iteration_saves[iteration_saves.size() - 1], rules, 1));

    }
    
    turtle_states.push_back({ start_x, start_y, 0, branch });
    
    process_string(window, iteration_saves[iterations - 1], stitch_length, start_x, start_y, branch, result);

    window.display();
}

    return 0;
}

