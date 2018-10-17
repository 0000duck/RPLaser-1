#-------------------------------------------------
#
# Project created by QtCreator 2015-04-10T14:23:43
#
#-------------------------------------------------

QT       += core gui \
            opengl   \
            xml
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = LCLaser
TEMPLATE = app

#DEFINES = WIN32

TRANSLATIONS = mui.ts

SOURCES += \
    src/mainwindow.cpp \
    src/main.cpp \
    src/builder/control.cpp \
    src/builder/tabcontrol.cpp \
    src/dataprocess/tabdata.cpp \
    src/display/display.cpp \
    src/display/draw.cpp \
    src/display/drawSlice.cpp \
    src/display/glwin.cpp \
    src/display/graph.cpp \
    src/display/tabgl.cpp \
    src/IO/ioslc.cpp \
    src/IO/formatSTL.cpp \
    src/dataprocess/polygonMesh.cpp \
    src/dataprocess/data.cpp \
    src/simulator/gtmock.cpp \
    src/display/drawAnimation.cpp \
    src/simulator/simuthread.cpp \
    src/simulator/simu.cpp \
    src/builder/buffthread.cpp \
    src/utils/config.cpp \
    src/utils/initialization.cpp \
    src/display/drawProcess.cpp \
    src/dataprocess/dataSlice.cpp \
    src/dataprocess/handelBallOutline.cpp \
    src/dataprocess/dataDrawTrack.cpp \
    src/dataprocess/datathread.cpp \
    src/dataprocess/handelscanpath.cpp \
    src/display/overlayscene.cpp \
    src/utils/matrix4x4.cpp \
    src/builder/pulselaser.cpp \
    src/builder/analoglaser.cpp \
    src/utils/Vec3D.cpp \
    src/dataprocess/triangleMesh.cpp \
    src/display/drawMesh.cpp \
    src/dataprocess/measure.cpp \
    src/dataprocess/measureAlignBtn.cpp \
    src/rpinterface.cpp \
    src/pluginManager.cpp \
    src/display/wizardbuttons.cpp \
    LGE400/ges.cpp \
    src/IO/formatPLY.cpp \
    src/IO/formatOBJ.cpp

HEADERS  += \
    src/mainwindow.h \
    src/builder/control.h \
    src/builder/tabcontrol.h \
    src/dataprocess/database.h \
    src/dataprocess/tabdata.h \
    src/display/display.h \
    src/display/displaybase.h \
    src/display/draw.h \
    src/display/drawSlice.h \
    src/display/glwin.h \
    src/display/graph.h \
    src/display/tabgl.h \
    src/IO/ioslc.h \
    src/IO/formatSTL.h \
    src/utils/hashtable.h \
    src/utils/standartHashFunction.h \
    src/utils/PoolAllocator.h \
    src/dataprocess/polygonMesh.h \
    src/dataprocess/data.h \
    src/simulator/gtmock.h \
    src/display/drawAnimation.h \
    src/utils/Vec3D.h \
    src/simulator/simuthread.h \
    src/simulator/simu.h \
    src/builder/buffthread.h \
    src/utils/config.h \
    src/builder/command.h \
    src/utils/initialization.h \
    src/display/drawProcess.h \
    src/dataprocess/dataSlice.h \
    src/dataprocess/dataDynamic.h\
    src/dataprocess/handelBallOutline.h \
    src/dataprocess/dataDrawTrack.h \
    src/dataprocess/datathread.h \
    src/dataprocess/handelscanpath.h \
    src/display/overlayscene.h \
    src/utils/Vec2D.h \
    src/utils/matrix4x4.h \
    src/builder/pulselaser.h \
    src/builder/analoglaser.h \
    src/utils/Vec4D.h \
    src/dataprocess/triangleMesh.h \
    src/display/drawMesh.h \
    src/dataprocess/measure.h \
    src/dataprocess/measureAlignBtn.h \
    src/rpinterface.h \
    src/pluginManager.h \
    src/builder/controlinterface.h \
    src/utils/threadlog.h \
    src/display/wizardbuttons.h \
    LGE400/ges.h \
    src/IO/formatOBJ.h \
    src/IO/formatPLY.h
   #LGE400/ges.h


FORMS    += \
    Forms/mainwindow.ui \
    Forms/tabdata.ui \
    Forms/tabgl.ui \
    Forms/tabcontrol.ui \
    Forms/pulselaser.ui \
    Forms/analoglaser.ui \
    Forms/measureAlignBtn.ui \
    Forms/pluginManager.ui

RESOURCES += \
    resource/resource.qrc

RC_FILE += Icon//RPLaser.rc

INCLUDEPATH += src/ \
               src/builder \
               src/dataprocess \
 	       src/display \
 	       src/IO \
 	       src/simulator \
	       src/utils \
               plugin_src/base_plugin\
               LGE400
              #LGE400

#LIBS += /lib64/libGLU.so.lib
#LIBS += $${PWD}/GE400/ges.lib

DISTFILES += \
    resource/splash.png \
    resource/pluginManager.png \

OTHER_FILES += \
    Icon/RPLaser.ico \
    Icon/RPLaser.rc

win32:LIBS += -lOpengl32 \
                -lglu32 \
                -lglut
