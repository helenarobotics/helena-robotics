import java.awt.Color;
import java.awt.Dimension;
import java.awt.Graphics;

import javax.swing.JPanel;

class CameraPanel extends JPanel {
    static final long serialVersionUID = -1869034191007993406L;

    // Camera location and scale
    private int azimuthAngle;
    private int inclinationAngle;
    private double dist;

    // The hoop objects.
    private Hoop hoops[];
    
    CameraPanel() {
        // Turn on double-buffering
        super(true);
//        setPreferredSize(new Dimension(1024, 768));
        setPreferredSize(new Dimension(800, 600));
        setBackground(Color.BLACK);
        setForeground(Color.WHITE);

        // We'll be drawing images onto this, so no layout!
        setLayout(null);

        // Create the hoops
        hoops = new Hoop[4];

        // Bottom hoop
        hoops[0] = new Hoop(0, 35.0);

        // Middle two hoops
        hoops[1] = new Hoop(-27.375, 1.5);
        hoops[2] = new Hoop(+27.375, 1.5);

        // Top hoop
        hoops[3] = new Hoop(0, -35.0);
    }

    void setCameraDistance(double _dist) {
        if (dist != _dist) {
            dist = _dist;
            repaint();
        }
    }

    void setCameraAzimuth(int azimuth) {
        if (azimuthAngle != azimuth) {
            azimuthAngle = azimuth;
            repaint();
        }
    }

    void setCameraInclination(int inclination) {
        if (inclinationAngle != inclination) {
            inclinationAngle = inclination;
            repaint();
        }
    }

    volatile boolean translated = false;

    public void paint(Graphics g) {
        // Parent constructor first...
        super.paint(g);

        // Find the center of the panel
        int xPos = getWidth() / 2;
        int yPos = getHeight() / 2;

        // XXX - Calculate how to display based on scale, azimuth, and
        // inclination.
        // @ 10' (120"), scale is ~8, and at 5' (60"), it should be 16.
        double scale = 960 / dist;
        xPos -= (int)(5 * azimuthAngle / scale);
        yPos += (int)(5 * inclinationAngle / scale);

        // Draw the hoops
        for (Hoop h: hoops)
            h.paint(g, xPos, yPos, scale);
    }
}
