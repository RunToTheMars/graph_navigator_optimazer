QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    abstract_axis_painter.cpp \
    abstract_painter.cpp \
    editable_graph_area_render.cpp \
    editor_settings_widget.cpp \
    graph_axis_model.cpp \
    graph_editor_tab.cpp \
    graph_painter.cpp \
    main.cpp \
    main_window.cpp \
    render_area_widget.cpp \
    thread_pool.cpp

HEADERS += \
    abstract_axis_model.h \
    abstract_axis_painter.h \
    abstract_painter.h \
    container_manager.h \
    editable_graph_area_render.h \
    editor_settings_widget.h \
    gno_graph.h \
    gno_graph_fwd.h \
    gno_graph_initial.h \
    gno_graph_initial_state.h \
    graph_axis_model.h \
    graph_editor_tab.h \
    graph_painter.h \
    main_window.h \
    render_area_widget.h \
    thread_pool.h \
    utils.h

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
 
