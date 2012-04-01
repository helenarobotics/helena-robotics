import java.net.InetAddress;
import java.net.URL;

public class ConnectionHandler{
    private URL connectionURL;
    private int delay;
    private int numThreads;

    private boolean connection = false; //assume no connection until a result is given
    private boolean running = false;
    private boolean gotResult = false;
    private ConnectionChecker[] checkers;

    private final int DEFAULT_DELAY = 5000;
    private final int DEFAULT_NUMBER_OF_THREADS = 5;

    public ConnectionHandler(URL connection, int delayMilliSecs, int numberOfThreads){
        connectionURL = connection;
        delay = delayMilliSecs;
        checkers = new ConnectionChecker[numberOfThreads];
    }

    public ConnectionHandler(URL connection){
        connectionURL = connection;
        delay = DEFAULT_DELAY;
        checkers = new ConnectionChecker[DEFAULT_NUMBER_OF_THREADS];
    }

    public void start(){
        if(!running){
            for(int i = 0; i < checkers.length; i++){
                checkers[i] = new ConnectionChecker();
                Thread t = new Thread(checkers[i], "Connection Checker " + Integer.toString(i+1));
                t.start();
                try{
                    Thread.sleep(delay/checkers.length);
                }catch(Exception e){}
            }
            running = true;
        }
    }

    public void stop(){
        if(running){
            for(int i = 0; i < checkers.length; i++){
                checkers[i].stop();
            }
            running = false;
            gotResult = false;
        }
    }

    public boolean isRunning(){
        return running && gotResult;
    }

    public boolean isConnected(){
        return connection;
    }

    class ConnectionChecker implements Runnable{
        private boolean running;

        public void run(){
            running = true;
            while(running){
                boolean connected;
                try{
                    if(InetAddress.getByName(getURL().getHost()).isReachable(getDelay()))
                        connected = true;
                    else
                        connected = false;
                } catch(Exception ex){ connected = false;}
                setConnectionValue(connected);
            }
        }

        public void stop(){
            running = false;
        }
    }

    private synchronized void setConnectionValue(boolean set){
        connection = set;
        gotResult = true;
    }

    private synchronized URL getURL(){
        return connectionURL;
    }

    private synchronized int getDelay(){
        return delay;
    }
}
