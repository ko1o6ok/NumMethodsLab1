#include "library.h"

#include <cmath>
#include <fstream>
#include <utility>
#include <iostream>


using namespace std;


// du/dx = f_test(u)
double f_test(double x, double u) {
    return -0.5 * u;
}
// Аналитическое решение тестовой задачи
double anal_sol_test(double x,double u0){
    return u0 * exp(-0.5 * x);
}
// du/dx = f_main_1(x,u)
double f_main_1(double x, double u) {
    return ((1 / (1 + x*x*x*x)) * u*u + u - u*u*u * sin(10 * x));
}
// Есть параметр a
// Вектор (u,s)
// Производная вектора:
// u' = s
// s' = -a*sin(u)
pair<double,double> f_main_2(pair<double,double> u, double a) {
    return {u.second,-a * sin(u.first)};
}
// Шаг метода РК4
// Для du/dx = f(x,u) - тестовая и основная задача 1
void rK_step(double (*f)(double, double),double& x,double& u,double step){
    double k1, k2, k3, k4;

    k1 = f(x, u);
    k2 = f(x + 0.5 * step, u + 0.5 * step * k1);
    k3 = f(x + 0.5 * step, u + 0.5 * step * k2);
    k4 = f(x + step, u + step * k3);

    u = u + (step / 6) * (k1 + 2 * k2 + 2 * k3 + k4);
    x = x + step;
}

// Шаг метода РК4 для двумерного вектора
// Для основной задачи 2
void rK_step(pair<double,double> (*f)(pair<double,double>, double),double& x,pair<double,double>& u,double step,double a){
    pair<double,double> k1, k2, k3, k4;
    k1 = f(u,a);
    k2 = f({u.first + step/2 * k1.first,u.second + step/2 * k1.second},a);
    k3 = f({u.first + step/2 * k2.first,u.second + step/2 * k2.second},a);
    k4 = f({u.first + step * k3.first,u.second + step * k3.second},a);

//    auto t1 = f(u,a);
//
//    k1 =  {t1.first,t1.second};
//
//    pair<double,double> t2 = {u.first + 0.5 * step * k1.first,u.second + 0.5 *step* k1.second};
//
//    auto t3 = f(t2,a);
//
//    k2 = {step * t3.first, step * t3.second};
//
//    pair<double,double> t4 = {u.first + 0.5 * k2.first,u.second + 0.5 * k2.second};
//
//    auto t5 = f(t4,a);
//
//    k3 = {step * t5.first,step * t5.second};
//
//    pair<double,double> t6 = {u.first + step * k3.first,u.second + step * k3.second};
//
//    auto t7 = f(t6,a);
//
//    k4 = {step * t7.first,step * t7.second};
//
    u = {u.first + (step / 6) * (k1.first + 2 * k2.first + 2 * k3.first + k4.first), u.second + (step / 6) * (k1.second + 2 * k2.second + 2 * k3.second + k4.second)};

    x = x + step;
}

