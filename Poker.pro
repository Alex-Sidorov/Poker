TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
        main.cpp \
    CardsModule/abstractcardsshuffle.cpp \
    CardsModule/uniformdistributionshuffle.cpp

HEADERS += \
    CardsModule/card.h \
    CardsModule/abstractcardsshuffle.h \
    CardsModule/uniformdistributionshuffle.h \
    CardsModule/abstractreferee.h \
    CardsModule/referee.h \
    CardsModule/combinations.h
