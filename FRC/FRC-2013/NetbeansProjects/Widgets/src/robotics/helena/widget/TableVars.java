package robotics.helena.widget;

import edu.wpi.first.smartdashboard.robot.Robot;
import edu.wpi.first.wpilibj.tables.ITable;

public class TableVars {
    public static ITable table = Robot.getTable();

    public static String mode = "Mode";
    public static String transmission = "Transmission";
    public static String timer = "Timer";
    public static String fieldX = "Field X";
    public static String fieldY = "Field Y";
    public static String rpm1 = "RPM 1";
    public static String rpm2 = "RPM 2";
    public static String rpm1Target = "RPM 1 Target";
    public static String rpm2Target = "RPM 2 Target";
    public static String j1Dir = "Joystick 1 Direction";
    public static String j1Magn = "Joystick 1 Magnitude";
    public static String j2X = "Joystick 2 X";
    public static String j2Y = "Joystick 2 Y";
    public static String j2Throttle = "Joystick 2 Throttle";
    public static String ballFeeder = "BallFeeder";
    public static String accelX = "Accel X";
    public static String accelY = "Accel Y";
    public static String accelZ = "Accel Z";
    public static String gyro = "Gyro";

    //Constants to detirmine types of names
    public static int INTEGER = 0;
    public static int BOOLEAN = 1;
    public static int DOUBLE = 2;
    public static int UNDEF = 3;


    //Only useful function below line is directly below
    public static int typeFromName(String name) {
        int ret = UNDEF;
        if (table.containsKey(name)) {
            if (name.equals("Mode"))
                ret = INTEGER;

            if (name.equals("Transmission"))
                ret = BOOLEAN;

            if (name.equals("Timer"))
                ret = INTEGER;

            if (name.equals("Field X"))
                ret = DOUBLE;

            if (name.equals("Field Y"))
                ret = DOUBLE;

            if (name.equals("RPM 1"))
                ret = DOUBLE;

            if (name.equals("RPM 2"))
                ret = DOUBLE;

            if (name.equals("Joystick 1 Direction"))
                ret = DOUBLE;

            if (name.equals("Joystick 1 Magnitude"))
                ret = DOUBLE;

            if (name.equals("Joystick 2 X"))
                ret = DOUBLE;

            if (name.equals("Joystick 2 Y"))
                ret = DOUBLE;

            if (name.equals("Joystick 2 Throttle")) {
                ret = DOUBLE;
            }

            if (name.equals("BallFeeder"))
                ret = BOOLEAN;

            if (name.equals("Accel X"))
                ret = DOUBLE;

            if (name.equals("Accel Y"))
                ret = DOUBLE;

            if (name.equals("Accel Z"))
                ret = DOUBLE;

            if (name.equals("Gyro"))
                ret = DOUBLE;
        }

        return ret;
    }


    /*public static String formalKey(String name){
        String ret = null;
        if("mode".equals(name))
            ret ="Mode";

        if("transmission".equals(name))
            ret ="Transmission";

        if("timer".equals(name))
            ret ="Timer";

        if("fieldX".equals(name))
            ret ="Field X";

        if("fieldY".equals(name))
            ret ="Field Y";

        if("rpm1".equals(name))
            ret ="RPM 1";

        if("rpm2".equals(name))
            ret ="RPM 2";

        if("j1Dir".equals(name))
            ret ="Joystick 1 Direction";

        if("j1Magn".equals(name))
            ret ="Joystick 1 Magnitude";

        if("j2X".equals(name))
            ret ="Joystick 2 X";

        if("j2Y".equals(name))
            ret ="Joystick 2 Y";

        if("j2Throttle".equals(name))
            ret ="Joystick 2 Throttle";

        if("ballFeeder".equals(name))
            ret ="BallFeeder";

        if("accelX".equals(name))
            ret ="Accel X";

        if("accelY".equals(name))
            ret ="Accel Y";

        if("accelZ".equals(name))
            ret ="Accel Z";

        if("gyro".equals(name))
            ret ="Gyro";

        return ret;
    }

    public static Double getDoubleFromName(String name){
        Double ret = null;

        if(name.equals("Field X"))
            ret = fieldX;

        if(name.equals("Field Y"))
            ret = fieldY;

        if(name.equals("RPM 1"))
            ret = rpm1;

        if(name.equals("RPM 2"))
            ret = rpm2;

        if(name.equals("Joystick 1 Direction"))
            ret = j1Dir;

        if(name.equals("Joystick 1 Magnitude"))
            ret = j1Magn;

        if(name.equals("Joystick 2 X"))
            ret = j2X;

        if(name.equals("Joystick 2 Y"))
            ret = j2Y;

        if(name.equals("Joystick 2 Throttle"))
            ret = j2Throttle;

        if(name.equals("Accel X"))
            ret = accelX;

        if(name.equals("Accel Y"))
            ret = accelY;

        if(name.equals("Accel Z"))
            ret = accelZ;

        if(name.equals("Gyro"))
            ret = gyro;

        return ret;
    }

    public static Integer getIntegerFromName(String name){
        Integer ret = null;

        if(name.equals("Mode"))
            ret = mode;

        if(name.equals("Timer"))
            ret = timer;

        return ret;
    }

    public static Boolean getBooleanFromName(String name){
        Boolean ret = null;

        if(name.equals("Transmission"))
            ret = transmission;

        if(name.equals("BallFeeder"))
            ret = ballFeeder;

        return ret;
    }*/

    //Make Class run as soon as possible

}