// Проверка выхода на правую границу
bool inside(double x,double b,double eps_b){
    return x < b - eps_b;
}
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
extern "C" __declspec(dllexport) void run_test_method(double u0, int Nmax,double b, double eps_b, double eps, double step){
    // x0 = 0

    //std::ofstream file_1(R"(C:\C++_proj\NM\NumMethodsLab1\NM1\test_method_1.txt)"); // Файл с данными для таблицы
    //std::ofstream file_2(R"(C:\C++_proj\NM\NumMethodsLab1\NM1\test_method_2.txt)"); // Файл с выходными данными

    std::ofstream file_1("test_method_1.txt"); // Файл с данными для таблицы
    std::ofstream file_2("test_method_2.txt"); // Файл с выходными данными

    double v = u0;
    double x = 0.0;
    double h = step;

    double x_help,v_help; // Координаты вспомогательной точки численной траектории
    double x_current,v_current; // Текущее положение
    double S = 10000; // Параметр оценки локальной погрешности

    int C1 = 0; // Счётчик деления шага
    int C2 = 0; // Счётчик удвоений шага

    double n=0; // Будет считать число итераций
    double max_OLP = -1; // Макс модуль ОЛП
    double OLP;
    double max_step = -1; // Максимальный шаг
    double x_max_step=0; // Соотв коорд
    double min_step = h; // Минимальный шаг
    double x_min_step=0; // Соотв коорд
    double max_diff = -1; // Макс модуль разности числ и анал реш
    double x_max_diff = 0; // Координата макс. модуля разности с числ. реш
    double diff; // Разность числ и анал реш в точке
    double sol; // Анал реш в точке
    for (int i = 0; (i < Nmax) && inside(x,b,eps_b); ++i) {
        if(h > max_step){
            max_step = h;
            x_max_step = x;
        }
        if(h < min_step){
            min_step = h;
            x_min_step = x;
        }
        // Находимся в точке (x_n,v_n)
        // Проверим, что сделав шаг h мы не вылетим за границу
        while((x+h)>b){
            h = h/2.0; // Делим пополам
            ++C1;
        }

        // Текущие координаты численной траектории
        x_current = x;
        v_current = v;
        x_help = x_current;
        v_help = v_current;
        // Перейдём во вспомогательную точку половинным шагом
        rK_step(f_test,x_help,v_help,h/2);
        // Получим новую точку ЧТ тем же половинным шагом
        rK_step(f_test,x_help,v_help,h/2);

        // Теперь считаем эту же точку с шагом h
        rK_step(f_test,x_current,v_current,h);

        // Вычисляем
        S = std::abs(v_help - v_current) / 15.0;
        OLP = 16 * S; // Оценка локальной погрешности
        if(OLP > max_OLP)
            max_OLP = OLP;
        if(S <= eps){
            // Принимаем следующую точку
            x = x_current;
            v = v_current;
            sol = anal_sol_test(x,u0); // Аналит. реш в этой точке
            diff =  std::abs(sol-v);
            if(diff > max_diff){
                max_diff = diff;
                x_max_diff = x;
            }

            file_1 << (i + 1) << " " << x << " " << v << " " << v_help<< " " <<v-v_help<< " " <<16*S<< " " <<h << " " <<C1<< " " <<C2<< " "<<sol<< " "<< diff <<"\n";
            if(S < eps/32){
                // Продолжаем счёт с удвоенным шагом
                h = 2 * h; // Удвоили шаг
                C2 ++;
            }
        }
        else {
            // Новая точка не принимается
            h = h / 2; // Шаг в 2 раза меньше
            C1++;
        }
        n = i + 1;
    }
    file_1.close();
    file_2 << n << " " << b-x << " " << max_OLP << " " << C2 << " " << C1 << " " << max_step << " " << x_max_step << " " << min_step << " " << x_min_step << " " << max_diff<<" "<<x_max_diff ;
    file_2.close();
}
// Тот же метод с постоянным шагом
extern "C" __declspec(dllexport) void run_test_method_const_step(double u0, int Nmax,double b, double eps_b, double eps, double step){
    // x0 = 0

    //std::ofstream file_1(R"(C:\C++_proj\NM\NumMethodsLab1\NM1\test_method_1.txt)"); // Файл с данными для таблицы
    //std::ofstream file_2(R"(C:\C++_proj\NM\NumMethodsLab1\NM1\test_method_2.txt)"); // Файл с выходными данными

    std::ofstream file_1("test_method_1_const_step.txt"); // Файл с данными для таблицы
    std::ofstream file_2("test_method_2_const_step.txt"); // Файл с выходными данными

    double v = u0;
    double x = 0.0;
    double h = step;

    double x_help,v_help; // Координаты вспомогательной точки численной траектории
    double x_current,v_current; // Текущее положение
    double S = 10000; // Параметр оценки локальной погрешности

    int C1 = 0; // Счётчик деления шага
    int C2 = 0; // Счётчик удвоений шага

    double n=0; // Будет считать число итераций
    double max_OLP = -1; // Макс модуль ОЛП
    double OLP;
    double max_step = -1; // Максимальный шаг
    double x_max_step=0; // Соотв коорд
    double min_step = h; // Минимальный шаг
    double x_min_step=0; // Соотв коорд
    double max_diff = -1; // Макс модуль разности числ и анал реш
    double x_max_diff = 0.0; // Координата макс. модуля разности
    double diff; // Разность числ и анал реш в точке
    double sol; // Анал реш в точке
    for (int i = 0; (i < Nmax) && inside(x+h,b,eps_b); ++i) {
        if(h > max_step){
            max_step = h;
            x_max_step = x;
        }
        if(h < min_step){
            min_step = h;
            x_min_step = x;
        }
        // Находимся в точке (x_n,v_n)

        // Текущие координаты численной траектории
        x_current = x;
        v_current = v;

        // Теперь считаем эту точку с шагом h
        rK_step(f_test,x_current,v_current,h);


        // Всегда принимаем следующую точку
        x = x_current;
        v = v_current;
        sol = anal_sol_test(x,u0); // Аналит. реш в этой точке
        diff =  std::abs(sol-v);
        if(diff > max_diff)
            max_diff = diff;
        file_1 << (i + 1) << " " << x << " " << v << " " <<h << " " <<C1<< " " <<C2<< " "<<sol<< " "<< diff <<"\n";


        n = i + 1;
    }
    file_1.close();
    file_2 << n << " " << b-x << " " << C2 << " " << C1 << " " << max_step << " " << x_max_step << " " << min_step << " " << x_min_step << " " << max_diff<<" "<<x_max_diff ;
    file_2.close();
}

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

