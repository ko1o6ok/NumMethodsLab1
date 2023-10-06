import matplotlib.pyplot as plt
from PyQt5 import uic
from PyQt5.QtWidgets import QMainWindow, QTableWidgetItem
from matplotlib.backends.backend_qt5agg import FigureCanvasQTAgg as FigureCanvas, \
    NavigationToolbar2QT as NavigationToolbar
from matplotlib.figure import Figure

import ctypes
import os

ui_file = './UI/MainWindow.ui'

columns = {
    0: ["номер итерации:\ni", "Координата:\nX_i", "Числ. решение:\nV_i", "Числ. решение двойным шагом\nV_2i",
        "V_i-V_2i", "Оценка лок. погр.", "h_i", "Счётчик делений шага:\nC1", "Счётчик удвоений шага:\nC2",
        "Аналит. решение:\n U_i", "|U_i-V_i|"],
    1: ["Номер итерации:\ni", "Координата: \n X_i", "Числ. решение:\nV_i", "Числ. решение двойным шагом V_2i",
        "Разность:\nv_i-v_2i", "Оценка лок погр:\nОЛП", "Шаг:\nh_i", "Делений шага:\nC1", "Удвоений шага:\nC2"],
    2: ["Номер итерации:\ni", "Координата:\nX_i", "Числ. решение:\nV_i", "Числ. решение(производная):\nv’_i",
        "Числ. решение двойным шагом:\nv_2i", "Разность:\nV_i-V_2i", "Оценка лок погр:\nОЛП", "Шаг:\nh_i",
        "Делений шага:\nC1", "Удвоений шага\nC2"]
}


def create_plot(parent):
    parent.fig = Figure(figsize=(parent.width() / 100, parent.height() / 100))
    parent.canvas = FigureCanvas(parent.fig)
    parent.plot = parent.fig.add_subplot()
    return parent.plot


