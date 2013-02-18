Installing the 2012 Vision code (updated to 2013 standards)
-------------------------------

The FRC folks at WPI have provided the following documentation.

http://wpilib.screenstepslive.com/s/3120/m/7885/l/79405-installing-the-java-development-tools

Here are my 'shortened' versions of the docs.

Step 1: - Install pre-requisite software
-------
On your local workstation, ensure the following software is installed.

- Java 7 (Version 1.7.0_13 is the latest as of the time of this document)
- Netbeans (version 7.2.1 is the latest as of the time of this document)
Both of the above are downloaded over the net from Oracle.
- ANT (optional) (Version 1.8.4 is the latest as of the time of this document)

Add the Netbeans Plugin location:
********************************
Tools -> Plugins -> Settings
  [ Add ] a new Update Center

  Name: FRC Plugins
  URL:  http://first.wpi.edu/FRC/java/netbeans/update/Release/updates.xml

Install the FRC plugins:
***********************
Select the 'Available Plugins' tab, and then click 'Check For Updates'.
In the window below, check the 6 plugins in the 'FRC Java' category
and then click the 'Install' button.

Configure the FRC plugins:
*************************
The only configuration necessary is to set the Team number in the plugin.

Prefences -> Miscellaneous -> FRC Configuration
Team Number: 1695

If the workstation is going to be used for working with the cRIO, you'll still
need to install some additional LabVIEW components such as the Driver Station,
Imaging Tool, and others onto your system.  Contact Mr. Miller for the DVD
containing the software.

Step 2: Import/build projects
-------
Import the two Netbean projects available on the website into Netbeans

File -> Open Project -> [ Browse to ShooterRobot ]
File -> Open Project -> [ Browse to Widgets ]

Choose ShooterRobot and then run the build target (Shift F11)
Choose Widgets and then run the build target (Shift F11)

Step 3: - Base SmartDashboard installation/configuration
-------
This must be installed *after* the normal LabVIEW components have been
installed (DriverStation in particular).

Download the latest version of SmarthDashboard from the following link (1.0.4
is provided in the SmarthDashboard sub-directory).
http://firstforge.wpi.edu/sf/frs/do/listReleases/projects.smartdashboard/frs.installer

This installs the program to 'C:\Program Files\SmartDashboard'.

The file C:\Users\Public\Documents\FRC\FRC DS Data Storage.ini needs to
be edited.  Modify the line starting with 'DashboardCmdLine'.

It will probably look something like:
DashboardCmdLine = ""C:\\Program Files (x86)\\FRC Dashboard\\Dashboard.exe""

Modify it to look like:
DashboardCmdLine = ""C:\\Program Files\\SmartDashboard\\SmartDashboard.exe""

Pay attention to the quotes and backslashes to ensure the correct number are
used.

Startup DriverStation, and make sure that the 'Default' button is chosen
(do not choose Java).

A second window should now popup with the 'SmartDashboard' title.  We have
not (yet) modified the default configuration.

Step 4: - Custom SmarthDashboard configuration
-------
In step 2, we build the custom Widgets for the SmartDashboard (hereafter
referenced as SD) to do computer vision processing from the camera.  This
allows us to analyze the data and send the results to the robot's cRIO
board as well as display to the drivers what the robot is seeing.

The cutstom processing software is contained in the 'Widgets' projects, so
to install software, locate the Widgets.jar in the dist' directory under
the Widgets project.  Also, in the 'dist/lib' directory, you will find a
number of 3rd party library files that are used for camera image access
and logging.

Copy the Widgets.jar file to the SD custom widget directory 
  C:\Program Files\SmartDashboard\extensions
and copy all the third party libraries to the library directory
  C:\Program Files\SmartDashboard\extensions\lib

At this point, the custom behavior for accessing the camera is now in-place.

Step 5: - Configure camera
------
XXX - Need instructions here

Step 6: - Install software onto Robot (cRIO)
-------
First, verify the workstation is connected to the robot properly (using
either wired/wireless network setup, whichever is easiest to configure).

Choose the Netbeans ShooterRobot project, and 'Run' the project (F6),
which compiles and downloads the project to the robot.  Ensure this process
completes without errors.

Step 7:
-------
Verify the program is working.  Startup the DriverStation, verify the
SmartDashboard window show up, and that the camera view shows up properly.
