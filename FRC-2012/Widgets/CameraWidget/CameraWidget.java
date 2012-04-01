import edu.wpi.first.smartdashboard.gui.DashboardFrame;
import edu.wpi.first.smartdashboard.gui.DashboardPrefs;
import edu.wpi.first.smartdashboard.gui.StaticWidget;
import edu.wpi.first.smartdashboard.properties.BooleanProperty;
import edu.wpi.first.smartdashboard.properties.DoubleProperty;
import edu.wpi.first.smartdashboard.properties.IPAddressProperty;
import edu.wpi.first.smartdashboard.properties.Property;
import net.sf.jipcam.axis.CameraAPI;

import javax.swing.*;
import java.awt.*;
import java.awt.geom.Line2D;
import java.awt.image.BufferedImage;
import java.net.URL;

public class CameraWidget extends StaticWidget{
    public static final String NAME = "Camera w/ Overlay (EK)";
    public final IPAddressProperty ipProperty = new IPAddressProperty(this, "Camera IP Address", new int[] { 10, DashboardPrefs.getInstance().team.getValue().intValue() / 100, DashboardPrefs.getInstance().team.getValue().intValue() % 100, 11 });
    public final BooleanProperty overlayProperty = new BooleanProperty(this, "Overlay", true);
    public final DoubleProperty aspectProperty = new DoubleProperty(this, "Aspect Ratio", 1.5);
    public final BooleanProperty autoSetAspectProperty = new BooleanProperty(this, "Auto-set Aspect Ratio", false);

    private boolean overlay;
    private boolean aspectAutoBeenSet = false;
    private boolean autoSetAspect;
    private double aspectRatio;

    private CameraAPI cam = null;
    private URL camURL;

    private BufferedImage im = null;
    private ImageResults results = null;
    private ImageQueue iq;
	private DataQueue dq;
	private ImageUnderstanding imUn;
	private ImageHandler imH;
	private Thread iu;
	private Thread ih;
    int counter = 0, counter2 = 0;

    public void init() {
       setPreferredSize(new Dimension(200,200));
       try{
           camURL = new URL("http://" + ipProperty.getSaveValue() + "/mjpg/video.mjpg");
           cam = new CameraAPI(camURL);
       } catch(Exception e){ }
       overlay = overlayProperty.getValue();
       aspectRatio = aspectProperty.getValue();
       autoSetAspect = autoSetAspectProperty.getValue();

       iq = new ImageQueue();
       dq = new DataQueue();

       imH = new ImageHandler();
       imUn = new ImageUnderstanding(iq, dq, 1);

       ih = new Thread(imH, "Image Handler");
       iu = new Thread(imUn, "Image Understanding");

       ih.start();
       iu.start();
    }

    public void propertyChanged(Property property) {
        if(property == ipProperty){
            ih.stop();
            imUn.stop();
            try{
                camURL = new URL("http://" + ipProperty.getSaveValue() + "/mjpg/video.mjpg");
                cam = new CameraAPI(camURL);
            } catch(Exception e){ }
            ih.start();
            iu.start();
        }else if(property == overlayProperty){
            overlay = overlayProperty.getValue();
        }else if(property == aspectProperty){
            aspectRatio = aspectProperty.getValue();
            aspectAutoBeenSet = true;
        }else if(property == autoSetAspectProperty){
            autoSetAspect = autoSetAspectProperty.getValue();
            if(autoSetAspect){
                aspectAutoBeenSet = false;
            }
        }
    }

    class ImageHandler implements Runnable{
        private ImageResults res = null;
        private boolean running = true;

        public void run(){
                while(running){
                             try{
                                 im = (BufferedImage)cam.getImage();
                             } catch(Exception e){ im = null; JOptionPane.showMessageDialog(null, "Exception!");}

                            iq.put(im);
                            counter2++;
                            res = dq.get();
                            if(res != null){
                               results = res;
                               counter++;
                             }

                            DashboardFrame.getInstance().getPanel().repaint(getBounds());
                }
        }

        public void stop(){
                running = false;
        }
    }