class UI_mainWindow(QMainWindow):
    def __init__(self):
        super(UI_mainWindow, self).__init__()
        uic.loadUi(ui_file, self)

        # создание окон для графиков
        self.plt = create_plot(self.plot_widget_1)
        self.plt_PS = create_plot(self.plot_widget_2)

        # присвоение мест для окон
        self.plot_widget_1.canvas.setParent(self.plot_widget_1)
        self.plot_widget_2.canvas.setParent(self.plot_widget_2)

        self.tabWidget.currentChanged.connect(
            self.toolBar_changing)  # задание функционала. В данной строке: Меняет тулбар при переходе на другую вклвдку
        self.plot_toolBar = NavigationToolbar(self.plot_widget_1.canvas, self)

        self.addToolBar(self.plot_toolBar)  # создание тулбара

        self.plot_button.clicked.connect(
            self.plotting)  # задание функционала. В данной строке: построение графика при нажатии на кнопку "Построить"
        self.delete_plot.clicked.connect(
            self.clear_plots)  # задание функционала. В данной строке: очистка окон от ВСЕХ графиков (чистит все окна(графики и таблицу))

    def clear_plots(self):
        self.plt.cla()
        self.plt_PS.cla()
        self.plot_widget_1.canvas.draw()  # обновление окна
        self.plot_widget_2.canvas.draw()
        self.clear_table()
        self.update_extra_info_table(0,[["0"]*11])


    def toolBar_changing(self, index):  # изменение привязки тулбара
        self.removeToolBar(self.plot_toolBar)
        if index == 0:  # тулбал для вкладки График
            self.plot_toolBar = NavigationToolbar(self.plot_widget_1.canvas, self)
        elif index == 2:  # тулбар для вкладки График ФП
            self.plot_toolBar = NavigationToolbar(self.plot_widget_2.canvas, self)
        self.addToolBar(self.plot_toolBar)

    def file_to_table(self, file_name):# из str делает list(list(str))
        if len(file_name.split('.')) == 1:
            file_name += '.txt'
        table = []
        with open(file_name, 'r') as f:
            for line in f:
                table.append(line.split(' '))
        return table

    def update_extra_info_table(self, task_index, table):
        table = table[0]
        # Почему такие индексы? см.: /help/spetsifikatsia_tablitsa.docx
        self.iterations.setText(table[0])
        self.border_error.setText(table[1])
        self.max_error.setText(table[2])
        self.step_doubling_counter.setText(table[3])
        self.step_division_counter.setText(table[4])
        self.max_step.setText(table[5])
        self.max_step_x.setText(table[6])
        self.min_step.setText(table[7])
        self.min_step_x.setText(table[8])
        if task_index == 0:  # для тестовой задачи еще есть параметр максимальной разности ан. и числ. решений
            self.max_anal_diff.setText(table[9])
            self.max_anal_diff_x.setText(table[10])
        else:
            self.max_anal_diff.setText('0')
            self.max_anal_diff_x.setText('0')

    def plotting(self):
        lib_dir = os.path.join(os.curdir, "dll", "libNM1_lib.dll")
        lib = ctypes.windll.LoadLibrary(lib_dir)

        X_start = 0.0
        X_end = float(self.get_X_end())

        X_arr = []
        V_arr = []
        # Начальные значения
        u0 = float(self.get_U0())  # Начальное значение функции
        du0 = float(self.get_DU0())  # Начальное значение производной функции (для осн. задачи - 2)
        h0 = float(self.get_start_step())  # Начальный шаг
        eps = float(self.get_step_control())  # Параметр контроля шага

        Nmax = int(self.get_num_max_iter())  # Максимальное число итераций
        a = float(self.get_param_a())  # параметр а для осн. задачи - 2

        task = self.get_task()
        file_name = ""  # Имя основного файла, в котором хранятся шаги счёта
        file_name_extra_info = ""  # Имя файла с дополнительной информацией (в UI - колонка, расположенная в правом нижнем углу)

        # task[0]- номер задачи. 0-тестовая; 1-основная №1; 2-основная №2
        if task[0] == 0:
            my_func = lib.run_test_method  # выбор задачи
            my_func.argtypes = [ctypes.c_double, ctypes.c_int, ctypes.c_double, ctypes.c_double, ctypes.c_double,
                                ctypes.c_double]  # задание типов для параметров функции
            my_func.restype = ctypes.c_void_p  # задание типа возвращаемого значения
            my_func(u0, Nmax, X_end, 0.01, eps, h0)# Подсчёт значений. Результат записывается в файл
            file_name = "test_method_1"
            file_name_extra_info = 'test_method_2'

        elif task[0] == 1:
            my_func = lib.run_main_method_1
            my_func.argtypes = [ctypes.c_double, ctypes.c_int, ctypes.c_double, ctypes.c_double, ctypes.c_double,
                                ctypes.c_double]
            my_func.restype = ctypes.c_void_p
            my_func(u0, Nmax, X_end, 0.01, eps, h0)
            file_name = "main_method_1_1"
            file_name_extra_info = 'main_method_1_2'
        elif task[0] == 2:
            my_func = lib.run_main_method_2
            my_func.argtypes = [ctypes.c_double, ctypes.c_double, ctypes.c_int, ctypes.c_double, ctypes.c_double,
                                ctypes.c_double, ctypes.c_double, ctypes.c_double]
            my_func.restype = ctypes.c_void_p
            my_func(u0, du0, Nmax, X_end, 0.01, eps, h0, a)  # Последнее значение - параметр a
            file_name = "main_method_2_1"
            file_name_extra_info = 'main_method_2_2'

        self.clear_table()
        table = self.file_to_table(file_name)  # Парсинг файла в табличный вид ВАЖНО:(тип ячейки:str)
        self.set_table(table, task[0]) # заполнение таблицы(вкладка "Таблица")
        table_extra_info = self.file_to_table(file_name_extra_info)
        self.update_extra_info_table(task[0], table_extra_info)# заполнение вспомогательной информации(правый нижний угол)

        X_arr = [float(row[1]) for row in table]
        V_arr = [float(row[2]) for row in table]

        if task[0] == 0:# Построение графика аналитического решения(Только для тестовой задачи)
            U_arr = [float(row[9]) for row in table]
            self.plt.plot(X_arr, U_arr, label="Аналит. решение")
        if task[0] == 2:# Постороение фазового портрета(Только для основной задачи №2)
            dotU_arr = [float(row[3]) for row in table]
            self.plt_PS.plot(X_arr, dotU_arr, label="du/dx")
            self.plt_PS.legend(loc="upper right")

        self.plt.plot(X_arr, V_arr, label="Числ. решение")
        self.plt.scatter(X_start, u0,
                         label="Старт. точка")  # scatter - построение точечного графика. В данном случае просто ставит точку (x0,u0)
        self.plt.set_xlim(auto=True)
        self.plt.set_ylim(auto=True)
        self.plt.legend(loc="upper right")  # legend - задание окна легенд

        #обновление графиков
        self.plot_widget_1.canvas.draw()
        self.plot_widget_2.canvas.draw()

    def get_X_start(self):
        return self.X_start.text()

    def get_X_end(self):
        return self.X_end.text()

    def get_U0(self):
        return self.U_X0.text()

    def get_DU0(self):
        return self.DU_X0.text()

    def get_start_step(self):
        return self.step_start.text()

    def get_step_control(self):
        return self.step_control.text()

    def get_task(self):
        return self.task_selection_box.currentIndex(), self.task_selection_box.currentText()

    def get_step_mode(self):
        return self.step_mode.isChecked()

    def set_row(self, row):
        max_row_index = self.info_table.rowCount()
        self.info_table.insertRow(max_row_index)# создание строки
        for i in range(len(row)):
            self.info_table.setItem(max_row_index, i, QTableWidgetItem(str(row[i])))# заполнение элементами

    def set_columns(self, task_index):
        cols = columns[task_index]
        self.info_table.setColumnCount(len(cols))# создание пустых колонок, в количестве len(cols) штук
        self.info_table.setHorizontalHeaderLabels(cols)# присвоение имен для колонок

    def set_table(self, data, task_index):
        self.set_columns(task_index)
        for row in data:
            self.set_row(row)

    def clear_table(self):
        while (self.info_table.rowCount() > 0):
            self.info_table.removeRow(0)

    def get_num_max_iter(self):
        return self.max_num_iter.text()

    def get_param_a(self):
        return self.parameter_a.text()
