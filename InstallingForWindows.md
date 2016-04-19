# Introduction #

To be able to develop Tonatiuh under Windows XP, Windows Vista or Windows 7, you need to install the MSYS shell environment, the MinGW C++ compiler, and a series of libraries. You will also need to install and configure an Integrated Development Environment, and the ancillary libraries and software infrastructure needed to automatize the generation of the program's documentation, to do unit testing and continuous integration.

Here you will find and overview of the overall process, together with links to the detailed instructions needed to execute every step of the process.

# Details #

To be able to develop Tonatiuh under Windows you need to:

  * Install the appropriate compilation and linking environment.
    * [Installing MinGW and MSYS](InstallingMinGWForWindows.md)

  * Install the third-party libraries needed by the program.
    * [Installing Qt](InstallingQtForWindows.md)
    * [Installing Simage](InstallingSimageForWindows.md)
    * [Installing Coin3D](InstallingCoin3DForWindows.md)
    * [Installing SoQt](InstallingSoQt.md)
    * [Installing Marble ](InstallingMarbleForWindows.md)
    * [Installing SQLite](InstallingQSLiteForWindows.md)


  * Install and configure the Integrated Development Environment.
    * [Installing and Configuring Eclipse](InstallingAndConfiguringEclipseForWindows.md)
    * [Installing Subversive](Installing_Subversive.md)
    * [Building Tonatiuh and plugins](BuildingTonatiuhUnderWindows.md)
    * [Debugging Tonatiuh with Eclipse](DebuggingTonatiuhOnWindiows.md)

  * Install Doxygen and configure it.
    * [Installing Doxygen](InstallingDoxygenForWindows.md)

  * Install the Unit Testing and Continuous Integration infrastructure.
    * [Install GoogleTest](InstallingGoogleTestForWindows.md)



[Developer's Guide](http://code.google.com/p/tonatiuh/wiki/DevelopersGuide) | [Wiki Home](http://code.google.com/p/tonatiuh/w/list)