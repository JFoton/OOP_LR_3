#include "graphics.h"
#pragma comment(lib, "Gdiplus.lib")

Grfx::Graphics console_graphics;

const char MENU = 'q';

const char TRAJ = 'z';
const char TRAJ2 = 'x';

const char SaveToFile = 'f';
const char ReadFromFile = 'l';

const char ChangeObject = 'o';
const char AddObject = 'g';
const char ChangeColor = 'r';
const char ChangeSize = 'm';
const char Hideobject = 'h';
const char Showobject = 'j';

const char ClearScreen = 'c';
const char ScreenSize = 'v';

const char ShapeTrail = 't';

const char UP = 'w';
const char DOWN = 's';
const char LEFT = 'a';
const char RIGHT = 'd';

const int  BGCOLOR = 0;
const int  COLOR = 2;
const int  STEP = 10;

class Shape
{
protected:
    int x, y, color, size;
    bool drawTrail = false;

    void drawPixel(int x, int y, int c) {
        console_graphics.setcolor(c);
        console_graphics.rectangle(x, y, x + 1, y + 1);
    }

public:
    Shape(int a, int b, int c) : x(a), y(b), color(c), size(1), drawTrail(false) {}

    virtual ~Shape() {};
    virtual void draw(int c) = 0;
    virtual void move(int dx, int dy) = 0;
    virtual void setColor(int c) { color = c; }
    virtual void setSize(int s) { size = s; }
    virtual int getSize() { return size; }
    virtual void resize(int delta) { size += delta; }

    void show() { draw(color); }
    void hide() { draw(BGCOLOR); }
    void SetTrail(bool value) { drawTrail = value; }
    void toggleTrail() { drawTrail = !drawTrail; }

    int getX() { return this->x; }
    int getY() { return this->y; }
    int getColor() { return this->color; }
    bool getDrawTrail() { return this->drawTrail; }

    virtual std::string getType() const {
        return "Shape";
    }
};

class Segment : public Shape
{
    int dx, dy;
    std::vector<std::pair<int, int>> trail;

public:
    Segment(int a, int b, int da, int db, int c) : Shape(a, b, c), dx(da), dy(db) { show(); }

    void draw(int c) override {
        console_graphics.setcolor(c);
        console_graphics.line(x, y, x + dx, y + dy);

        if (drawTrail) {
            for (const auto& point : trail) {
                drawPixel(point.first, point.second, c);
            }
        }
    }

    int getSize() override {
        return this->dx;
    }


    void resize(int delta) override {
        dx += delta;
        dy += delta;
    }

    void setColor(int c) override {
        color = c;
    }

    void move(int dx, int dy) override {
        hide();

        if (drawTrail) {
            trail.push_back(std::make_pair(x, y));
        }

        x += dx;
        y += dy;

        show();
    }

    void setSize(int s) override {
        double ratio = static_cast<double>(s) / getSize();
        dx = static_cast<int>(dx * ratio);
        dy = static_cast<int>(dy * ratio);
    }

    std::string getType() const override {
        return "Segment";
    }
};

class Star : public Shape
{
    int innerRadius;
    int outerRadius;
    std::vector<std::pair<int, int>> trail; // Trail coordinates

public:
    Star(int a, int b, int inner, int outer, int c) : Shape(a, b, c), innerRadius(inner), outerRadius(outer) { show(); }

    void setColor(int c) override {
        color = c;
    }

    int getSize() override {
        return innerRadius;
    }

    void resize(int delta) override {
        innerRadius += delta;
        outerRadius += delta;
    }

    void draw(int c) override {
        console_graphics.setcolor(c);

        // Draw the star using lines
        for (int i = 0; i < 360; i += 36) {
            double angle1 = i * M_PI / 180;
            double angle2 = (i + 36) * M_PI / 180;

            int x1 = static_cast<int>(x + outerRadius * cos(angle1));
            int y1 = static_cast<int>(y + outerRadius * sin(angle1));

            int x2 = static_cast<int>(x + innerRadius * cos(angle2));
            int y2 = static_cast<int>(y + innerRadius * sin(angle2));

            console_graphics.line(x1, y1, x2, y2);
        }

        if (getDrawTrail()) {
            // Draw the trail for stars
            for (const auto& point : trail) {
                drawPixel(point.first, point.second, c);
            }
        }
    }

    void move(int dx, int dy) override {
        hide();

        if (getDrawTrail()) {
            trail.push_back(std::make_pair(x, y));
        }

        x += dx;
        y += dy;

        show();
    }

