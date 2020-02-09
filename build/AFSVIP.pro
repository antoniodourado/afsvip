TEMPLATE = app
CONFIG += console c++11 -DBOOST_LOG_DYN_LINK
CONFIG -= app_bundle
CONFIG -= qt
DEFINES += BOOST_LOG_DYN_LINK

INCLUDEPATH += "/usr/local/include"
INCLUDEPATH += "/usr/local/cuda/include"
INCLUDEPATH += "/usr/include/libusb-1.0"

LIBS += -lopencv_core -lopencv_imgproc -lopencv_highgui -lopencv_calib3d -lopencv_features2d -lopencv_imgcodecs -lopencv_videoio -lopencv_cudafilters -lpthread -lcaffe -lboost_system -lglog -lsphinxad -llapack -lpocketsphinx -lsphinxbase -lespeak -lopenal -lalut -lconfig++ -lsl_zed -lsl_core -lasound -levdev -linput -ludev -lncurses -lusb-1.0 -lboost_thread -lboost_log -lboost_log_setup -lboost_date_time -lboost_chrono -lboost_atomic -lrealsense2 -lboost_program_options
LIBS += -L"/usr/local/lib" -L"/usr/lib/aarch64-linux-gnu"

DISTFILES += \
    AFSVIP.pro.user

VPATH += ../src

HEADERS += \
    AudioFeedback.h \
    AudioSoundClass.h \
    CVLayer.h \
    CaffeClassifier.h \
    CodeUtil.h \
    ConfigHandler.h \
    Configuration.h \
    DetectedObject.h \
    DepthCamera.h \
    DepthCameraInitializer.h \
    FeedbackLayer.h \
    jetsonGPIO.h \
    Kinect.h \
    PILayer.h \
    PSContinuous.h \
    RawToWavConverter.h \
    RealSense435i.h \
    TactileFeedback.h \
    VILayer.h \
    VoiceFeedback.h \
    VoiceHypotesisProcessor.h \
    Zed.h \
    __afsvipGLOBALS.h

SOURCES += \
    AudioFeedback.cpp \
    AudioSoundClass.cpp \
    CVLayer.cpp \
    CaffeClassifier.cpp \
    CodeUtil.cpp \
    ConfigHandler.cpp \
    Configuration.cpp \
    DetectedObject.cpp \
    DepthCameraInitializer.cpp \
    FeedbackLayer.cpp \
    jetsonGPIO.cpp \
    Kinect.cpp \
    PILayer.cpp \
    PSContinuous.cpp \
    RawToWavConverter.cpp \
    RealSense435i.cpp \
    TactileFeedback.cpp \
    VILayer.cpp \
    VoiceFeedback.cpp \
    VoiceHypotesisProcessor.cpp \
    Zed.cpp \
    __afsvipGLOBALS.cpp \
    main.cpp
