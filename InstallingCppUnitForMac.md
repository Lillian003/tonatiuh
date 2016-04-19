# CppUnit #

CppUnit is the C++ port of the famous JUnit framework for unit testing. Test output is in XML or text format for automatic testing and GUI based for supervised tests.


First, download source files from [CppUnit Project Site](http://sourceforge.net/projects/cppunit/). Place into your desktop directory. Open a console shell and navigate to your desktop directory . Extract archive file typing :
```
 tar xvf cppunit-1.12.1.tar
```

Now, configure and install:
```
 cd cppunit-1.12.1
./configure
make 
make install
```

## QxRunner ##

[QxRunner](http://qxrunner.systest.ch/qxrunner/) is a library to develop C++ GUI applications to execute user selected items from a list.

The QxRunner for CppUnit (QxCppUnit) library provides a GUI frontend for CppUnit test execution, shortly called the QxTestRunner.

Download library source files from [QxRunner Project Site](http://qxrunner.systest.ch/common/download.htm). The current version is qxrunner 0.9.2.Extract files tiping the following command in a shell:
```
 tar xvf IbRwPa.tar
```

Open with a text editor qxconfig.pro file in just created folder. Delete:

```
 line 253: win32:dll{
line 254: QX_RUNNERFILENAME=$${QX_RUNNERFILENAME}_dll
.
.
.
line 292: QX_CPPUNITFILENAME=$${QX_CPPUNITFILENAME}_shared
line 293: }
```

To configure and install:
```

 cd qxrunner
qmake qxrunner_all.pro CONFIG+=debug_dll
make
cp lib/* /usr/local/lib
csh make_clean
qmake qxrunner_all.pro CONFIG+=release_all
make
cp lib/* /usr/local/lib
cp -r include/* /usr/local/include
```

[Mac Configuration ](InstallingForMac.md) |[Wiki Home](http://code.google.com/p/tonatiuh/w/list)