extern "C" __declspec(dllexport) void run_main_method_1(double u0, int Nmax,double b, double eps_b, double eps, double step){
    // x0 = 0

    std::ofstream file_1("main_method_1_1.txt"); // Файл с данными для таблицы
    std::ofstream file_2("main_method_1_2.txt"); // Файл с выходными данными


    double v = u0;
    double x = 0.0;
    double h = step;

    double x_help,v_help; // Координаты вспомогательной точки численной траектории
    double x_current,v_current; // Текущее положение
    double S = 10000; // Параметр оценки локальной погрешности

    int C1 = 0; // Счётчик деления шага
    int C2 = 0; // Счётчик удвоений шага

    double n=0; // Будет считать число итераций
    double max_OLP = -1; // Макс модуль ОЛП
    double OLP;
    double max_step = -1; // Максимальный шаг
    double x_max_step; // Соотв коорд
    double min_step = h; // Минимальный шаг
    double x_min_step=0; // Соотв коорд

    for (int i = 0; (i < Nmax) && inside(x,b,eps_b); ++i) {
        if(h > max_step){
            max_step = h;
            x_max_step = x;
        }
        if(h < min_step){
            min_step = h;
            x_min_step = x;
        }
        // Находимся в точке (x_n,v_n)
        // Проверим, что сделав шаг h мы не вылетим за границу
        while((x+h)>b){
            h = h/2.0; // Делим пополам
            ++C1;
        }
        // Текущие координаты численной траектории
        x_current = x;
        v_current = v;
        x_help = x_current;
        v_help = v_current;
        // Перейдём во вспомогательную точку половинным шагом
        rK_step(f_main_1,x_help,v_help,h/2);
        // Получим новую точку ЧТ тем же половинным шагом
        rK_step(f_main_1,x_help,v_help,h/2);

        // Теперь считаем эту же точку с шагом h
        rK_step(f_test,x_current,v_current,h);

        // Вычисляем
        S = std::abs((v_help - v_current) / 15.0);
        OLP = 16 * S; // Оценка локальной погрешности
        if(OLP > max_OLP)
            max_OLP = OLP;
        if(S <= eps){
            // Принимаем следующую точку
            x = x_current;
            v = v_current;

            file_1 << (i + 1) << " " << x << " " << v << " " << v_help<< " " <<v-v_help<< " " <<16*S<< " " <<h << " " <<C1<< " " <<C2 <<"\n";
            if(S < eps/32){
                // Продолжаем счёт с удвоенным шагом
                h = 2 * h; // Удвоили шаг
                C2 ++;
            }
        }
        else {
            // Новая точка не принимается
            h = h / 2; // Шаг в 2 раза меньше
            C1++;
        }
        n = i + 1;
    }
    file_1.close();
    file_2 << n << " " << b-x << " " << max_OLP << " " << C2 << " " << C1 << " " << max_step << " " << x_max_step << " " << min_step << " " << x_min_step ;
    file_2.close();
}
// Версия с постоянным шагом
extern "C" __declspec(dllexport) void run_main_method_1_const_step(double u0, int Nmax,double b, double eps_b, double eps, double step){
    // x0 = 0

    std::ofstream file_1("main_method_1_1_const_step.txt"); // Файл с данными для таблицы
    std::ofstream file_2("main_method_1_2_const_step.txt"); // Файл с выходными данными


    double v = u0;
    double x = 0.0;
    double h = step;

    double x_help,v_help; // Координаты вспомогательной точки численной траектории
    double x_current,v_current; // Текущее положение
    double S = 10000; // Параметр оценки локальной погрешности

    int C1 = 0; // Счётчик деления шага
    int C2 = 0; // Счётчик удвоений шага

    double n=0; // Будет считать число итераций
    double max_OLP = -1; // Макс модуль ОЛП
    double OLP;
    double max_step = -1; // Максимальный шаг
    double x_max_step; // Соотв коорд
    double min_step = h; // Минимальный шаг
    double x_min_step=0; // Соотв коорд

    for (int i = 0; (i < Nmax) && inside(x+h,b,eps_b); ++i) {
        if(h > max_step){
            max_step = h;
            x_max_step = x;
        }
        if(h < min_step){
            min_step = h;
            x_min_step = x;
        }
        // Находимся в точке (x_n,v_n)
        // Текущие координаты численной траектории
        x_current = x;
        v_current = v;

        // Теперь считаем эту точку с шагом h
        rK_step(f_test,x_current,v_current,h);

        // Принимаем следующую точку
        x = x_current;
        v = v_current;

        file_1 << (i + 1) << " " << x << " " << v << " " <<h << " " <<C1<< " " <<C2 <<"\n";
        n = i + 1;
    }
    file_1.close();
    file_2 << n << " " << b-x  << " " << C2 << " " << C1 << " " << max_step << " " << x_max_step << " " << min_step << " " << x_min_step ;
    file_2.close();
}
// Евклидова норма
double euclid_norm(pair<double,double> v1, pair<double,double> v2){
    double d1 = v1.first-v2.first;
    double d2 = v1.second-v2.second;
    return sqrt(d1*d1 + d2*d2);
}
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