    void setSize(int s) override {
        double ratio = static_cast<double>(s) / getSize();
        innerRadius = static_cast<int>(innerRadius * ratio);
        outerRadius = static_cast<int>(outerRadius * ratio);
    }

    std::string getType() const override {
        return "Star";
    }

protected:
    void drawTrailPixels(int c) {
        for (const auto& point : trail) {
            drawPixel(point.first, point.second, c);
        }
    }
};

class MyRectangle : public Shape
{
    int width, height;
    std::vector<std::pair<int, int>> trail;

public:
    MyRectangle(int a, int b, int w, int h, int c) : Shape(a, b, c), width(w), height(h) { show(); }

    void draw(int c) override {
        console_graphics.setcolor(c);
        console_graphics.rectangle(x, y, x + width, y + height);

        if (drawTrail) {
            for (const auto& point : trail) {
                drawPixel(point.first, point.second, c);
            }
        }
    }


    void move(int dx, int dy) override {
        hide();

        if (drawTrail) {
            trail.push_back(std::make_pair(x, y));
        }

        x += dx;
        y += dy;

        show();
    }

    void setSize(int s) override {
        double ratio = static_cast<double>(s) / getSize();
        width = static_cast<int>(width * ratio);
        height = static_cast<int>(height * ratio);
    }
};

class Circle : public Shape
{
    int radius;
    std::vector<std::pair<int, int>> trail;

public:
    Circle(int a, int b, int r, int c) : Shape(a, b, c), radius(r) { show(); }

    std::string getType() const override {
        return "Circle";
    }

    void setColor(int c) override {
        color = c;
    }

    void draw(int c) override {
        console_graphics.setcolor(c);
        console_graphics.circle(x, y, radius);

        if (drawTrail) {
            for (const auto& point : trail) {
                drawPixel(point.first, point.second, c);
            }
        }
    }

    void resize(int delta) override {
        radius += delta;
    }

    void move(int dx, int dy) override {
        hide();

        if (drawTrail) {
            trail.push_back(std::make_pair(x, y));
        }

        x += dx;
        y += dy;

        show();
    }

    int getSize() override
    {
        return this->radius;
    }

    void setSize(int s) override {
        double ratio = static_cast<double>(s) / getSize();
        radius = static_cast<int>(radius * ratio);
    }
};

class Square : public Shape
{
    int side;
    std::vector<std::pair<int, int>> trail;

public:
    Square(int a, int b, int s, int c) : Shape(a, b, c), side(s) { show(); }

    void setColor(int c) override {
        color = c;
    }

    int getSize() override {
        return this->side;
    }

    std::string getType() const override {
        return "Square";
    }

    void draw(int c) override {
        console_graphics.setcolor(c);
        console_graphics.rectangle(x, y, x + side, y + side);

        if (drawTrail) {
            for (const auto& point : trail) {
                drawPixel(point.first, point.second, c);
            }
        }
    }

    void resize(int delta) override {
        side += delta;
    }

    void move(int dx, int dy) override {
        hide();

        if (drawTrail) {
            trail.push_back(std::make_pair(x, y));
        }

        x += dx;
        y += dy;

        show();
    }

    void setSize(int s) override {
        double ratio = static_cast<double>(s) / getSize();
        side = static_cast<int>(side * ratio);
    }
};

void menu(const std::vector<Shape*>& objects) {

    for (const auto& obj : objects)
    {
        obj->hide();
    }

    std::cout << "Выберите действие:" << std::endl;

    std::cout << SaveToFile << " - Сохранить в файл" << std::endl;
    std::cout << ReadFromFile << " - Прочитать из файла" << std::endl;

    std::cout << TRAJ << " - Запомнить траекторию" << std::endl;
    std::cout << TRAJ2 << " - Двигатся по траектории" << std::endl;

    //std::cout << Composite << " - Создать агрегат" << std::endl;

    std::cout << ChangeObject << " - Изменить объект" << std::endl;
    std::cout << AddObject << " - Добавить объект" << std::endl;
    std::cout << Hideobject << " - Скрыть объект" << std::endl;
    std::cout << Showobject << " - Показать объект" << std::endl;
    std::cout << ChangeSize << " - Изменить размер объекта" << std::endl;
    std::cout << ChangeColor << " - Изменить цвет объекта" << std::endl;

    std::cout << ClearScreen << " - Очистить экран" << std::endl;
    std::cout << ScreenSize << " - Изменить размер экрана" << std::endl;
    std::cout << ShapeTrail << " - Отобразить/скрыть траекторию объекта" << std::endl;

    std::cout << UP << " - Двигаться вверх" << std::endl;
    std::cout << DOWN << " - Двигаться вниз" << std::endl;
    std::cout << LEFT << " - Двигаться влево" << std::endl;
    std::cout << RIGHT << " - Двигаться вправо" << std::endl;

    system("pause");
    system("cls");

    for (const auto& obj : objects)
    {
        obj->show();
    }
}

