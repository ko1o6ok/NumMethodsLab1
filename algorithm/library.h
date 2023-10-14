#ifndef NM1_LIB_LIBRARY_H
#define NM1_LIB_LIBRARY_H


#include <utility>

double f_test(double x, double u); // Функция для тестовой задачи

double anal_sol_test(double x,double u0); // Аналитическое решение тестовой задачи

double f_main_1(double x, double u); // Функция для основной задачи-1

std::pair<double,double> f_main_2(std::pair<double,double> u, double a); // Функция для основной задачи-2

void rK_step(double (*f)(double, double),double& x,double& u,double step); // Шаг метода РК4

// Шаг метода РК4 для двумерной системы
void rK_step(std::pair<double,double> (*f)(std::pair<double,double>, double),double& x,std::pair<double,double>& u,double step,double a);

// Проверка выхода на правую границу
bool inside(double x,double b,double eps_b);

// Метод для тестовой задачи
// -------------------------
// u0 - начальное значение
// Nmax - макс. число шагов
// b - правая граница
// eps_b - контроль выхода на правую границу
// eps - контроль лок. погрешности
// step - начальный шаг
// ------------------------
// В файл-1 уходит:
// - точное + приближённое решение + то, что нужно для таблицы (удвоения, ОЛП и прочее)
// В файл-2 уходит:
// - выходные данные:
// - - n - число проделанных итераций
// - - b-x_n - отход от границы
// - - max|ОЛП|
// - - всего удвоений шага
// - - всего делений шага
// - - max h = "" при x = ""
// - - min h= "" при x = ""
// - - max|u_i-v_i| = "" при x = ""
extern "C" __declspec(dllexport) void run_test_method(double u0, int Nmax,double b, double eps_b, double eps, double step);
extern "C" __declspec(dllexport) void run_test_method_const_step(double u0, int Nmax,double b, double eps_b, double eps, double step);

// Метод для основной задачи - 1
// -------------------------
// u0 - начальное значение
// Nmax - макс. число шагов
// b - правая граница
// eps_b - контроль выхода на правую границу
// eps - контроль лок. погрешности
// step - начальный шаг
// ------------------------
// В файл-1 уходит:
// - приближённое решение + то, что нужно для таблицы (удвоения, ОЛП и прочее)
// В файл-2 уходит:
// - выходные данные:
// - - n - число проделанных итераций
// - - b-x_n - отход от границы
// - - max|ОЛП|
// - - всего удвоений шага
// - - всего делений шага
// - - max h = "" при x = ""
// - - min h= "" при x = ""

extern "C" __declspec(dllexport) void run_main_method_1(double u0, int Nmax,double b, double eps_b, double eps, double step);
extern "C" __declspec(dllexport) void run_main_method_1_const_step(double u0, int Nmax,double b, double eps_b, double eps, double step);

// Евклидова норма
double euclid_norm(std::pair<double,double> v1, std::pair<double,double> v2);

// Разница векторов (модули координат)
std::pair<double,double> diff(std::pair<double,double> v1, std::pair<double,double> v2);
// Метод для основной задачи - 2
// -------------------------
// (u0,u0_dot) - начальные значения
// Nmax - макс. число шагов
// b - правая граница
// eps_b - контроль выхода на правую границу
// eps - контроль лок. погрешности
// step - начальный шаг
// a - параметр в уравнении
// ------------------------
// В файл-1 уходит:
// - приближённое решение + то, что нужно для таблицы (удвоения, ОЛП и прочее)
// В файл-2 уходит:
// - выходные данные:
// - - n - число проделанных итераций
// - - b-x_n - отход от границы
// - - max|ОЛП|
// - - всего удвоений шага
// - - всего делений шага
// - - max h = "" при x = ""
// - - min h= "" при x = ""

extern "C" __declspec(dllexport) void run_main_method_2(double u0,double u0_dot, int Nmax,double b, double eps_b, double eps, double step,double a);
extern "C" __declspec(dllexport) void run_main_method_2_const_step(double u0,double u0_dot, int Nmax,double b, double eps_b, double eps, double step,double a);
#endif //NM1_LIB_LIBRARY_H
