## Клиент-серверная система для построения выпуклой оболочки

## Описание
Система вычисляет выпуклую оболочку множества точек методом Джарвиса, состоящая из:
- Сервера на C++ (обработка запросов)
- Клиента на C++ (отправка данных)
- Тестера на C++ (массовая проверка)
- Визуализатора на Python (графическое представление)

## Требования
- ОС: Windows/Linux
- Компилятор C++17
- Python 3.8+
- Библиотеки: Winsock2 (Windows), matplotlib

## Установка
```bash
# Сборка компонентов
g++ -std=c++17 jarvis_server.cpp -o server -lws2_32
g++ -std=c++17 jarvis_client.cpp -o client -lws2_32
g++ -std=c++17 jarvis_mass_test.cpp -o tester -lws2_32

/Project
│── jarvis_hull.h       # Алгоритм Джарвиса
│── jarvis_server.cpp   # Сервер
│── jarvis_client.cpp   # Клиент
│── jarvis_mass_test.cpp # Тестер
│── jarvis_visualize.py # Визуализатор
└── jarvis_tests.log    # Логи тестов

> Сервер:
Server started on port 9090

> Клиент:
Введите точки: 0,0 1,1 2,2 1,0
Результат: 0,0 2,2 1,0

> Тестер:
Tests passed: 100/100 (100%)

> Визуализатор:
Generated 100 test images