void clearConsoleLine(int line) {
    COORD cursorPosition;
    cursorPosition.X = 0;
    cursorPosition.Y = line;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), cursorPosition);

    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);

    DWORD written;
    FillConsoleOutputCharacter(GetStdHandle(STD_OUTPUT_HANDLE), ' ', csbi.dwSize.X, cursorPosition, &written);
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), cursorPosition);
}

void addObject(std::vector<Shape*>& objects) {

    std::cout << "Выберите тип создаваемого объекта (1 - Segment, 2 - Circle, 3 - Square, 4 - Star): ";
    char choice;
    std::cin >> choice;

    clearConsoleLine(0);
    std::cin.ignore(32767, '\n');


    switch (choice) {
    case '1':
        objects.push_back(new Segment(200, 200, 100, 100, COLOR));
        break;

    case '2':
        objects.push_back(new Circle(300, 300, 50, COLOR));
        break;

    case '3':
        objects.push_back(new Square(400, 400, 50, COLOR));
        break;

    case '4':
        objects.push_back(new Star(500, 400, 30, 20, COLOR));
        break;   

    default:
        std::cout << "Введен некорректный номер." << std::endl;
        clearConsoleLine(0);
        std::cin.ignore(32767, '\n');
        break;
    }
}

int switchObject(const std::vector<Shape*>& objects) {
    if (objects.size() >= 2) {
        int obj = 0;
        std::cout << "Введите номер объекта, с которым вы хотите работать( всего объектов активно: " << objects.size() << ")";
        std::cin >> obj;
        clearConsoleLine(0);
        std::cin.ignore(32767, '\n');
        std::cin.clear();

        while (obj < 0 || obj > objects.size())
        {
            std::cout << "Пожалуйста, введите корректный номер объекта";
            std::cin >> obj;
            clearConsoleLine(0);
            std::cin.ignore(32767, '\n');
            std::cin.clear();
        }
        return obj - 1;
    }
    return int(objects.size() - 1);
}

void resizeObject(Shape*& object) {

    std::cout << "Введите новый размер для текущего объекта: ";
    int newSize;
    std::cin >> newSize;
    clearConsoleLine(0);
    std::cin.ignore(32767, '\n');

    if (std::cin.fail()) {
        std::cin.clear();
        std::cin.ignore(32767, '\n');
        std::cout << "Не удалось записать. Попробуйте еще раз"; std::cin >> newSize;
        clearConsoleLine(0);
        std::cin.ignore(32767);
    }
    else {
        object->hide();
        object->resize(object->getSize() + newSize);
    }
}

void Recolor(Shape*& object)
{
    std::cout << "Выберите цвет 1 - Красный, 2 - Синий, 3 - Фиолетовый, 4 - Белый ";
    int color;
    std::cin >> color;
    clearConsoleLine(0);
    std::cin.ignore(32767, '\n');

    switch (color)
    {
    case 1:
        object->setColor(4);
        break;
    case 2:
        object->setColor(1);
        break;
    case 3:
        object->setColor(5);
        break;
    case 4:
        object->setColor(7);
        break;
    default:
        Recolor(object);
        break;
    }
}

void SFile(const std::vector<Shape*> objects) {

    std::string filename;
    std::cin.clear();
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);
    std::cout << "Введите путь к файлу: ";
    std::getline(std::cin, filename);
    clearConsoleLine(0);
    std::cin.ignore(32767, '\n');
    std::cin.clear();
    std::ofstream file(filename);

    if (file.is_open()) {
        for (const auto& obj : objects) {
            file << obj->getType() << " "
                << obj->getX() << " "
                << obj->getY() << " "
                << obj->getSize() << " "
                << obj->getColor() << " \n";
        }

        file.close();
    }

    SetConsoleCP(866);
    SetConsoleOutputCP(866);
}

