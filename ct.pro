MOC_DIR = .moc
OBJECTS_DIR = .obj
QMAKE_LFLAGS += `pkg-config opencv --cflags --libs`
QMAKE_CXXFLAGS += -std=c++0x
QMAKE_LIBS += `pkg-config opencv --cflags --libs`


TEMPLATE = app
CONFIG += qt warn_on release thread
SOURCES = ct.cpp\
CppMT/CMT.cpp\
CppMT/common.cpp\
CppMT/Consensus.cpp\
CppMT/gui.cpp\
CppMT/Tracker.cpp\
CppMT/Matcher.cpp\
CppMT/Fusion.cpp\
CppMT/fastcluster/fastcluster.cpp\
counter.hpp

TARGET = ct
CONFIG -= app_bundle

INCLUDEPATH += /usr/local/include/opencv2/

LIBS = -L/usr/local/lib/ -lopencv_highgui -lopencv_imgproc -lopencv_core -lopencv_features2d -lopencv_flann -lopencv_calib3d

  
