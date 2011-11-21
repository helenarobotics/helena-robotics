import java.text.DecimalFormat;
import java.util.ArrayList;

public class Routine{
    protected final ArrayList<RTask> tasks = new ArrayList<RTask>();
    DecimalFormat d = new DecimalFormat("#.##");
    public Routine(){

    }

    public Routine(String data){
		char[] input = data.toCharArray();
		int taskLoc = 0;
		int loc = 0;
		int vLoc = 0;
		float val[] = new float[5];
		String buf = "";
		for(int i = 0; i < val.length; i++)
			val[i] = -1;

		for(int i = 0; i < input.length; i++){
			if(input[i] == ' ')	{
				for(int j = 0; j < i; j++){
					buf = buf + input[j];
				}
				taskLoc = Integer.parseInt(buf);
				loc = i+1;
                i = input.length;
			}
		}

        buf = "";

		while(taskLoc != 0){
			if(input[loc] == 'Q'){
			 	addTask(new RTask(val));
				for(int i = 0; i < val.length; i++){
					val[i] = -1;
                }
				vLoc = 0;
				taskLoc--;
				loc++;
			}else if(input[loc] == ','){
                loc++;
            }else if (input[loc] != ','){
				while(input[loc] != 'Q' && input[loc] != ','){
					buf = buf + input[loc];
					loc++;
				}
				val[vLoc] = Float.parseFloat(buf);
				vLoc+=1;
				buf = "";
			}
		}
	}

    public int getSize(){
        return tasks.size();
    }

    public RTask getTask(int num){
        return (RTask)tasks.get(num);
    }

    public void setTask(int num, RTask task){
        tasks.set(num, task);
    }

    public void addTask(RTask task){
        tasks.add(task);
    }

    public void removeTask(int num){
        tasks.remove(num);
    }

    public String represent(){
        String rep = Integer.toString(tasks.size());
        rep = rep + " ";
        for(int i = 0; i < tasks.size(); i++){
            RTask x = (RTask)tasks.get(i);
            rep = rep + Integer.toString(x.getLength());
            rep = rep + ",";
            rep = rep + Integer.toString(x.getID());
            if(x.getParam(0) != -1){
            	rep = rep + ",";
                rep = rep + d.format(x.getParam(0));
            }
            if(x.getParam(1) != -1){
            	rep = rep + ",";
                rep = rep + d.format(x.getParam(1));
                rep = rep + ",";
                rep = rep + d.format(x.getParam(2));
            }
            rep = rep + "Q";
        }
        return rep;
    }
}