void RFile(std::vector<Shape*>& objects) {
    for (const auto& obj : objects)
    {
        obj->hide();
    }

    std::string filename;
    std::cin.clear();
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);
    std::cout << "Введите путь к файлу: ";
    std::getline(std::cin, filename);
    clearConsoleLine(0);
    std::cin.clear();
    std::ifstream file(filename);

    if (file.is_open()) {
        std::string line;
        while (std::getline(file, line)) {
            std::istringstream iss(line);
            std::string objectType;
            iss >> objectType;


            int x, y, size, color;
            iss >> x >> y >> size >> color;

            if (objectType == "Segment") {
                objects.push_back(new Segment(x, y, size, size, color));
            }
            else if (objectType == "Circle") {
                objects.push_back(new Circle(x, y, size, color));
            }
            else if (objectType == "Square") {
                objects.push_back(new Square(x, y, size, color));
            }

            else if (objectType == "Star") {
                objects.push_back(new Star(x, y, size, 2 * size / 3, color));
            }
        }
        file.close();
    }


    for (const auto& obj : objects)
    {
        obj->show();
    }
}

void trajectory(std::vector <int>& t, const char step)
{
    if (step == UP) { t.push_back(0); return; }
    if (step == DOWN) { t.push_back(1); return; }
    if (step == LEFT) { t.push_back(2); return; }
    if (step == RIGHT) { t.push_back(3); return; }
}

char trajectory2(const std::vector <int>& t, const int i)
{
    if (t[i] == 0) return UP;
    if (t[i] == 1) return DOWN;
    if (t[i] == 2) return LEFT;
    if (t[i] == 3) return RIGHT;
    return _getch();
}

int main() {

    std::vector<Shape*> objects;
    objects.push_back(new Segment(200, 200, 100, 100, COLOR));

    std::vector <int> t;

    setlocale(LC_ALL, "russian");

    int iter = 0, i = 0;
    bool tr1 = false, tr2 = false;
    char c = 0;

    while (c != 27)
    {
        /*if (GetAsyncKeyState(VK_LEFT) & 0x8000) objects.at(iter)->move(-STEP, 0);
            if (GetAsyncKeyState(VK_RIGHT) & 0x8000) objects.at(iter)->move(STEP, 0);
            if (GetAsyncKeyState(VK_UP) & 0x8000) objects.at(iter)->move(0, -STEP);
            if (GetAsyncKeyState(VK_DOWN) & 0x8000) objects.at(iter)->move(0, STEP);*/

        if (tr1)
        {
            trajectory(t, c);
        }

        if (tr2)
        {
            c = trajectory2(t, i);
            if (i == t.size() - 1) {
                trajectory2(t, i);
                tr2 = false;
                i = 0;
                c = _getch();
            }
            else i++;
        }
        else
        {
            c = _getch();
        }

        switch (c)
        {
        case UP:
            objects.at(iter)->move(0, -STEP);
            break;
        case DOWN:
            objects.at(iter)->move(0, STEP);
            break;
        case LEFT:
            objects.at(iter)->move(-STEP, 0);
            break;
        case RIGHT:
            objects.at(iter)->move(STEP, 0);
            break;

        case MENU:
            menu(objects);
            break;

        case ChangeColor:
            Recolor(objects.at(iter));
            break;

        case Showobject:
            objects.at(iter)->show();
            break;

        case ChangeSize:
            resizeObject(objects.at(iter));
            break;

        case TRAJ:
            if (tr1 == false && t.size() != 0) { tr1 = true; t.clear(); break; }
            if (tr1 == false) { tr1 = true; break; }
            tr1 = false;
            break;

        case TRAJ2:
            if (tr2 == false) { tr2 = true; break; }
            tr2 = false;
            break;

        case SaveToFile:
            SFile(objects);
            break;

        case ReadFromFile:
            RFile(objects);
            break;
        case Hideobject:
            objects.at(iter)->hide();
            break;

        case ClearScreen:
            console_graphics.cls();
            break;

        case ShapeTrail:

            if (!objects.empty()) {
                objects.at(iter)->toggleTrail();
            }
            break;

        case ScreenSize:
            std::cout << console_graphics.hSize() << ' ' << console_graphics.vSize() << std::endl;
            break;

        case AddObject:
            addObject(objects);
            iter++;
            break;

        case ChangeObject:
            iter = switchObject(objects);
            break;

        default:
            std::cin.clear();
            c = _getch();
            break;
        }


        if (!objects.empty()) {
            objects.at(iter)->show();
        }
    }


    for (Shape* obj : objects) {
        delete obj;
    }

    return 0;
}