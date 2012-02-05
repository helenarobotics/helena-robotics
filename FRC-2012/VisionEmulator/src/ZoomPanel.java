import java.awt.Component;
import java.awt.FlowLayout;

import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.MouseEvent;
import java.awt.event.MouseListener;

import javax.swing.JButton;
import javax.swing.JPanel;
import javax.swing.Timer;

// This class keeps track of the camera panning controls.
class ZoomPanel extends JPanel {
    static final long serialVersionUID = -5149961119681723198L;

    // The field width (not totally correct due to angle, but call it ok)
    private static final int MAX_DIST = 57 * 12;

    // Fender limit's how close we can get
    private static final int MIN_DIST = 39;

    // Default distance is Keeps track of how the camera is tilted on the robot
    private int distance;

    // The model we're controlling!
    private CameraPanel cp;

    ZoomPanel(CameraPanel _cp) {
        super(new FlowLayout());

        // Set the defaults!
        distance = 20 * 12;

        // Keep track of what we're controlling!
        cp = _cp;
        cp.setCameraDistance(distance);

        // Zoom-In button
        JButton zoomInButton = new JButton("Zoom In");
        final ActionListener zoomInAction = new ActionListener() {
                public void actionPerformed(ActionEvent e) {
                    if (distance > 0) {
                        // XXX - Arbitarily move by one foot at a time
                        distance -= 12;
                        // Fender limits us getting any closer than 39"
                        if (distance < MIN_DIST)
                            distance = MIN_DIST;
                        cp.setCameraDistance(distance);
                    }
                }
            };
        zoomInButton.addMouseListener(new MouseListener() {
                private Timer timer;
                public void mouseClicked(MouseEvent ignored) { }
                public void mouseEntered(MouseEvent ignored) { }
                public void mouseExited(MouseEvent ignored) { }
                public void mousePressed(MouseEvent e) {
                    timer = new Timer(100, zoomInAction);
                    timer.start();
                }
                public void mouseReleased(MouseEvent e) {
                    timer.stop();
                }
            });
        add(zoomInButton);

        // Zoom-out
        JButton zoomOutButton = new JButton("Zoom Out");
        final ActionListener zoomOutAction = new ActionListener() {
                public void actionPerformed(ActionEvent e) {
                    if (distance < MAX_DIST) {
                        // XXX - Arbitarily move by one foot at a time
                        distance += 12;
                        if (distance > MAX_DIST)
                            distance = MAX_DIST;
                        cp.setCameraDistance(distance);
                    }
                }
            };
        zoomOutButton.addMouseListener(new MouseListener() {
                private Timer timer;
                public void mouseClicked(MouseEvent ignored) { }
                public void mouseEntered(MouseEvent ignored) { }
                public void mouseExited(MouseEvent ignored) { }
                public void mousePressed(MouseEvent e) {
                    timer = new Timer(100, zoomOutAction);
                    timer.start();
                }
                public void mouseReleased(MouseEvent e) {
                    timer.stop();
                }
            });
        add(zoomOutButton);
    }
}
