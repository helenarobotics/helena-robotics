package robotics.helena.widget.camera;

import edu.wpi.first.smartdashboard.gui.DashboardFrame;
import edu.wpi.first.smartdashboard.gui.DashboardPrefs;
import edu.wpi.first.smartdashboard.gui.StaticWidget;

import edu.wpi.first.smartdashboard.properties.BooleanProperty;
import edu.wpi.first.smartdashboard.properties.DoubleProperty;
import edu.wpi.first.smartdashboard.properties.IPAddressProperty;
import edu.wpi.first.smartdashboard.properties.Property;

import net.sf.jipcam.axis.CameraAPI;

import java.awt.Color;
import java.awt.Dimension;
import java.awt.Font;
import java.awt.Graphics;
import java.awt.Image;
import java.awt.Polygon;
import java.awt.Rectangle;
import java.awt.geom.Line2D;
import java.awt.image.BufferedImage;

import java.net.URL;

import java.util.concurrent.TimeUnit;
import java.util.concurrent.locks.Lock;
import java.util.concurrent.locks.ReentrantLock;

import javax.swing.JOptionPane;

import robotics.helena.widget.WidgetComm;

public class CameraWidget extends StaticWidget {
    // Keep the compiler happy
    private static final long serialVersionUID = 1L;

    public static final String NAME = "Camera w/ Overlay (EK)";

    private static final double linePos = (double)5/(double)8;

    // The camera should have an address related to the team number with
    // a final IP of '11'.
    public final IPAddressProperty ipProperty =
        new IPAddressProperty(
            this, "Camera IP Address", new int[] {
                10,
                DashboardPrefs.getInstance().team.getValue().intValue() / 100,
                DashboardPrefs.getInstance().team.getValue().intValue() % 100,
                11 });
    public final BooleanProperty overlayProperty =
        new BooleanProperty(this, "Overlay", true);
    public final DoubleProperty aspectProperty =
        new DoubleProperty(this, "Aspect Ratio", 1.5);
    public final BooleanProperty autoSetAspectProperty =
        new BooleanProperty(this, "Auto-set Aspect Ratio", false);

    // Data contained in the 'Property' above.
    private boolean showOverlay;
    private boolean aspectAutoBeenSet = false;
    private boolean autoSetAspect;
    private double aspectRatio;

    // The code that fetches the images and hands them off the the image
    // processing code.
    private CameraAPI cam = null;
    private BufferedImage cameraImage = null;
    private ImageResults results = null;

    // The structures used to communicate the data between the camera
    // fetching and image processing threads.
    private ImageQueue iq;
    private DataQueue dq;

    // The code to process the images
    private ImageUnderstanding imUn;
    private ImageHandler imH;

    private Thread iu;
    private Thread ih;
    private Thread gc;

    // Keep track of how many images have been fetched/processed.
    private int imageCounter = 0, processedImageCounter = 0;

    public void init() {
        setPreferredSize(new Dimension(200, 200));
        try {
            URL camURL = new URL(
                "http://" + ipProperty.getSaveValue() + "/mjpg/video.mjpg");
            cam = new CameraAPI(camURL);
        } catch (Exception ignored) {
        }
        showOverlay = overlayProperty.getValue();
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
        // IP address changed
        if (property == ipProperty) {
            // Stop the two threads
            ih.stop();
            iu.stop();

            // Reconnect to the camera
            cam = null;
            try {
                URL camURL = new URL(
                    "http://" + ipProperty.getSaveValue() + "/mjpg/video.mjpg");
                cam = new CameraAPI(camURL);
            } catch (Exception ignored) {
            }
            // Restart up the image processing threads.
            ih = new Thread(imH, "Image Handler");
            iu = new Thread(imUn, "Image Understanding");
            ih.start();
            iu.start();
        } else if (property == overlayProperty) {
            showOverlay = overlayProperty.getValue();
        } else if (property == aspectProperty) {
            aspectRatio = aspectProperty.getValue();
            aspectAutoBeenSet = true;
        } else if (property == autoSetAspectProperty) {
            autoSetAspect = autoSetAspectProperty.getValue();
            if (autoSetAspect)
                aspectAutoBeenSet = false;
        }
    }

    // Makes sure that we don't change the image out from under the
    // paint method.  We use a Lock instead of synchronized methods so
    // we can continue to work if we can't get the lock in a
    // 'reasonable' period of time.
    private Lock paintLock = new ReentrantLock();

    private class ImageHandler implements Runnable {
        private volatile boolean running = true;

