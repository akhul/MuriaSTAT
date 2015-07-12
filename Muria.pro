#-------------------------------------------------
#
# Project created by QtCreator 2015-04-21T01:02:24
#
#-------------------------------------------------

QT       += core gui
QT += webkit

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Muria
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    console.cpp \
    variableeditor.cpp \
    activedataset.cpp \
    properties.cpp \
    menu.cpp \
    dataeditor.cpp \
    rcore.cpp \
    open.cpp \
    tabeditor.cpp \
    test.cpp \
    buttoncustom.cpp \
    openpreview.cpp \
    slregression.cpp \
    regressionr.cpp \
    save.cpp \
    dmsortingdialog.cpp \
    dmrecodedialog.cpp \
    dmaggregatedialog.cpp \
    dm_mergebycoldialog.cpp \
    datamanagement.cpp

HEADERS  += mainwindow.h \
    console.h \
    variableeditor.h \
    activedataset.h \
    properties.h \
    menu.h \
    dataeditor.h \
    rcore.h \
    open.h \
    tabeditor.h \
    test.h \
    buttoncustom.h \
    openpreview.h \
    slregression.h \
    regressionr.h \
    save.h \
    dmsortingdialog.h \
    dmrecodedialog.h \
    dmaggregatedialog.h \
    dm_mergebycoldialog.h \
    datamanagement.h

FORMS    += mainwindow.ui \
    openpreview.ui \
    slregression.ui \
    dmsortingdialog.ui \
    dmrecodedialog.ui \
    dmaggregatedialog.ui \
    dm_mergebycoldialog.ui


R_HOME = 		$$system(R RHOME)
## include headers and libraries for R
RCPPFLAGS = 		$$system($$R_HOME/bin/R CMD config --cppflags)
RLDFLAGS = 		$$system($$R_HOME/bin/R CMD config --ldflags)
RBLAS = 		$$system($$R_HOME/bin/R CMD config BLAS_LIBS)
RLAPACK = 		$$system($$R_HOME/bin/R CMD config LAPACK_LIBS)

## if you need to set an rpath to R itself, also uncomment
RRPATH =		-Wl,-rpath,$$R_HOME/lib

## include headers and libraries for Rcpp interface classes
## note that RCPPLIBS will be empty with Rcpp (>= 0.11.0) and can be omitted
RCPPINCL = 		$$system($$R_HOME/bin/Rscript -e \"Rcpp:::CxxFlags\(\)\")
RCPPLIBS = 		$$system($$R_HOME/bin/Rscript -e \"Rcpp:::LdFlags\(\)\")

## for some reason when building with Qt we get this each time
##   /usr/local/lib/R/site-library/Rcpp/include/Rcpp/module/Module_generated_ctor_signature.h:25: warning: unused parameter ‘classname
## so we turn unused parameter warnings off
## no longer needed with Rcpp 0.9.3 or later
#RCPPWARNING =		-Wno-unused-parameter

## include headers and libraries for RInside embedding classes
RINSIDEINCL = 		$$system($$R_HOME/bin/Rscript -e \"RInside:::CxxFlags\(\)\")
RINSIDELIBS = 		$$system($$R_HOME/bin/Rscript -e \"RInside:::LdFlags\(\)\")

## compiler etc settings used in default make rules
QMAKE_CXXFLAGS +=	$$RCPPWARNING $$RCPPFLAGS $$RCPPINCL $$RINSIDEINCL
QMAKE_LIBS +=           $$RLDFLAGS $$RBLAS $$RLAPACK $$RINSIDELIBS $$RCPPLIBS

## addition clean targets
QMAKE_CLEAN +=		TestSpreadsheet Makefile

RESOURCES += \
    image.qrc