extern "C" __declspec(dllexport) void run_main_method_2(double u0,double u0_dot, int Nmax,double b, double eps_b, double eps, double step,double a){
    // x0 = 0

    std::ofstream file_1("main_method_2_1_v.txt"); // Файл с данными для таблицы
    std::ofstream file_2("main_method_2_2.txt"); // Файл с выходными данными
    std::ofstream file_3("main_method_2_1_v_dot.txt"); // Файл с данными для таблицы

    pair<double,double> v = {u0,u0_dot}; // Теперь мы итерируем вектор
    double x = 0.0;
    double h = step;

    double x_help; // Координаты вспомогательной точки численной траектории
    pair<double,double> v_help,v_current;
    double x_current; // Текущее положение
    double S = 10000; // Параметр оценки локальной погрешности

    int C1 = 0; // Счётчик деления шага
    int C2 = 0; // Счётчик удвоений шага

    double n=0; // Будет считать число итераций
    double max_OLP = -1; // Макс модуль ОЛП
    double OLP;
    double max_step = -1; // Максимальный шаг
    double x_max_step; // Соотв коорд
    double min_step = h; // Минимальный шаг
    double x_min_step=0; // Соотв коорд

    for (int i = 0; (i < Nmax) && inside(x,b,eps_b); ++i) {
        if(h > max_step){
            max_step = h;
            x_max_step = x;
        }
        if(h < min_step){
            min_step = h;
            x_min_step = x;
        }
        // Находимся в точке (x_n,v_n)
        // Проверим, что сделав шаг h мы не вылетим за границу
        while((x+h)>b){
            h = h/2.0; // Делим пополам
            ++C1;
        }
        // Текущие координаты численной траектории
        x_current = x;
        v_current = v;

        x_help = x_current;
        v_help = v_current;
        // Перейдём во вспомогательную точку половинным шагом
        rK_step(f_main_2,x_help,v_help,h/2,a);
        // Получим новую точку ЧТ тем же половинным шагом
        rK_step(f_main_2,x_help,v_help,h/2,a);

        // Теперь считаем эту же точку с шагом h
        rK_step(f_main_2,x_current,v_current,h,a);

        // Вычисляем
        S = euclid_norm(v_help,v_current)/ 15.0;
        OLP = 16 * S; // Оценка локальной погрешности
        if(OLP > max_OLP)
            max_OLP = OLP;
        if(S <= eps){
            // Принимаем следующую точку
            x = x_current;
            v = v_current;
            // Здесь не как в таблице сделано - добавлена производная u' = v.second
            file_1 << (i + 1) << " " << x << " " << v.first << " " << v_help.first<< " " <<v.first-v_help.first<< " " <<16*S<< " " <<h << " " <<C1<< " " <<C2 <<"\n";
            file_3 << (i + 1) << " " << x << " " << v.second << " " << v_help.second<< " " <<v.second-v_help.second<< " " <<16*S<< " " <<h << " " <<C1<< " " <<C2 <<"\n";
            if(S < eps/32){
                // Продолжаем счёт с удвоенным шагом
                h = 2 * h; // Удвоили шаг
                C2 ++;
            }
        }
        else {
            // Новая точка не принимается
            h = h / 2; // Шаг в 2 раза меньше
            C1++;
        }
        n = i + 1;
    }
    file_3.close();
    file_1.close();
    file_2 << n << " " << b-x << " " << max_OLP << " " << C2 << " " << C1 << " " << max_step << " " << x_max_step << " " << min_step << " " << x_min_step ;
    file_2.close();
}

