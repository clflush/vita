# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file 'vita_sr_gui.ui'
#
# Created: Thu Oct 28 09:47:14 2010
#      by: PyQt4 UI code generator 4.7.3
#
# WARNING! All changes made in this file will be lost!

from PyQt4 import QtCore, QtGui

class Ui_main_window(object):
    def setupUi(self, main_window):
        main_window.setObjectName("main_window")
        main_window.resize(598, 455)
        self.central_widget = QtGui.QWidget(main_window)
        self.central_widget.setObjectName("central_widget")
        self._4 = QtGui.QVBoxLayout(self.central_widget)
        self._4.setObjectName("_4")
        self.tw_vger = QtGui.QTabWidget(self.central_widget)
        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Maximum, QtGui.QSizePolicy.Maximum)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.tw_vger.sizePolicy().hasHeightForWidth())
        self.tw_vger.setSizePolicy(sizePolicy)
        self.tw_vger.setMaximumSize(QtCore.QSize(30000, 30000))
        self.tw_vger.setSizeIncrement(QtCore.QSize(0, 0))
        self.tw_vger.setObjectName("tw_vger")
        self.tab_data = QtGui.QWidget()
        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Preferred, QtGui.QSizePolicy.Preferred)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.tab_data.sizePolicy().hasHeightForWidth())
        self.tab_data.setSizePolicy(sizePolicy)
        self.tab_data.setObjectName("tab_data")
        self.verticalLayout = QtGui.QVBoxLayout(self.tab_data)
        self.verticalLayout.setObjectName("verticalLayout")
        self.gb_training_data = QtGui.QGroupBox(self.tab_data)
        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Preferred, QtGui.QSizePolicy.Fixed)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.gb_training_data.sizePolicy().hasHeightForWidth())
        self.gb_training_data.setSizePolicy(sizePolicy)
        self.gb_training_data.setAutoFillBackground(False)
        self.gb_training_data.setCheckable(False)
        self.gb_training_data.setObjectName("gb_training_data")
        self.horizontalLayout = QtGui.QHBoxLayout(self.gb_training_data)
        self.horizontalLayout.setObjectName("horizontalLayout")
        self.le_training_data = QtGui.QLineEdit(self.gb_training_data)
        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Expanding, QtGui.QSizePolicy.Fixed)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.le_training_data.sizePolicy().hasHeightForWidth())
        self.le_training_data.setSizePolicy(sizePolicy)
        self.le_training_data.setText("")
        self.le_training_data.setObjectName("le_training_data")
        self.horizontalLayout.addWidget(self.le_training_data)
        self.pb_browse_training_data = QtGui.QPushButton(self.gb_training_data)
        self.pb_browse_training_data.setObjectName("pb_browse_training_data")
        self.horizontalLayout.addWidget(self.pb_browse_training_data)
        self.verticalLayout.addWidget(self.gb_training_data)
        self.gb_validation_data = QtGui.QGroupBox(self.tab_data)
        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Preferred, QtGui.QSizePolicy.Fixed)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.gb_validation_data.sizePolicy().hasHeightForWidth())
        self.gb_validation_data.setSizePolicy(sizePolicy)
        self.gb_validation_data.setFlat(False)
        self.gb_validation_data.setCheckable(True)
        self.gb_validation_data.setChecked(False)
        self.gb_validation_data.setObjectName("gb_validation_data")
        self._3 = QtGui.QHBoxLayout(self.gb_validation_data)
        self._3.setObjectName("_3")
        self.le_validation_data = QtGui.QLineEdit(self.gb_validation_data)
        self.le_validation_data.setObjectName("le_validation_data")
        self._3.addWidget(self.le_validation_data)
        self.pb_browse_validation_data = QtGui.QPushButton(self.gb_validation_data)
        self.pb_browse_validation_data.setObjectName("pb_browse_validation_data")
        self._3.addWidget(self.pb_browse_validation_data)
        self.verticalLayout.addWidget(self.gb_validation_data)
        self.gb_symbols = QtGui.QGroupBox(self.tab_data)
        sizePolicy = QtGui.QSizePolicy(QtGui.QSizePolicy.Preferred, QtGui.QSizePolicy.Fixed)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.gb_symbols.sizePolicy().hasHeightForWidth())
        self.gb_symbols.setSizePolicy(sizePolicy)
        self.gb_symbols.setCheckable(True)
        self.gb_symbols.setChecked(False)
        self.gb_symbols.setObjectName("gb_symbols")
        self.horizontalLayout_4 = QtGui.QHBoxLayout(self.gb_symbols)
        self.horizontalLayout_4.setObjectName("horizontalLayout_4")
        self.le_symbols = QtGui.QLineEdit(self.gb_symbols)
        self.le_symbols.setObjectName("le_symbols")
        self.horizontalLayout_4.addWidget(self.le_symbols)
        self.pb_browse_symbols = QtGui.QPushButton(self.gb_symbols)
        self.pb_browse_symbols.setObjectName("pb_browse_symbols")
        self.horizontalLayout_4.addWidget(self.pb_browse_symbols)
        self.verticalLayout.addWidget(self.gb_symbols)
        self.tw_vger.addTab(self.tab_data, "")
        self.tab_monitor = QtGui.QWidget()
        self.tab_monitor.setObjectName("tab_monitor")
        self.verticalLayout_3 = QtGui.QVBoxLayout(self.tab_monitor)
        self.verticalLayout_3.setObjectName("verticalLayout_3")
        self.tw_monitor = QtGui.QTabWidget(self.tab_monitor)
        self.tw_monitor.setTabPosition(QtGui.QTabWidget.East)
        self.tw_monitor.setObjectName("tw_monitor")
        self.tab_overview = QtGui.QWidget()
        self.tab_overview.setObjectName("tab_overview")
        self.verticalLayout_4 = QtGui.QVBoxLayout(self.tab_overview)
        self.verticalLayout_4.setObjectName("verticalLayout_4")
        self.groupBox_projectStatus = QtGui.QGroupBox(self.tab_overview)
        self.groupBox_projectStatus.setObjectName("groupBox_projectStatus")
        self.formLayout = QtGui.QFormLayout(self.groupBox_projectStatus)
        self.formLayout.setFieldGrowthPolicy(QtGui.QFormLayout.ExpandingFieldsGrow)
        self.formLayout.setObjectName("formLayout")
        self.label = QtGui.QLabel(self.groupBox_projectStatus)
        self.label.setObjectName("label")
        self.formLayout.setWidget(0, QtGui.QFormLayout.LabelRole, self.label)
        self.label_totalRunsStarted = QtGui.QLabel(self.groupBox_projectStatus)
        self.label_totalRunsStarted.setObjectName("label_totalRunsStarted")
        self.formLayout.setWidget(0, QtGui.QFormLayout.FieldRole, self.label_totalRunsStarted)
        self.label_3 = QtGui.QLabel(self.groupBox_projectStatus)
        self.label_3.setObjectName("label_3")
        self.formLayout.setWidget(1, QtGui.QFormLayout.LabelRole, self.label_3)
        self.label_programsEvaluated = QtGui.QLabel(self.groupBox_projectStatus)
        self.label_programsEvaluated.setObjectName("label_programsEvaluated")
        self.formLayout.setWidget(1, QtGui.QFormLayout.FieldRole, self.label_programsEvaluated)
        self.label_5 = QtGui.QLabel(self.groupBox_projectStatus)
        self.label_5.setObjectName("label_5")
        self.formLayout.setWidget(3, QtGui.QFormLayout.LabelRole, self.label_5)
        self.label_totalTime = QtGui.QLabel(self.groupBox_projectStatus)
        self.label_totalTime.setObjectName("label_totalTime")
        self.formLayout.setWidget(3, QtGui.QFormLayout.FieldRole, self.label_totalTime)
        self.verticalLayout_4.addWidget(self.groupBox_projectStatus)
        self.gbBestProgram = QtGui.QGroupBox(self.tab_overview)
        self.gbBestProgram.setObjectName("gbBestProgram")
        self.formLayout_2 = QtGui.QFormLayout(self.gbBestProgram)
        self.formLayout_2.setObjectName("formLayout_2")
        self.label_10 = QtGui.QLabel(self.gbBestProgram)
        self.label_10.setObjectName("label_10")
        self.formLayout_2.setWidget(0, QtGui.QFormLayout.LabelRole, self.label_10)
        self.label_trainingScore = QtGui.QLabel(self.gbBestProgram)
        self.label_trainingScore.setObjectName("label_trainingScore")
        self.formLayout_2.setWidget(0, QtGui.QFormLayout.FieldRole, self.label_trainingScore)
        self.label_2 = QtGui.QLabel(self.gbBestProgram)
        self.label_2.setObjectName("label_2")
        self.formLayout_2.setWidget(1, QtGui.QFormLayout.LabelRole, self.label_2)
        self.label_validationScore = QtGui.QLabel(self.gbBestProgram)
        self.label_validationScore.setObjectName("label_validationScore")
        self.formLayout_2.setWidget(1, QtGui.QFormLayout.FieldRole, self.label_validationScore)
        self.verticalLayout_4.addWidget(self.gbBestProgram)
        self.tw_monitor.addTab(self.tab_overview, "")
        self.tab_history = QtGui.QWidget()
        self.tab_history.setObjectName("tab_history")
        self.verticalLayout_5 = QtGui.QVBoxLayout(self.tab_history)
        self.verticalLayout_5.setObjectName("verticalLayout_5")
        self.tw_monitor.addTab(self.tab_history, "")
        self.tab_detail = QtGui.QWidget()
        self.tab_detail.setObjectName("tab_detail")
        self.verticalLayout_6 = QtGui.QVBoxLayout(self.tab_detail)
        self.verticalLayout_6.setObjectName("verticalLayout_6")
        self.listView = QtGui.QListView(self.tab_detail)
        self.listView.setObjectName("listView")
        self.verticalLayout_6.addWidget(self.listView)
        self.tw_monitor.addTab(self.tab_detail, "")
        self.tab_current = QtGui.QWidget()
        self.tab_current.setObjectName("tab_current")
        self.horizontalLayout_3 = QtGui.QHBoxLayout(self.tab_current)
        self.horizontalLayout_3.setObjectName("horizontalLayout_3")
        self.groupBox_3 = QtGui.QGroupBox(self.tab_current)
        self.groupBox_3.setObjectName("groupBox_3")
        self.formLayout_3 = QtGui.QFormLayout(self.groupBox_3)
        self.formLayout_3.setObjectName("formLayout_3")
        self.label_4 = QtGui.QLabel(self.groupBox_3)
        self.label_4.setObjectName("label_4")
        self.formLayout_3.setWidget(0, QtGui.QFormLayout.LabelRole, self.label_4)
        self.label_6 = QtGui.QLabel(self.groupBox_3)
        self.label_6.setObjectName("label_6")
        self.formLayout_3.setWidget(0, QtGui.QFormLayout.FieldRole, self.label_6)
        self.label_9 = QtGui.QLabel(self.groupBox_3)
        self.label_9.setObjectName("label_9")
        self.formLayout_3.setWidget(1, QtGui.QFormLayout.LabelRole, self.label_9)
        self.label_11 = QtGui.QLabel(self.groupBox_3)
        self.label_11.setObjectName("label_11")
        self.formLayout_3.setWidget(1, QtGui.QFormLayout.FieldRole, self.label_11)
        self.label_14 = QtGui.QLabel(self.groupBox_3)
        self.label_14.setObjectName("label_14")
        self.formLayout_3.setWidget(4, QtGui.QFormLayout.LabelRole, self.label_14)
        self.label_15 = QtGui.QLabel(self.groupBox_3)
        self.label_15.setObjectName("label_15")
        self.formLayout_3.setWidget(4, QtGui.QFormLayout.FieldRole, self.label_15)
        self.label_16 = QtGui.QLabel(self.groupBox_3)
        self.label_16.setObjectName("label_16")
        self.formLayout_3.setWidget(2, QtGui.QFormLayout.LabelRole, self.label_16)
        self.label_17 = QtGui.QLabel(self.groupBox_3)
        self.label_17.setObjectName("label_17")
        self.formLayout_3.setWidget(2, QtGui.QFormLayout.FieldRole, self.label_17)
        self.horizontalLayout_3.addWidget(self.groupBox_3)
        self.groupBox_5 = QtGui.QGroupBox(self.tab_current)
        self.groupBox_5.setObjectName("groupBox_5")
        self.formLayout_4 = QtGui.QFormLayout(self.groupBox_5)
        self.formLayout_4.setObjectName("formLayout_4")
        self.label_7 = QtGui.QLabel(self.groupBox_5)
        self.label_7.setObjectName("label_7")
        self.formLayout_4.setWidget(0, QtGui.QFormLayout.LabelRole, self.label_7)
        self.label_8 = QtGui.QLabel(self.groupBox_5)
        self.label_8.setObjectName("label_8")
        self.formLayout_4.setWidget(0, QtGui.QFormLayout.FieldRole, self.label_8)
        self.label_12 = QtGui.QLabel(self.groupBox_5)
        self.label_12.setObjectName("label_12")
        self.formLayout_4.setWidget(1, QtGui.QFormLayout.LabelRole, self.label_12)
        self.label_13 = QtGui.QLabel(self.groupBox_5)
        self.label_13.setObjectName("label_13")
        self.formLayout_4.setWidget(1, QtGui.QFormLayout.FieldRole, self.label_13)
        self.label_18 = QtGui.QLabel(self.groupBox_5)
        self.label_18.setObjectName("label_18")
        self.formLayout_4.setWidget(2, QtGui.QFormLayout.LabelRole, self.label_18)
        self.label_19 = QtGui.QLabel(self.groupBox_5)
        self.label_19.setObjectName("label_19")
        self.formLayout_4.setWidget(2, QtGui.QFormLayout.FieldRole, self.label_19)
        self.label_20 = QtGui.QLabel(self.groupBox_5)
        self.label_20.setObjectName("label_20")
        self.formLayout_4.setWidget(3, QtGui.QFormLayout.LabelRole, self.label_20)
        self.label_21 = QtGui.QLabel(self.groupBox_5)
        self.label_21.setObjectName("label_21")
        self.formLayout_4.setWidget(3, QtGui.QFormLayout.FieldRole, self.label_21)
        self.horizontalLayout_3.addWidget(self.groupBox_5)
        self.tw_monitor.addTab(self.tab_current, "")
        self.verticalLayout_3.addWidget(self.tw_monitor)
        self.tw_vger.addTab(self.tab_monitor, "")
        self.tab_options = QtGui.QWidget()
        self.tab_options.setObjectName("tab_options")
        self.verticalLayout_8 = QtGui.QVBoxLayout(self.tab_options)
        self.verticalLayout_8.setObjectName("verticalLayout_8")
        self.tw_options = QtGui.QTabWidget(self.tab_options)
        self.tw_options.setTabPosition(QtGui.QTabWidget.East)
        self.tw_options.setObjectName("tw_options")
        self.tab_general = QtGui.QWidget()
        self.tab_general.setObjectName("tab_general")
        self.formLayout_5 = QtGui.QFormLayout(self.tab_general)
        self.formLayout_5.setFieldGrowthPolicy(QtGui.QFormLayout.ExpandingFieldsGrow)
        self.formLayout_5.setObjectName("formLayout_5")
        self.gb_config = QtGui.QGroupBox(self.tab_general)
        self.gb_config.setObjectName("gb_config")
        self.formLayout_7 = QtGui.QFormLayout(self.gb_config)
        self.formLayout_7.setFieldGrowthPolicy(QtGui.QFormLayout.ExpandingFieldsGrow)
        self.formLayout_7.setObjectName("formLayout_7")
        self.gb_program_path = QtGui.QGroupBox(self.gb_config)
        self.gb_program_path.setObjectName("gb_program_path")
        self.horizontalLayout_2 = QtGui.QHBoxLayout(self.gb_program_path)
        self.horizontalLayout_2.setObjectName("horizontalLayout_2")
        self.le_program_path = QtGui.QLineEdit(self.gb_program_path)
        self.le_program_path.setObjectName("le_program_path")
        self.horizontalLayout_2.addWidget(self.le_program_path)
        self.pb_browse_program_path = QtGui.QPushButton(self.gb_program_path)
        self.pb_browse_program_path.setObjectName("pb_browse_program_path")
        self.horizontalLayout_2.addWidget(self.pb_browse_program_path)
        self.formLayout_7.setWidget(0, QtGui.QFormLayout.LabelRole, self.gb_program_path)
        self.formLayout_5.setWidget(0, QtGui.QFormLayout.LabelRole, self.gb_config)
        self.gb_evolution = QtGui.QGroupBox(self.tab_general)
        self.gb_evolution.setObjectName("gb_evolution")
        self.verticalLayout_2 = QtGui.QVBoxLayout(self.gb_evolution)
        self.verticalLayout_2.setObjectName("verticalLayout_2")
        self.label_22 = QtGui.QLabel(self.gb_evolution)
        self.label_22.setObjectName("label_22")
        self.verticalLayout_2.addWidget(self.label_22)
        self.spinBox_populationSize = QtGui.QSpinBox(self.gb_evolution)
        self.spinBox_populationSize.setMinimum(2)
        self.spinBox_populationSize.setMaximum(1000000000)
        self.spinBox_populationSize.setProperty("value", 100)
        self.spinBox_populationSize.setObjectName("spinBox_populationSize")
        self.verticalLayout_2.addWidget(self.spinBox_populationSize)
        self.label_23 = QtGui.QLabel(self.gb_evolution)
        self.label_23.setObjectName("label_23")
        self.verticalLayout_2.addWidget(self.label_23)
        self.spinBox_programSize = QtGui.QSpinBox(self.gb_evolution)
        self.spinBox_programSize.setEnabled(True)
        self.spinBox_programSize.setMinimum(1)
        self.spinBox_programSize.setMaximum(1000000)
        self.spinBox_programSize.setProperty("value", 100)
        self.spinBox_programSize.setObjectName("spinBox_programSize")
        self.verticalLayout_2.addWidget(self.spinBox_programSize)
        self.formLayout_5.setWidget(0, QtGui.QFormLayout.FieldRole, self.gb_evolution)
        self.groupBox_9 = QtGui.QGroupBox(self.tab_general)
        self.groupBox_9.setObjectName("groupBox_9")
        self.formLayout_5.setWidget(1, QtGui.QFormLayout.LabelRole, self.groupBox_9)
        self.tw_options.addTab(self.tab_general, "")
        self.tab_evolution = QtGui.QWidget()
        self.tab_evolution.setObjectName("tab_evolution")
        self.verticalLayout_7 = QtGui.QVBoxLayout(self.tab_evolution)
        self.verticalLayout_7.setObjectName("verticalLayout_7")
        self.tw_options.addTab(self.tab_evolution, "")
        self.verticalLayout_8.addWidget(self.tw_options)
        self.tw_vger.addTab(self.tab_options, "")
        self.tab_evaluator = QtGui.QWidget()
        self.tab_evaluator.setObjectName("tab_evaluator")
        self.tw_vger.addTab(self.tab_evaluator, "")
        self._4.addWidget(self.tw_vger)
        main_window.setCentralWidget(self.central_widget)
        self.menubar = QtGui.QMenuBar(main_window)
        self.menubar.setGeometry(QtCore.QRect(0, 0, 598, 20))
        self.menubar.setObjectName("menubar")
        self.menu_File = QtGui.QMenu(self.menubar)
        self.menu_File.setObjectName("menu_File")
        self.menu_Run = QtGui.QMenu(self.menubar)
        self.menu_Run.setObjectName("menu_Run")
        main_window.setMenuBar(self.menubar)
        self.statusbar = QtGui.QStatusBar(main_window)
        self.statusbar.setObjectName("statusbar")
        main_window.setStatusBar(self.statusbar)
        self.toolBar = QtGui.QToolBar(main_window)
        self.toolBar.setObjectName("toolBar")
        main_window.addToolBar(QtCore.Qt.ToolBarArea(QtCore.Qt.TopToolBarArea), self.toolBar)
        self.act_open_project = QtGui.QAction(main_window)
        self.act_open_project.setObjectName("act_open_project")
        self.act_close = QtGui.QAction(main_window)
        self.act_close.setObjectName("act_close")
        self.act_new_project = QtGui.QAction(main_window)
        self.act_new_project.setObjectName("act_new_project")
        self.act_import_configuration = QtGui.QAction(main_window)
        self.act_import_configuration.setObjectName("act_import_configuration")
        self.act_save_project = QtGui.QAction(main_window)
        self.act_save_project.setObjectName("act_save_project")
        self.act_start = QtGui.QAction(main_window)
        self.act_start.setObjectName("act_start")
        self.act_stop = QtGui.QAction(main_window)
        self.act_stop.setObjectName("act_stop")
        self.act_finish = QtGui.QAction(main_window)
        self.act_finish.setObjectName("act_finish")
        self.act_continue = QtGui.QAction(main_window)
        self.act_continue.setObjectName("act_continue")
        self.menu_File.addAction(self.act_new_project)
        self.menu_File.addAction(self.act_open_project)
        self.menu_File.addAction(self.act_save_project)
        self.menu_File.addAction(self.act_close)
        self.menu_File.addSeparator()
        self.menu_File.addAction(self.act_import_configuration)
        self.menu_Run.addAction(self.act_start)
        self.menu_Run.addAction(self.act_stop)
        self.menu_Run.addAction(self.act_continue)
        self.menu_Run.addAction(self.act_finish)
        self.menubar.addAction(self.menu_File.menuAction())
        self.menubar.addAction(self.menu_Run.menuAction())
        self.toolBar.addAction(self.act_new_project)
        self.toolBar.addAction(self.act_open_project)
        self.toolBar.addAction(self.act_save_project)
        self.toolBar.addSeparator()
        self.toolBar.addAction(self.act_start)
        self.toolBar.addAction(self.act_finish)

        self.retranslateUi(main_window)
        self.tw_vger.setCurrentIndex(0)
        self.tw_monitor.setCurrentIndex(0)
        self.tw_options.setCurrentIndex(0)
        QtCore.QMetaObject.connectSlotsByName(main_window)
        main_window.setTabOrder(self.tw_monitor, self.listView)
        main_window.setTabOrder(self.listView, self.spinBox_populationSize)
        main_window.setTabOrder(self.spinBox_populationSize, self.tw_options)
        main_window.setTabOrder(self.tw_options, self.spinBox_programSize)

    def retranslateUi(self, main_window):
        main_window.setWindowTitle(QtGui.QApplication.translate("main_window", "VITA - Symbolic Regression and classification", None, QtGui.QApplication.UnicodeUTF8))
        self.gb_training_data.setTitle(QtGui.QApplication.translate("main_window", "Training data", None, QtGui.QApplication.UnicodeUTF8))
        self.pb_browse_training_data.setText(QtGui.QApplication.translate("main_window", "Browse", None, QtGui.QApplication.UnicodeUTF8))
        self.gb_validation_data.setTitle(QtGui.QApplication.translate("main_window", "Validation data", None, QtGui.QApplication.UnicodeUTF8))
        self.pb_browse_validation_data.setText(QtGui.QApplication.translate("main_window", "Browse", None, QtGui.QApplication.UnicodeUTF8))
        self.gb_symbols.setTitle(QtGui.QApplication.translate("main_window", "Symbols", None, QtGui.QApplication.UnicodeUTF8))
        self.pb_browse_symbols.setText(QtGui.QApplication.translate("main_window", "Browse", None, QtGui.QApplication.UnicodeUTF8))
        self.tw_vger.setTabText(self.tw_vger.indexOf(self.tab_data), QtGui.QApplication.translate("main_window", "Data", None, QtGui.QApplication.UnicodeUTF8))
        self.groupBox_projectStatus.setTitle(QtGui.QApplication.translate("main_window", "Project status", None, QtGui.QApplication.UnicodeUTF8))
        self.label.setText(QtGui.QApplication.translate("main_window", "Total runs started", None, QtGui.QApplication.UnicodeUTF8))
        self.label_totalRunsStarted.setText(QtGui.QApplication.translate("main_window", "____________", None, QtGui.QApplication.UnicodeUTF8))
        self.label_3.setText(QtGui.QApplication.translate("main_window", "Programs evaluated", None, QtGui.QApplication.UnicodeUTF8))
        self.label_programsEvaluated.setText(QtGui.QApplication.translate("main_window", "____________", None, QtGui.QApplication.UnicodeUTF8))
        self.label_5.setText(QtGui.QApplication.translate("main_window", "Total time", None, QtGui.QApplication.UnicodeUTF8))
        self.label_totalTime.setText(QtGui.QApplication.translate("main_window", "____________", None, QtGui.QApplication.UnicodeUTF8))
        self.gbBestProgram.setTitle(QtGui.QApplication.translate("main_window", "Best program", None, QtGui.QApplication.UnicodeUTF8))
        self.label_10.setText(QtGui.QApplication.translate("main_window", "Training fitness", None, QtGui.QApplication.UnicodeUTF8))
        self.label_trainingScore.setText(QtGui.QApplication.translate("main_window", "____________", None, QtGui.QApplication.UnicodeUTF8))
        self.label_2.setText(QtGui.QApplication.translate("main_window", "Validation fitness", None, QtGui.QApplication.UnicodeUTF8))
        self.label_validationScore.setText(QtGui.QApplication.translate("main_window", "____________", None, QtGui.QApplication.UnicodeUTF8))
        self.tw_monitor.setTabText(self.tw_monitor.indexOf(self.tab_overview), QtGui.QApplication.translate("main_window", "Overview", None, QtGui.QApplication.UnicodeUTF8))
        self.tw_monitor.setTabText(self.tw_monitor.indexOf(self.tab_history), QtGui.QApplication.translate("main_window", "History", None, QtGui.QApplication.UnicodeUTF8))
        self.tw_monitor.setTabText(self.tw_monitor.indexOf(self.tab_detail), QtGui.QApplication.translate("main_window", "Detail", None, QtGui.QApplication.UnicodeUTF8))
        self.groupBox_3.setTitle(QtGui.QApplication.translate("main_window", "Status", None, QtGui.QApplication.UnicodeUTF8))
        self.label_4.setText(QtGui.QApplication.translate("main_window", "Generations since start", None, QtGui.QApplication.UnicodeUTF8))
        self.label_6.setText(QtGui.QApplication.translate("main_window", "____________", None, QtGui.QApplication.UnicodeUTF8))
        self.label_9.setText(QtGui.QApplication.translate("main_window", "Generations w/o improv.", None, QtGui.QApplication.UnicodeUTF8))
        self.label_11.setText(QtGui.QApplication.translate("main_window", "____________", None, QtGui.QApplication.UnicodeUTF8))
        self.label_14.setText(QtGui.QApplication.translate("main_window", "Validation fitness", None, QtGui.QApplication.UnicodeUTF8))
        self.label_15.setText(QtGui.QApplication.translate("main_window", "____________", None, QtGui.QApplication.UnicodeUTF8))
        self.label_16.setText(QtGui.QApplication.translate("main_window", "Training fitness", None, QtGui.QApplication.UnicodeUTF8))
        self.label_17.setText(QtGui.QApplication.translate("main_window", "____________", None, QtGui.QApplication.UnicodeUTF8))
        self.groupBox_5.setTitle(QtGui.QApplication.translate("main_window", "Selected parameters", None, QtGui.QApplication.UnicodeUTF8))
        self.label_7.setText(QtGui.QApplication.translate("main_window", "Population size", None, QtGui.QApplication.UnicodeUTF8))
        self.label_8.setText(QtGui.QApplication.translate("main_window", "____________", None, QtGui.QApplication.UnicodeUTF8))
        self.label_12.setText(QtGui.QApplication.translate("main_window", "Max program size", None, QtGui.QApplication.UnicodeUTF8))
        self.label_13.setText(QtGui.QApplication.translate("main_window", "____________", None, QtGui.QApplication.UnicodeUTF8))
        self.label_18.setText(QtGui.QApplication.translate("main_window", "Crossover rate", None, QtGui.QApplication.UnicodeUTF8))
        self.label_19.setText(QtGui.QApplication.translate("main_window", "____________", None, QtGui.QApplication.UnicodeUTF8))
        self.label_20.setText(QtGui.QApplication.translate("main_window", "Mutation rate", None, QtGui.QApplication.UnicodeUTF8))
        self.label_21.setText(QtGui.QApplication.translate("main_window", "____________", None, QtGui.QApplication.UnicodeUTF8))
        self.tw_monitor.setTabText(self.tw_monitor.indexOf(self.tab_current), QtGui.QApplication.translate("main_window", "Current run", None, QtGui.QApplication.UnicodeUTF8))
        self.tw_vger.setTabText(self.tw_vger.indexOf(self.tab_monitor), QtGui.QApplication.translate("main_window", "Monitor", None, QtGui.QApplication.UnicodeUTF8))
        self.gb_config.setTitle(QtGui.QApplication.translate("main_window", "Config", None, QtGui.QApplication.UnicodeUTF8))
        self.gb_program_path.setTitle(QtGui.QApplication.translate("main_window", "Program path", None, QtGui.QApplication.UnicodeUTF8))
        self.pb_browse_program_path.setText(QtGui.QApplication.translate("main_window", "Browse", None, QtGui.QApplication.UnicodeUTF8))
        self.gb_evolution.setTitle(QtGui.QApplication.translate("main_window", "Evolution", None, QtGui.QApplication.UnicodeUTF8))
        self.label_22.setText(QtGui.QApplication.translate("main_window", "Population size", None, QtGui.QApplication.UnicodeUTF8))
        self.label_23.setText(QtGui.QApplication.translate("main_window", "Program size", None, QtGui.QApplication.UnicodeUTF8))
        self.groupBox_9.setTitle(QtGui.QApplication.translate("main_window", "Output files", None, QtGui.QApplication.UnicodeUTF8))
        self.tw_options.setTabText(self.tw_options.indexOf(self.tab_general), QtGui.QApplication.translate("main_window", "General", None, QtGui.QApplication.UnicodeUTF8))
        self.tw_options.setTabText(self.tw_options.indexOf(self.tab_evolution), QtGui.QApplication.translate("main_window", "Evolution", None, QtGui.QApplication.UnicodeUTF8))
        self.tw_vger.setTabText(self.tw_vger.indexOf(self.tab_options), QtGui.QApplication.translate("main_window", "Options", None, QtGui.QApplication.UnicodeUTF8))
        self.tw_vger.setTabText(self.tw_vger.indexOf(self.tab_evaluator), QtGui.QApplication.translate("main_window", "Evaluator", None, QtGui.QApplication.UnicodeUTF8))
        self.menu_File.setTitle(QtGui.QApplication.translate("main_window", "&File", None, QtGui.QApplication.UnicodeUTF8))
        self.menu_Run.setTitle(QtGui.QApplication.translate("main_window", "&Run", None, QtGui.QApplication.UnicodeUTF8))
        self.toolBar.setWindowTitle(QtGui.QApplication.translate("main_window", "toolBar", None, QtGui.QApplication.UnicodeUTF8))
        self.act_open_project.setText(QtGui.QApplication.translate("main_window", "&Open project", None, QtGui.QApplication.UnicodeUTF8))
        self.act_close.setText(QtGui.QApplication.translate("main_window", "&Close", None, QtGui.QApplication.UnicodeUTF8))
        self.act_new_project.setText(QtGui.QApplication.translate("main_window", "&New project", None, QtGui.QApplication.UnicodeUTF8))
        self.act_import_configuration.setText(QtGui.QApplication.translate("main_window", "&Import configuration", None, QtGui.QApplication.UnicodeUTF8))
        self.act_save_project.setText(QtGui.QApplication.translate("main_window", "&Save project", None, QtGui.QApplication.UnicodeUTF8))
        self.act_start.setText(QtGui.QApplication.translate("main_window", "&Start", None, QtGui.QApplication.UnicodeUTF8))
        self.act_stop.setText(QtGui.QApplication.translate("main_window", "&Pause", None, QtGui.QApplication.UnicodeUTF8))
        self.act_finish.setText(QtGui.QApplication.translate("main_window", "&Finish", None, QtGui.QApplication.UnicodeUTF8))
        self.act_continue.setText(QtGui.QApplication.translate("main_window", "&Continue", None, QtGui.QApplication.UnicodeUTF8))

