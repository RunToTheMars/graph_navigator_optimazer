QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    abstract_axis_painter.cpp \
    abstract_painter.cpp \
    algorithm_base.cpp \
    color_builder.cpp \
    editable_graph_area_render.cpp \
    editor_settings_widget.cpp \
    gno_graph.cpp \
    gno_modeling.cpp \
    gno_modeling_simple_acceleration.cpp \
    gno_modeling_simple_on_edge.cpp \
    gno_modeling_star.cpp \
    gno_path_finder_brute_force.cpp \
    gno_path_finder_dijkstra.cpp \
    gno_test.cpp \
    graph_axis_model.cpp \
    graph_painter.cpp \
    main.cpp \
    main_window.cpp \
    model_graph_area_render.cpp \
    model_graph_widget.cpp \
    path_graph_widget.cpp \
    render_area_widget.cpp \
    tab_graph_editor.cpp \
    tab_graph_modeling.cpp \
    tab_graph_path.cpp \
    thread_pool.cpp \
    veh_on_graph_painter.cpp

HEADERS += \
    abstract_axis_model.h \
    abstract_axis_painter.h \
    abstract_painter.h \
    algorithm_base.h \
    color_builder.h \
    editable_graph_area_render.h \
    editor_settings_widget.h \
    gno_graph.h \
    gno_graph_fwd.h \
    gno_graph_initial.h \
    gno_graph_initial_state.h \
    gno_modeling.h \
    gno_modeling_simple_acceleration.h \
    gno_modeling_simple_on_edge.h \
    gno_modeling_star.h \
    gno_path_finder.h \
    gno_path_finder_brute_force.h \
    gno_path_finder_dijkstra.h \
    gno_test.h \
    graph_axis_model.h \
    graph_painter.h \
    main_window.h \
    model_graph_area_render.h \
    model_graph_widget.h \
    path_graph_widget.h \
    render_area_widget.h \
    tab_graph_editor.h \
    tab_graph_modeling.h \
    tab_graph_path.h \
    thread_pool.h \
    utils.h \
    veh_on_graph_painter.h

FORMS +=

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

QMAKE_EXTRA_TARGETS += before_build makefilehook
 
makefilehook.target = $(MAKEFILE)
makefilehook.depends = .beforebuild
 
PRE_TARGETDEPS += .beforebuild
 
before_build.target = .beforebuild
before_build.depends = FORCE
before_build.commands = chcp 1251
 
