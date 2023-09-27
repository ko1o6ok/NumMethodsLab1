from PyQt5 import uic
from PyQt5.QtWidgets import QMainWindow
from matplotlib.backends.backend_qt5agg import FigureCanvasQTAgg as FigureCanvas, \
    NavigationToolbar2QT as NavigationToolbar
from matplotlib.figure import Figure

import ctypes
import os

ui_file = './UI/MainWindow.ui'


class UI_mainWindow(QMainWindow):
    def __init__(self):
        super(UI_mainWindow, self).__init__()
        uic.loadUi(ui_file, self)

        fig = Figure(figsize=(self.plot_wiget.width() / 100, self.plot_wiget.height() / 100))

        self.plot_wiget.canvas = FigureCanvas(fig)
        self.plot_wiget.plot = fig.add_subplot()

        self.plot_wiget.canvas.setParent(self.plot_wiget)

        self.addToolBar(NavigationToolbar(self.plot_wiget.canvas, self))
        self.plot_button.clicked.connect(self.plotting)



    def plotting(self):
        lib_dir = os.path.join(os.curdir,"libNM1_lib.dll")
        lib = ctypes.windll.LoadLibrary(lib_dir)
        #X_start=float(self.get_X_start())
        X_start = 0.0
        X_end=float(self.get_X_end())

        X_arr=[]
        Y_arr=[]

        # Начальные значения
        u0 = float(self.get_U0()) # Начальное значение функции
        du0 = float(self.get_DU0()) # Начальное значение производной функции (для осн. задачи - 2)
        h0 = float(self.get_start_step()) # Начальный шаг
        eps = float(self.get_step_control()) # Параметр контроля шага

        Nmax = int(self.get_num_max_iter()) # Максимальное число итераций
        a = float(self.get_param_a()) # параметр а для осн. задачи - 2
        # my_func = lib.run_test_method
        # my_func.argtypes = [ctypes.c_double,ctypes.c_int,ctypes.c_double,ctypes.c_double,ctypes.c_double,ctypes.c_double]
        # my_func.restype = ctypes.c_void_p
        # my_func(u0,1000,X_end,0.01,eps,h0)

        task = self.get_task()
        if task[0] == 0:
            # Выбрана тестовая задача
            my_func = lib.run_test_method
            my_func.argtypes = [ctypes.c_double,ctypes.c_int,ctypes.c_double,ctypes.c_double,ctypes.c_double,ctypes.c_double]
            my_func.restype = ctypes.c_void_p
            my_func(u0,Nmax,X_end,0.01,eps,h0)
        elif task[0] == 1:
            # Выбрана основная задача - 1
            my_func = lib.run_main_method_1
            my_func.argtypes = [ctypes.c_double,ctypes.c_int,ctypes.c_double,ctypes.c_double,ctypes.c_double,ctypes.c_double]
            my_func.restype = ctypes.c_void_p
            my_func(u0,Nmax,X_end,0.01,eps,h0)
        elif task[0] == 2:
            # Выбрана основная задача - 2
            my_func = lib.run_main_method_2
            my_func.argtypes = [ctypes.c_double,ctypes.c_double,ctypes.c_int,ctypes.c_double,ctypes.c_double,ctypes.c_double,ctypes.c_double,ctypes.c_double]
            my_func.restype = ctypes.c_void_p
            my_func(u0,du0,Nmax,X_end,0.01,eps,h0,a) # Последнее значение - параметр a

        ###TODO получить значение точек
        ###TODO получить значение точек
        ###TODO получить значение точек

        self.plot_wiget.plot.plot(X_arr, Y_arr)

        self.plot_wiget.plot.set_xlim(X_start,X_end)
        self.plot_wiget.plot.set_ylim(0,5)

        self.plot_wiget.canvas.draw()

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

    def get_param_a(self):
        return self.parameter_a.text()

    def get_num_max_iter(self):
        return self.max_num_iter.text()
