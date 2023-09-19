#include <iostream>
#include <cmath>
#include <utility>

#define pi 3.1415926535
#define g 9.80665

double f(double x, double u, double alpha, double sigma) {
    return -0.6 * sigma * sqrt(2 * g) * std::pow(tan(0.5 * alpha), -2) * std::pow(pi, -1) * std::pow(u, -1.5);
}

//TODO: расширить переменную u на векторный тип
// Метод Рунге-Кутта 4-го порядка
std::pair<double, double> rungeKutta(double u0, double x0, double alpha, double sigma, double h, double eps_border,double b) {
////     (u0,x0) - нач. точка
////     h - фикс. шаг метода
////     b - координата правой границы
////     eps_border - параметр контроля выхода на правую границу
    double k1, k2, k3, k4;
    double u = u0;
    double x = x0;

    // Как условие записан выход на правую границу
    while ((b-x) > eps_border) {

        k1 = h * f(x, u, alpha, sigma);
        k2 = h * f(x + 0.5 * h, u + 0.5 * k1, alpha, sigma);
        k3 = h * f(x + 0.5 * h, u + 0.5 * k2, alpha, sigma);
        k4 = h * f(x + h, u + h * k3, alpha, sigma);

        u = u + (h / 6) * (k1 + 2 * k2 + 2 * k3 + k4);
        x = x + h;
        // Тут будет запись в файл
        std::cout << x << " " << u << " " << k1 << " " << k2 << " " << k3 << " " << k4 << std::endl;
    }
    return {x, u};
}

//TODO: расширить переменную u на векторный тип, добавить контроль погрешности
// Метод Рунге-Кутта 4-го порядка с контролем локальной погрешности
std::pair<double, double> rungeKutta_control(double u0, double x0, double alpha, double sigma, double h, double eps_border,double b, double eps) {
////     (u0,x0) - нач. точка
////     h - фикс. шаг метода
////     b - координата правой границы
////     eps_border - параметр контроля выхода на правую границу
////     eps - параметр контроля локальной погрешности
    double k1, k2, k3, k4;
    double u = u0;
    double x = x0;

    while ((b-(x+h)) > eps_border) {

        k1 = h * f(x, u, alpha, sigma);
        k2 = h * f(x + 0.5 * h, u + 0.5 * k1, alpha, sigma);
        k3 = h * f(x + 0.5 * h, u + 0.5 * k2, alpha, sigma);
        k4 = h * f(x + h, u + h * k3, alpha, sigma);

        u = u + (h / 6) * (k1 + 2 * k2 + 2 * k3 + k4);
        x = x + h;
        // Тут будет запись в файл
        std::cout << x << " " << u << " " << k1 << " " << k2 << " " << k3 << " " << k4 << std::endl;
    }
    return {x, u};
}

int main() {
    double alpha = 30.0;
    double sigma = 2250.0;
    double u0 = 0.1;
    double x0 = 0.0;
    double h = 1e-6;

    //std::pair<double, double> result = rungeKutta(u0, x0, alpha, sigma, h);

    //std::cout << result.first << ", " << result.second << std::endl;

    return 0;
}
