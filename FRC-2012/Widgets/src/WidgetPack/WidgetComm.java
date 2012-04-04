package robotics.helena.widget;

import edu.wpi.first.smartdashboard.robot.Robot;
import edu.wpi.first.wpilibj.networking.NetworkTable;

import java.util.ArrayList;

public class WidgetComm extends Thread{
	public static NetworkTable table = Robot.getTable();
    private static ArrayList<VarNotifier> notifiersList = new ArrayList<VarNotifier>();
	
	//Variables for access
	public static Integer mode;
	public static Boolean transmission;
	public static Integer timer;
	public static Double fieldX;
	public static Double fieldY;
	public static Double rpm1;
	public static Double rpm2;
	public static Double j1Dir;
	public static Double j1Magn;
	public static Double j2X;
	public static Double j2Y;
	public static Double j2Throttle;
	public static Boolean ballFeeder;
	public static Double accelX;
	public static Double accelY;
	public static Double accelZ;
	public static Double gyro;
    
    //Constants to detirmine types of names
    public static int INTEGER = 0;
    public static int BOOLEAN = 1;
    public static int DOUBLE = 2;
    public static int UNDEF = 3;

    public void run(){
        while(true){
            try{
                Thread.sleep(500); //Sleep some arbitrary amound of time
            }catch(Exception e){}
            table = Robot.getTable();

            mode = getInt("Mode");
            transmission = getBoolean("Transmission");
            timer = getInt("Timer");
            fieldX = getDouble("Field X");
            fieldY = getDouble("Field Y");
            rpm1 = getDouble("RPM 1");
            rpm2 = getDouble("RPM 2");
            j1Dir = getDouble("Joystick 1 Direction");
            j1Magn = getDouble("Joystick 1 Magnitude");
            j2X = getDouble("Joystick 2 X");
            j2Y = getDouble("Joystick 2 Y");
            j2Throttle = getDouble("Joystick 2 Throttle");
            ballFeeder = getBoolean("BallFeeder");
            accelX = getDouble("Accel X");
            accelY = getDouble("Accel Y");
            accelZ = getDouble("Accel Z");
            gyro = getDouble("Gyro");

            callNotifiers();
        }
    }

	public static Double getDouble(String key){
		if(table.containsKey(key))
			return table.getDouble(key);
		else
			return null;
	}
	
	public static Integer getInt(String key){
		if(table.containsKey(key))
			return table.getInt(key);
		else
			return null;
	}
	
	public static Boolean getBoolean(String key){
		if(table.containsKey(key))
			return table.getBoolean(key);
		else
			return null;
	}
    
    public static int typeFromName(String name){
        int ret = UNDEF;
        if(formalKey(name) != null)
        if(table.containsKey(formalKey(name))){
            if(name.equals("mode"))
                ret = INTEGER;

            if(name.equals("transmission"))
                ret = BOOLEAN;

            if(name.equals("timer"))
                ret = INTEGER;

            if(name.equals("fieldX"))
                ret = DOUBLE;

            if(name.equals("fieldY"))
                ret = DOUBLE;

            if(name.equals("rpm1"))
                ret = DOUBLE;

            if(name.equals("rpm2"))
                ret = DOUBLE;

            if(name.equals("j1Dir"))
                ret = DOUBLE;

            if(name.equals("j1Magn"))
                ret = DOUBLE;

            if(name.equals("j2X"))
                ret = DOUBLE;

            if(name.equals("j2Y"))
                ret = DOUBLE;

            if(name.equals("j2Throttle")){
                ret = DOUBLE;
            }

            if(name.equals("ballFeeder"))
                ret = BOOLEAN;

            if(name.equals("accelX"))
                ret = DOUBLE;

            if(name.equals("accelY"))
                ret = DOUBLE;

            if(name.equals("accelZ"))
                ret = DOUBLE;

            if(name.equals("gyro"))
                ret = DOUBLE;
        }

        return ret;
    }
    
    public static String formalKey(String name){
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

        if(name.equals("fieldX"))
            ret = fieldX;

        if(name.equals("fieldY"))
            ret = fieldY;

        if(name.equals("rpm1"))
            ret = rpm1;

        if(name.equals("rpm2"))
            ret = rpm2;

        if(name.equals("j1Dir"))
            ret = j1Dir;

        if(name.equals("j1Magn"))
            ret = j1Magn;

        if(name.equals("j2X"))
            ret = j2X;

        if(name.equals("j2Y"))
            ret = j2Y;

        if(name.equals("j2Throttle"))
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

        if(name.equals("mode"))
            ret = mode;

        if(name.equals("timer"))
            ret = timer;

        return ret;
    }

    public static Boolean getBooleanFromName(String name){
        Boolean ret = null;

        if(name.equals("transmission"))
            ret = transmission;

        if(name.equals("ballFeeder"))
            ret = ballFeeder;

        return ret;
    }

    public static void registerVarNotifier(VarNotifier add){
        notifiersList.add(add);
    }

    private static void callNotifiers(){
        for(int i = 0; i < notifiersList.size(); i++){
            notifiersList.get(i).varUpdate();
        }
        System.out.println("Notified...");
    }

	//Make Class run as soon as possible
    private static boolean initialized = false;
    public static void initialize(){
        if(!initialized)
            new WidgetComm();
    }

    private WidgetComm(){
        super("WidgetComm");
        start();
    }
}