package robotics.helena.widget.camera;

import com.sun.org.apache.bcel.internal.generic.RET;
import edu.wpi.first.wpilibj.networking.NetworkTable;

import javax.imageio.ImageIO;
import java.awt.image.BufferedImage;
import java.io.File;

public class ImageResultsTable{
    public static NetworkTable toNetworkTable(ImageResults make){
        byte[] bytes = null;
        try{
            bytes = ImageResults.serializeResults(make);
        }catch(Exception e){System.out.println("Error in ImageResultsTable: " + e.getMessage());}
        return bytesToTable(bytes);
    }

    private static NetworkTable bytesToTable(byte[] to){
        NetworkTable.setTeam(1695);
        NetworkTable ret = NetworkTable.getTable("ImageResults");
        System.out.println(to == null);
        int numInts = to.length/4;
        int rem = to.length%4;
        if(rem != 0)
            numInts++;
        ret.putInt("Number of Integers",numInts);
        ret.putInt("Number of Elements for Last Item", rem);
        ret.putInt("473",1);
        for(int i = 0;i < to.length; i+=4){
            if(i+3 < to.length){
                ret.putInt(Integer.toString(i/4),byteGroupToInt(new byte[]{to[i],to[i+1],to[i+2],to[i+3]}));
            }else if(i+2 < to.length){
                ret.putInt(Integer.toString(i/4),byteGroupToInt(new byte[]{to[i],to[i+1],to[i+2]}));
            }else if(i+1 < to.length){
                ret.putInt(Integer.toString(i/4),byteGroupToInt(new byte[]{to[i],to[i+1]}));
            }else{
                ret.putInt(Integer.toString(i/4),byteGroupToInt(new byte[]{to[i]}));
            }
        }
        System.out.println(to.length+": length");
        return ret;
    }

    private static int byteGroupToInt(byte[] convert){
        int result = 0;
        for(int i = convert.length-1; i >= 0; i--){
            result <<= 8;
            result += (convert[i] & 0xFF);
        }
        return result;
    }

    public static ImageResults toImageResults(NetworkTable table){
        int length = table.getInt("Number of Integers");
        System.out.println(length+": read length");
        int lastByteNum = table.getInt("Number of Elements for Last Item");
        if(lastByteNum == 0)
            lastByteNum = 4;
        byte[] serializedRes = new byte[4*(length-1)+lastByteNum];

        for(int i = 1; i < length; i++){
            byte[] add;
            if(i+1 == length && lastByteNum != 4){
                add = intToByteArray(table.getInt(Integer.toString(i)), lastByteNum);
                for(int j = 0; j < add.length; j++)
                    System.out.print(add[j] +" ");
                System.out.println("");
            }else{
                add = intToByteArray(table.getInt(Integer.toString(i)));
                System.out.println(add[0] + " " + add[1] + " " + add[2] + " " + add[3]);
            }
            for(int j = 0; j < add.length; j++){
                serializedRes[4*i+j] = add[j];
            }
        }
        try{
            return ImageResults.deserializeResults(serializedRes);
        }catch(Exception e){
            System.out.println("Error in ImageResults Table: " + e.getMessage());
        }
        return null;
    }

    private static byte[] intToByteArray(int conv){
        byte[] ret = new byte[4];
        for(int i = 0; i < ret.length; i++)
            ret[i] = (byte)(conv >> (8*i) & 0xFF);
        return ret;
    }

    private static byte[] intToByteArray(int conv, int nElements){
        byte[] ret = new byte[nElements];
        for(int i = 0; i < ret.length; i++)
            ret[i] = (byte)(conv >> (8*(nElements-i-1)) & 0xFF);
        return ret;
    }

    public static void main(String[] args){
        BufferedImage im = null;
        try{
            im = ImageIO.read(new File("F:/image.jpeg"));
        }catch(Exception e){System.out.println("Error Reading: " + e.getMessage());}
        ImageResults res = new ImageResults(im, 1, 60, 200);
        System.out.println(toImageResults(toNetworkTable(res)) == res);
    }
}