        public void run() {
            BufferedImage image = null;
            while (running) {
                try {
                    image = (BufferedImage)cam.getImage();
                    imageCounter++;
                    iq.put(image);
                } catch (Exception e) {
                    JOptionPane.showMessageDialog(
                        null, "Exception:" + e.getMessage());
                }

                // We consider it a success if we process an image, even
                // if we don't get to use it in the repaint method.
                ImageResults res = dq.get();
                if (res != null){
                    processedImageCounter++;
                }

                // Safely update the image and results for the paint
                // method to use.
                boolean gotLock = false;
                try {
                    gotLock = paintLock.tryLock();
                    if (gotLock) {
                        if (image != null)
                            cameraImage = image;
                        if (res != null){
                            results = res;
                            sendResults();
                        }
                    }
                } finally {
                    if (gotLock)
                        paintLock.unlock();
                }
                DashboardFrame.getInstance().getPanel().repaint(getBounds());
            }
        }
        
        public void sendResults(){
        	WidgetComm.table.putDouble("Target RPM 1 BB", distanceToBBRPM(results.getHoop().range));
        	WidgetComm.table.putDouble("Target RPM 1 Swish", distanceToSwishRPM(results.getHoop().range));
        }
        
        public double distanceToBBRPM(double dist){
            return 0.00008 * Math.pow(dist,4) - 0.0667*Math.pow(dist, 3) + 19.844 * Math.pow(dist, 2) - 2599.7 * dist + 127490;
        }

        public double distanceToSwishRPM(double dist){
            return -0.036* Math.pow(dist, 3) + 23.73*Math.pow(dist,2) - 5194.3 * dist + 378388;
        }

        public void stop() {
            running = false;
        }
    }

    private class GarbageCollectorThread extends Thread {
        private volatile boolean running = true;
        public void run() {
            while (running) {
                try {
                    Thread.sleep(10 * 1000);
                } catch (Exception ignored) {
                }
                System.gc();
            }
        }

        public void end() {
            running = false;
            interrupt();
        }
    }

    protected void paintComponent(Graphics g) {
        boolean gotLock = false;
        try {
            // If we can't get the lock in 100ms, give up and let the
            // AWT thread continue.  We cleanup in the finally clause.
            try {
                gotLock = paintLock.tryLock(100L, TimeUnit.MILLISECONDS);
            } catch (InterruptedException ignored) {
                gotLock = false;
            }
            if (!gotLock)
                return;

            // Double buffer by drawing the image onto a background
            // image which we then redraw after it's all done drawing.
            Image buffer = createImage(getSize().width, getSize().height);
            paintStuff(buffer.getGraphics());
            g.drawImage(buffer, 0, 0, null);
        } finally {
            if (gotLock)
                paintLock.unlock();
        }
	}

	private void paintStuff(Graphics g) {
        double currAspectRatio = getSize().getWidth() / getSize().getHeight();

        if (cameraImage != null) {
            if (autoSetAspect && !aspectAutoBeenSet) {
                aspectAutoBeenSet = true;
                aspectProperty.setValue(
                    (double)cameraImage.getWidth() / (double)cameraImage.getHeight());
            }

            g.setColor(Color.GREEN);
            g.drawString(Integer.toString(cameraImage.getWidth()) + " : " +
                         Integer.toString(cameraImage.getHeight()), 5, 70);

            // Width is too large
            if (currAspectRatio > aspectRatio) {
                int width = (int)(getSize().getHeight() * aspectRatio);
                int height = (int)getSize().getHeight();
                g.drawImage(cameraImage,
                            ((int)getSize().getWidth() - width) / 2, 0,
                            ((int)getSize().getWidth() + width) / 2, height,
                            0, 0,
                            cameraImage.getWidth(), cameraImage.getHeight(), null);
            } else if (currAspectRatio < aspectRatio) {
                // Height is too large
                int width = (int)getSize().getWidth();
                int height = (int)(getSize().getWidth() / aspectRatio);
                g.drawImage(cameraImage,
                            0, ((int)getSize().getHeight() - height) / 2,
                            width, ((int)getSize().getHeight() + height) / 2,
                            0, 0,
                            cameraImage.getWidth(), cameraImage.getHeight(), null);
            } else {
                // Just right
                g.drawImage(cameraImage,
                            0, 0,
                            (int)getSize().getWidth(), (int)getSize().getHeight(),
                            0, 0,
                            cameraImage.getWidth(), cameraImage.getHeight(), null);
            }

            // Draw the overlay on top of the image!
            if (showOverlay && results != null) {
                drawResults(g);
                g.setColor(Color.ORANGE);
                g.setFont(new Font("Dialog", Font.PLAIN, 12));
                g.drawString("Overlay Successful #" +
                             Integer.toString(processedImageCounter) + "/" +
                             Integer.toString(imageCounter), 5, 10);
            } else {
                g.setColor(Color.GREEN);
                g.setFont(new Font("Dialog", Font.PLAIN, 12));
                g.drawString("No Overlay", 5, 10);
            }
        } else {
            // No image found!
            g.setColor(Color.RED);
            if (currAspectRatio > aspectRatio) {
                // Width is too large
                int width = (int)(getSize().getHeight() * aspectRatio);
                int height = (int)getSize().getHeight();
                g.fillRect(((int)getSize().getWidth() - width) / 2, 0, width, height);
            } else if (currAspectRatio < aspectRatio) {
                // Height is too large
                int width = (int)getSize().getWidth();
                int height = (int)(getSize().getWidth() / aspectRatio);
                g.fillRect(0, ((int)getSize().getHeight() - height) / 2, width, height);
            } else {
                // Just right
                g.fillRect(0, 0, (int)getSize().getWidth(), (int)getSize().getHeight());
            }
            g.setColor(Color.BLACK);
            g.setFont(new Font("Dialog", Font.PLAIN, 12));
            g.drawString("No Camera Connection", 5, 10);
        }
        double width = getSize().getWidth();
        double basex = 0;
        if(currAspectRatio > aspectRatio){
            width = getSize().getHeight() * aspectRatio;
            basex = (getSize().getWidth()-width)/2;
        }

        g.setColor(Color.orange);
        g.drawLine((int)(linePos*width+basex),0,(int)(linePos*width+basex),(int)getSize().getHeight());
    }