extern "C" __declspec(dllexport) void run_main_method_2_const_step(double u0,double u0_dot, int Nmax,double b, double eps_b, double eps, double step,double a){
    // x0 = 0

    std::ofstream file_1("main_method_2_1_const_step_v.txt"); // Файл с данными для таблицы
    std::ofstream file_2("main_method_2_2_const_step.txt"); // Файл с выходными данными
    std::ofstream file_3("main_method_2_1_const_step_v_dot.txt"); // Файл с данными для таблицы

    pair<double,double> v = {u0,u0_dot}; // Теперь мы итерируем вектор
    double x = 0.0;
    double h = step;

    double x_help; // Координаты вспомогательной точки численной траектории
    pair<double,double> v_help,v_current;
    double x_current; // Текущее положение
    double S = 10000; // Параметр оценки локальной погрешности

    int C1 = 0; // Счётчик деления шага
    int C2 = 0; // Счётчик удвоений шага

    double n=0; // Будет считать число итераций
    double max_OLP = -1; // Макс модуль ОЛП
    double OLP;
    double max_step = -1; // Максимальный шаг
    double x_max_step; // Соотв коорд
    double min_step = h; // Минимальный шаг
    double x_min_step=0; // Соотв коорд

    for (int i = 0; i < Nmax && inside(x+h,b,eps_b); ++i) {
        if(h > max_step){
            max_step = h;
            x_max_step = x;
        }
        if(h < min_step){
            min_step = h;
            x_min_step = x;
        }
        // Находимся в точке (x_n,v_n)
        // Текущие координаты численной траектории
        x_current = x;
        v_current = v;


        // Теперь считаем эту точку с шагом h
        rK_step(f_main_2,x_current,v_current,h,a);


        // Принимаем следующую точку
        x = x_current;
        v = v_current;
        // Здесь не как в таблице сделано - добавлена производная u' = v.second
        file_1 << (i + 1) << " " << x << " " << v.first << " " <<h << " " <<C1<< " " <<C2 <<"\n";
        file_3 << (i + 1) << " " << x << " " << v.second<< " " <<h << " " <<C1<< " " <<C2 <<"\n";
        n = i + 1;
    }
    file_3.close();
    file_1.close();
    file_2 << n << " " << b-x  << " " << C2 << " " << C1 << " " << max_step << " " << x_max_step << " " << min_step << " " << x_min_step ;
    file_2.close();
}