    protected void paintComponent(Graphics g){
        if(im != null){
            if(autoSetAspect && !aspectAutoBeenSet) {
                aspectProperty.setValue((double)im.getWidth()/(double)im.getHeight());
                aspectAutoBeenSet = true;
            }
            g.setColor(Color.green);
            g.drawString(Integer.toString(im.getWidth()) + " : " + Integer.toString(im.getHeight()),5,70);
            if(getSize().getWidth()/getSize().getHeight() > aspectRatio){ //Width is too large
                int width = (int)(getSize().getHeight() * aspectRatio);
                int height = (int)getSize().getHeight();
                g.drawImage(im,((int) getSize().getWidth() - width) / 2, 0, ((int) getSize().getWidth() + width) / 2, height,0,0,im.getWidth(),im.getHeight(),null);
            }else if(getSize().getWidth()/getSize().getHeight() < aspectRatio){ //Height is too large
                int width = (int)getSize().getWidth();
                int height = (int)(getSize().getWidth()/aspectRatio);
                g.drawImage(im, 0, ((int) getSize().getHeight() - height) / 2, width, ((int) getSize().getHeight() + height) / 2,0,0,im.getWidth(),im.getHeight(),null);
            }else{ //Just right
                g.drawImage(im, 0,0,(int)getSize().getWidth(),(int)getSize().getHeight(),0,0,im.getWidth(),im.getHeight(),null);
            }
            if(overlay && results != null){
                drawResults(g);
                g.setColor(Color.ORANGE);
                g.setFont(new Font("Dialog", Font.PLAIN, 12));
                g.drawString("Overlay Successful #" + Integer.toString(counter)+"/"+Integer.toString(counter2),5,10);
            }else{
                g.setColor(Color.GREEN);
                g.setFont(new Font("Dialog", Font.PLAIN, 12));
                g.drawString("No Overlay",5,10);
            }
        }else{
            g.setColor(Color.red);
            if(getSize().getWidth()/getSize().getHeight() > aspectRatio){ //Width is too large
                int width = (int)(getSize().getHeight() * aspectRatio);
                int height = (int)getSize().getHeight();
                g.fillRect(((int) getSize().getWidth() - width) / 2, 0, width, height);
            }else if(getSize().getWidth()/getSize().getHeight() < aspectRatio){ //Height is too large
                int width = (int)getSize().getWidth();
                int height = (int)(getSize().getWidth()/aspectRatio);
                g.fillRect(0, ((int) getSize().getHeight() - height) / 2, width, height);
            }else{ //Just right
                g.fillRect(0,0,(int)getSize().getWidth(),(int)getSize().getHeight());
            }
            g.setColor(Color.black);
            g.setFont(new Font("Dialog", Font.PLAIN, 12));
            g.drawString("No Camera Connection",5,10);
        }
    }

    public void drawResults(Graphics g){

        int width = (int)getSize().getWidth();
        int height = (int)getSize().getHeight();
        int basex = 0;
        int basey = 0;
        if(getSize().getWidth()/getSize().getHeight() > aspectRatio){
            width = (int)(height * aspectRatio);
            basex = (int)((getSize().getWidth() - width)/2);
        }else if(getSize().getWidth()/getSize().getHeight() < aspectRatio){
            height = (int)(width / aspectRatio);
            basey = (int)((getSize().getHeight() - height)/2);
        }
        double scale = (double)width/im.getWidth() * results.downsample;

        g.setColor(Color.red);
        for(int i = 0; i < results.regions.size(); i++){
            Polygon p = new Polygon();
            Rectangle r = results.regions.elementAt(i).getEnclosingRectangle();
            p.addPoint((int)(r.x * scale + basex),(int)(r.y * scale + basey));
            p.addPoint((int)((r.x + r.width)* scale + basex), (int)(r.y * scale + basey));
            p.addPoint((int)((r.x + r.width)* scale + basex), (int)((r.y + r.height) * scale + basey));
            p.addPoint((int)(r.x* scale + basex), (int)((r.y + r.height)* scale + basey));
            g.drawPolygon(p);
            g.drawString(""+ (int)results.regions.elementAt(0).estimates.elementAt(0).range, (int)(r.x * scale + basex), (int)((r.y + r.width/2)*scale + basey));
            g.setColor(Color.blue);
            g.drawString("Scale: " + Double.toString(scale) + " Base: (" + Integer.toString(basex)+", " + Integer.toString(basey)+")",5,30);
        }

        //Draw Hoop Detection
        for(int i = 0; i < results.regions.size(); i++){
            Line2D.Double left = results.regions.get(i).leftEdge;
            Line2D.Double right = results.regions.get(i).rightEdge;
            Line2D.Double top = results.regions.get(i).topEdge;
            Line2D.Double bottom = results.regions.get(i).bottomEdge;
            g.drawLine((int)(left.x1*results.downsample * scale + basex),
                       (int)(left.y1*results.downsample * scale + basey),
                       (int)(left.x2*results.downsample * scale + basex),
                       (int)(left.y2*results.downsample * scale + basey));
            g.drawLine((int)(right.x1*results.downsample * scale + basex),
                       (int)(right.y1*results.downsample * scale + basey),
                       (int)(right.x2*results.downsample * scale + basex),
                       (int)(right.y2*results.downsample * scale + basey));
            g.drawLine((int)(top.x1*results.downsample * scale + basex),
                       (int)(top.y1*results.downsample * scale + basey),
                       (int)(top.x2*results.downsample * scale + basex),
                       (int)(top.y2*results.downsample * scale + basey));
            g.drawLine((int)(bottom.x1*results.downsample * scale + basex),
                       (int)(bottom.y1*results.downsample * scale + basey),
                       (int)(bottom.x2*results.downsample * scale + basex),
                       (int)(bottom.y2*results.downsample * scale + basey));
        }

    }
}
