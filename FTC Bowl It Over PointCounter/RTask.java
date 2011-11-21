public class RTask{
    protected int length;
    protected int id;
    protected float[] params = new float[3];

    public RTask(){
        for(int i = 0; i < params.length; i++)
            params[i] = -1;
    }

    public RTask(float[] val){
		if(val[0] != -1)
			length = (int)val[0];
		if(val[1] != -1)
			id = (int)val[1];
		if(val[2] != -1)
			params[0] = val[2];
		if(val[3] != -1)
			params[1] = val[3];
		if(val[4] != -1)
			params[2] = (int)val[4];
	}

    public void setParam(int num, float val){
        params[num] = val;
    }

    public float getParam(int num){
        return params[num];
    }

    public void setLength(int length){
        this.length = length;
    }

    public int getLength(){
        return length;
    }

    public int getID(){
        return id;
    }

    public void setID(int id){
        this.id = id;
    }
}
