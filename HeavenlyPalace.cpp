#include "gameHeaders.h"

using colour_t = int;

// Получить код цвета с выбранными цветами фона и текста
colour_t colour(Colour back, Colour text = Colour::BLACK) {
	return ((int)back * (int)Colour::MAX_COLOURS + (int)text);
}

// Вывести текст с кодом цвета
void PrintColour(std::string str, colour_t col = 15) {
	HANDLE hC = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hC, col);
	std::cout << str;

	SetConsoleTextAttribute(hC, colour(Colour::BLACK, Colour::WHITE));
}

struct Point {
	int x{ 0 };
	int y{ 0 };

	Point(int _x, int _y) : x(_x), y(_y) {}
	Point(const Point& _other) : x(_other.x), y(_other.y) {}

	Point& operator=(const Point& _other) noexcept(true) { x = _other.x; y = _other.y; return *this; }
	Point& operator=(Point&& _other)      noexcept(true) { x = _other.x; y = _other.y; return *this; }
};

class Pixel {
public:
	Pixel() {}
	Pixel(Object _obj, std::string _str = "  "): m_object(_obj), m_string(_str) {}

	friend std::ostream& operator<<(std::ostream& out, const Pixel& px);
	
	Object      getObject() const { return m_object; }
	std::string getString() const { return m_string; }

	void setObject(Object _obj);
	void setString(std::string _str);

	static std::map<Object, colour_t> gs_defaultColours; // Цвета объектов по дефолту

private:
    Object      m_object{ Object::FLOOR };   // Тип объекта
	std::string m_string{ "  " }; // Текст, написанный на пикселе
};

using addObj = std::pair<Object, colour_t>;

std::map<Object, colour_t> Pixel::gs_defaultColours{
	addObj(Object::FLOOR,  colour(Colour::WHITE,   Colour::BLACK)),
	addObj(Object::EDGE,   colour(Colour::DR_GREY, Colour::BLACK)),
	addObj(Object::WALL,   colour(Colour::BLACK,   Colour::DR_GREY)),
	addObj(Object::PLAYER, colour(Colour::VIOLET,  Colour::RED)),
	addObj(Object::FRIEND, colour(Colour::PINK,    Colour::RED)),
	addObj(Object::DOOR,   colour(Colour::BLUE,    Colour::RED)),
};

void Pixel::setString(std::string _str) {
	_str.resize(2, ' ');
	m_string = _str;
}

void Pixel::setObject(Object _obj) {
	m_object = _obj;
}

std::ostream& operator<<(std::ostream& out, const Pixel& px) {
	PrintColour(px.m_string, Pixel::gs_defaultColours[px.m_object]);
	return out;
}

class Field {
public:
	friend Subject;

	Field(int _rows = 10, int _cols = 10);

	void resize(int _rows, int _cols);  // изменить размер поля
	void clear(Object _wall);           // очистить поле и заполнить одним объектом
	void print() const;                 // вывести поле по пикселю
	void setBorder(Object _floor);      // установить границы из выбранного объекта

	std::vector<Pixel>& operator[](unsigned ind);
	Pixel& operator[](const Point& _place);

private:
	int m_rows{ 10 };
	int m_cols{ 10 };
	std::vector<std::vector<Pixel>> m_field;
};

Pixel& Field::operator[](const Point& _place) {
	return m_field[_place.x][_place.y];
}

std::vector<Pixel>& Field::operator[](unsigned ind) {
	if (ind > m_field.size()) throw std::exception("Index is bigger than size of m_field");
	return m_field[ind];
}

void Field::setBorder(Object _wall) {
	for (int i = 0; i < m_rows; i++) {
		if (i == 0 || i == m_rows - 1) {
			m_field[i].assign(m_cols, Pixel(_wall));
			continue;
		}
		m_field[i][0] = m_field[i][m_cols-1] = Pixel(_wall);
	}
}

void Field::print() const{
	for (const std::vector<Pixel>& line : m_field) {
		for (const Pixel& el : line) {
			std::cout << el;
		}
		std::cout << '\n';
	}
}

