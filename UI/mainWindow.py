import matplotlib.pyplot as plt
from PyQt5 import uic
from PyQt5.QtWidgets import QMainWindow, QTableWidgetItem
from matplotlib.backends.backend_qt5agg import FigureCanvasQTAgg as FigureCanvas, \
    NavigationToolbar2QT as NavigationToolbar
from matplotlib.figure import Figure

import ctypes
import os

ui_file = './UI/MainWindow.ui'


def create_plot(parent):
    parent.fig = Figure(figsize=(parent.width() / 100, parent.height() / 100))
    parent.canvas = FigureCanvas(parent.fig)
    parent.plot = parent.fig.add_subplot()
    return parent.plot


class UI_mainWindow(QMainWindow):
    def __init__(self):
        super(UI_mainWindow, self).__init__()
        uic.loadUi(ui_file, self)

        self.plt = create_plot(self.plot_widget_1)
        self.plt_PS = create_plot(self.plot_widget_2)

        self.plot_widget_1.canvas.setParent(self.plot_widget_1)
        self.plot_widget_2.canvas.setParent(self.plot_widget_2)

        self.tabWidget.currentChanged.connect(self.toolBar_changing)
        self.plot_toolBar = NavigationToolbar(self.plot_widget_1.canvas, self)

        self.addToolBar(self.plot_toolBar)

        self.plot_button.clicked.connect(self.plotting)

        self.delete_plot.clicked.connect(self.clear_plots)

    def clear_plots(self):
        self.plt.cla()
        self.plt_PS.cla()
        self.plot_widget_1.canvas.draw()
        self.plot_widget_2.canvas.draw()

    def toolBar_changing(self, index):
        self.removeToolBar(self.plot_toolBar)
        if index == 0:
            self.plot_toolBar = NavigationToolbar(self.plot_widget_1.canvas, self)
        elif index == 2:
            self.plot_toolBar = NavigationToolbar(self.plot_widget_2.canvas, self)
        self.addToolBar(self.plot_toolBar)

    def file_to_table(self, file_name):
        if len(file_name.split('.')) == 1:
            file_name += '.txt'
        table = []
        with open(file_name, 'r') as f:
            for line in f:
                table.append(line.split(' '))
        self.set_table(table)
        return table

    def plotting(self):
        lib_dir = os.path.join(os.curdir,"dll","libNM1_lib.dll")
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
        file_name = ""
        file_name_extra_info = ""
        if task[0] == 0:
            # Выбрана тестовая задача
            my_func = lib.run_test_method
            my_func.argtypes = [ctypes.c_double, ctypes.c_int, ctypes.c_double, ctypes.c_double, ctypes.c_double,
                                ctypes.c_double]
            my_func.restype = ctypes.c_void_p
            my_func(u0, Nmax, X_end, 0.01, eps, h0)
            file_name = "test_method_1"
            file_name_extra_info = 'test_method_2'

        elif task[0] == 1:
            # Выбрана основная задача - 1
            my_func = lib.run_main_method_1
            my_func.argtypes = [ctypes.c_double, ctypes.c_int, ctypes.c_double, ctypes.c_double, ctypes.c_double,
                                ctypes.c_double]
            my_func.restype = ctypes.c_void_p
            my_func(u0, Nmax, X_end, 0.01, eps, h0)
            file_name = "main_method_1_1"
            file_name_extra_info = 'main_method_1_2'
        elif task[0] == 2:
            # Выбрана основная задача - 2
            my_func = lib.run_main_method_2
            my_func.argtypes = [ctypes.c_double, ctypes.c_double, ctypes.c_int, ctypes.c_double, ctypes.c_double,
                                ctypes.c_double, ctypes.c_double, ctypes.c_double]
            my_func.restype = ctypes.c_void_p
            my_func(u0, du0, Nmax, X_end, 0.01, eps, h0, a)  # Последнее значение - параметр a
            file_name = "main_method_2_1"
            file_name_extra_info = 'main_method_2_2'

        self.clear_table()
        table = self.file_to_table(file_name)

        table_extra_info = self.file_to_table(file_name_extra_info)
        ###TODO вывод дополнительной инфы в колонку, которая находится в правом нижнем углу

        X_arr = [float(row[1]) for row in table]
        V_arr = [float(row[2]) for row in table]
        if task[0] == 0:
            U_arr=[float(row[9]) for row in table]
            self.plt.plot(X_arr,U_arr,label="Аналит. решение")
        if task[0]==2:
            dotU_arr=[float(row[3]) for row in table]
            self.plt_PS.plot(X_arr,dotU_arr,label="du/dx")
            self.plt_PS.legend(loc="upper right")

        self.plt.plot(X_arr, V_arr,label="Числ. решение")
        self.plt.scatter(X_start,u0,label="Старт. точка")
        self.plt.set_xlim(auto=True)
        self.plt.set_ylim(auto=True)
        self.plt.legend(loc="upper right")


        self.plot_widget_1.canvas.draw()

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
        self.info_table.insertRow(max_row_index)
        for i in range(len(row)):
            self.info_table.setItem(max_row_index, i, QTableWidgetItem(str(row[i])))

    def set_table(self, data):
        for row in data:
            self.set_row(row)

    def clear_table(self):
        while (self.info_table.rowCount() > 0):
            self.info_table.removeRow(0)

    def get_num_max_iter(self):
        return self.max_num_iter.text()

    def get_param_a(self):
        return self.parameter_a.text()
