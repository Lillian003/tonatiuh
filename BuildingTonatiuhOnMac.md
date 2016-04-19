# Building Tonatiuh #

To start builing Tonatiuh under Eclipse, you need to download the source code from the repository. Follow the [Subversive installation instruccions](Installing_Subversive.md) to install SVN integration for Eclipse. Then, define Tonatiuh repository  by reading  [Anonymous\_Download](Anonymous_Download.md) to anonymous download of Tonatiuh code or reading  [Upload\_to\_Repository](Upload_to_Repository.md) to download code and upload changes.

## Tonatiuh Application ##
First, build Tonatiuh.
  * In the Project Explorer tab, right click on the Tonatiuh project.
  * Select properties.
  * Expand C/C++ Build and select Environment properties.
![http://tonatiuh.googlecode.com/svn/wiki/images/eclipse/tonatiuhPropertiesMac.png](http://tonatiuh.googlecode.com/svn/wiki/images/eclipse/tonatiuhPropertiesMac.png)
  * Add new variable:
    * Name: TDE\_ROOT, Value: /usr.
    * Name: TONATIUH\_ROOT, Value: /Applications/eclipse/workspace/TonatiuhProject.
  * On Settings select Mach-O Parser.
  * Apply changes. Ok to finish with properties.
  * On the Make Target tab, right click Tonatiuh and select Add Make Target.
![http://tonatiuh.googlecode.com/svn/wiki/images/eclipse/qmakeTargetMac.png](http://tonatiuh.googlecode.com/svn/wiki/images/eclipse/qmakeTargetMac.png)
  * Define the first Make Target as:
    * Target Name: qmake
    * Make Target: src/Tonatiuh.pro
    * Build command: /usr/local/Trolltech/Qt-4.6.2/bin/qmake -spec macx-g++
  * Add new Make Target as:
    * Target Name: all
    * Make Target: all
    * Build command: make
  * To generate Makefile run "qmake". When the qmake command finishes run "all".

[Mac Configuration](InstallingForMac.md) | [Developer's Guide](http://code.google.com/p/tonatiuh/wiki/DevelopersGuide) | [Wiki Home](http://code.google.com/p/tonatiuh/w/list)