void Field::resize(int _rows, int _cols) {
	m_field.resize(_rows);
	for (std::vector<Pixel>& line : m_field)
		line.resize(_cols, Pixel(Object::FLOOR));
	m_rows = _rows;
	m_cols = _cols;
}

void Field::clear(Object _floor) {
	for (std::vector<Pixel>& line : m_field) {
		for (Pixel& el : line) {
			el.setObject(_floor);
		}
	}
}

Field::Field(int _rows, int _cols) : m_rows(_rows), m_cols(_cols) {
	resize(_rows, _cols);
	clear(Object::FLOOR);
}

class Subject {
public:
	Point m_place{ 0, 0 };

	Subject(Object _object): m_object(_object) {}
	Subject(Object _object, Point _place, Field* _fieldPtr);

	void refTo(Field* _field);
	void moveTo(Point _place) noexcept(false);

private:
	Field* m_fieldPtr { nullptr };
	Object m_object   { Object::PLAYER };
	Object m_standOn  { Object::MAX_OBJ };
};

void Subject::moveTo(Point _place) {
	if (!m_fieldPtr) 
		throw std::exception("Subject hasn't Field pointer");
	if (_place.x < 0 || _place.x >= m_fieldPtr->m_rows || _place.y < 0 || _place.y >= m_fieldPtr->m_cols)
		return;
	if ((*m_fieldPtr)[_place].getObject() != Object::FLOOR)
		return;

	Object temp = (*m_fieldPtr)[_place].getObject();
	(*m_fieldPtr)[_place].setObject(m_object);
	
	if (m_standOn != Object::MAX_OBJ)
		(*m_fieldPtr)[m_place].setObject(m_standOn);

	m_standOn = temp;

	m_place = _place;
}

void Subject::refTo(Field* _field) {
	if (m_fieldPtr && m_standOn != Object::MAX_OBJ) {
		(*m_fieldPtr)[m_place].setObject(m_standOn);
		m_standOn = Object::MAX_OBJ;
	}
	m_fieldPtr = _field;
}

Subject::Subject(Object _object, Point _place, Field* _fieldPtr) : m_place(_place), m_object(_object) {
	m_fieldPtr = _fieldPtr;
	moveTo(_place);
}

BOOL ShowConsoleCursor(BOOL bShow)
{
	CONSOLE_CURSOR_INFO cci;
	HANDLE hStdOut;
	hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
	if (hStdOut == INVALID_HANDLE_VALUE)
		return FALSE;
	if (!GetConsoleCursorInfo(hStdOut, &cci))
		return FALSE;
	cci.bVisible = bShow;
	if (!SetConsoleCursorInfo(hStdOut, &cci))
		return FALSE;
	return TRUE;
}

int main()
{
	//
	HWND hwnd = GetConsoleWindow();
	ShowWindow(hwnd, SW_SHOWMAXIMIZED); // Показываем окно консоли на полный экран
	SetConsoleCP(1251);
	SetConsoleOutputCP(1251);
	srand((unsigned)time(0));
	//

	Field f(15, 25);
	f.setBorder(Object::EDGE);

	Subject player(Object::PLAYER, {1, 1}, &f);
	int& x = player.m_place.x;
	int& y = player.m_place.y;
	
	char move;

	ShowConsoleCursor(FALSE);

	while (true) {
		HANDLE cons = GetStdHandle(STD_OUTPUT_HANDLE);
		SetConsoleCursorPosition(cons, { 0, 0 });

		f.print();

		move = _getch();
		
		switch (move) {
		case 72: player.moveTo({ x - 1, y }); break;
		case 80: player.moveTo({ x + 1, y }); break;
		case 77: player.moveTo({ x, y + 1 }); break;
		case 75: player.moveTo({ x, y - 1 }); break;
		}
	}

	/*
	
	if (cons != INVALID_HANDLE_VALUE)
	{
		CONSOLE_SCREEN_BUFFER_INFO old;
		GetConsoleScreenBufferInfo(cons, &old);
		SetConsoleCursorPosition(cons, { 0, 0 });
		std::cout << "ABOBA... A.. AMOGUS...";
		SetConsoleCursorPosition(cons, old.dwCursorPosition);
	}
	*/
}