    private void drawResults(Graphics g) {
        int width = (int)getSize().getWidth();
        int height = (int)getSize().getHeight();
        int baseX = 0;
        int baseY = 0;

        double currAspectRatio = getSize().getWidth() / getSize().getHeight();
        if (currAspectRatio > aspectRatio) {
            width = (int)(height * aspectRatio);
            baseX = (int)((getSize().getWidth() - width) / 2);
        } else if (currAspectRatio < aspectRatio) {
            height = (int)(width / aspectRatio);
            baseY = (int)((getSize().getHeight() - height) / 2);
        }
        double scale = (double)width / cameraImage.getWidth();

        g.setColor(Color.RED);
        for (int i = 0; i < results.regions.size(); i++) {
            Polygon p = new Polygon();
            Rectangle rec = results.regions.get(i).getEnclosingRectangle();
            p.addPoint((int)(rec.x * scale + baseX), (int)(rec.y * scale + baseY));
            p.addPoint((int)((rec.x + rec.width) * scale + baseX), (int)(rec.y * scale + baseY));
            p.addPoint((int)((rec.x + rec.width) * scale + baseX), (int)((rec.y + rec.height) * scale + baseY));
            p.addPoint((int)(rec.x * scale + baseX), (int)((rec.y + rec.height) * scale + baseY));
            g.drawPolygon(p);
            g.drawString("" + (int)results.regions.get(0).estimates.get(0).range,
                         (int)(rec.x * scale + baseX), (int)((rec.y + rec.width / 2) * scale + baseY));
            g.setColor(Color.BLUE);
            g.drawString("Scale: " + Double.toString(scale) +
                         " Base: (" + Integer.toString(baseX) + ", " +
                         Integer.toString(baseY) + ")", 5, 30);
        }

        // Draw Hoop Detection
        for (int i = 0; i < results.regions.size(); i++) {
            Line2D.Double left = results.regions.get(i).leftEdge;
            Line2D.Double right = results.regions.get(i).rightEdge;
            Line2D.Double top = results.regions.get(i).topEdge;
            Line2D.Double bottom = results.regions.get(i).bottomEdge;
            g.drawLine((int)(left.x1 * scale + baseX),
                       (int)(left.y1 * scale + baseY),
                       (int)(left.x2 * scale + baseX),
                       (int)(left.y2 * scale + baseY));
            g.drawLine((int)(right.x1 * scale + baseX),
                       (int)(right.y1 * scale + baseY),
                       (int)(right.x2 * scale + baseX),
                       (int)(right.y2 * scale + baseY));
            g.drawLine((int)(top.x1 * scale + baseX),
                       (int)(top.y1 * scale + baseY),
                       (int)(top.x2 * scale + baseX),
                       (int)(top.y2 * scale + baseY));
            g.drawLine((int)(bottom.x1 * scale + baseX),
                       (int)(bottom.y1 * scale + baseY),
                       (int)(bottom.x2 * scale + baseX),
                       (int)(bottom.y2 * scale + baseY));
        }
    }
}
