package view;

import java.awt.Color;
import java.awt.Dimension;
import java.awt.Graphics;
import java.awt.Graphics2D;
import java.awt.Point;
import java.awt.Polygon;

import java.awt.event.MouseEvent;
import java.awt.event.MouseWheelEvent;
import java.awt.event.MouseWheelListener;
import java.awt.event.MouseMotionListener;

import java.util.Observable;
import java.util.Observer;

import javax.swing.JPanel;

import model.Robot;

public class FieldPanel extends JPanel implements Observer {
    static final long serialVersionUID = -6058652728550317910L;

    // Robot information.
    private Robot robot;

    // Field markings!
    private static final double BRIDGE_WIDTH = 48;
    private static final double BRIDGE_LENGTH = 88;

    private static final double BUMPER_WIDTH = 101;
    private static final double BUMPER_LENGTH = 39;

    private static final double FIELD_LENGTH = 54 * 12;
    private static final double FIELD_WIDTH = 27 * 12;

    private static final double KEY_LENGTH = 144;
    private static final double KEY_WIDTH = 101;
    private static final double KEY_RADIUS = 48;

    // Robot Sprite size (arbitrarily chosen)
    private static final double ROBOT_LENGTH = 38;
    private static final double ROBOT_WIDTH = 27;

    // To make it fit on the screen, we scale the field.
    private static final double scale = 7.0 / 12.0;

    // Internal objects to draw on the field
    private Alley alleys[];
    private Bumper bumpers[];
    private Bridge bridges[];
    private Key keys[];
    private RobotSprite rs;

    public FieldPanel(Robot _robot) {
        // Turn on double-buffering
        super(true);

        // Keep track of the objects we're controlling
        robot = _robot;

        int width = (int)(FIELD_WIDTH * scale);
        int height = (int)(FIELD_LENGTH * scale);

        setPreferredSize(new Dimension(width, height));
        setForeground(Color.WHITE);
        setBackground(Color.BLACK);

        // We'll be drawing images onto this, so no layout!
        setLayout(null);

        // The two alleys
        alleys = new Alley[2];
        alleys[0] = new Alley(width, height, true);
        alleys[1] = new Alley(width, height, false);

        // Three bridges
        bridges = new Bridge[3];
        for (int i = 0; i < bridges.length; i++)
            bridges[i] = new Bridge(width, height, i);

        // The two keys
        keys = new Key[2];
        keys[0] = new Key(width, height, true);
        keys[1] = new Key(width, height, false);

        // The end bumper/hoops
        bumpers = new Bumper[2];
        bumpers[0] = new Bumper(width, height, true);
        bumpers[1] = new Bumper(width, height, false);

        // Finally, add the robot!
        rs = new RobotSprite(width, height, robot);

        // Watch for mouse-wheel events for robot rotation.
        addMouseWheelListener(new MouseWheelListener() {
                public void mouseWheelMoved(MouseWheelEvent e) {
                    boolean rotateLeft = true;
                    int notches = e.getWheelRotation();
                    if (notches < 0)
                        rotateLeft = false;
                    for (int i = Math.abs(e.getWheelRotation()); i > 0; i--) {
                        if (rotateLeft)
                            robot.rotateLeft();
                        else
                            robot.rotateRight();
                    }
                }
            });

        // Watch the state of the robot so we can update the position
        // labels.
        robot.addObserver(this);
    }

    // The Robot model was updated, so update the information.
    public void update(Observable o, Object arg) {
        repaint();
    }

    public void paint(Graphics g) {
        // Parent constructor first...
        super.paint(g);

        try {
            // Recenter the origin to the center of the top of the field
            // to match the robot's perspective.
            g.translate(getWidth() / 2, 0);

            // Draw a mid-line marker of three pixels wide.
            int edge = getWidth() / 2;
            int center = getHeight() / 2;
            g.setColor(Color.WHITE);
            g.fillRect(-edge, center - 1, getWidth(), 3);

            // Draw the bridges
            for (Bridge b: bridges)
                b.draw(g);

            // Draw the shooting keys
            for (Key k: keys)
                k.draw(g);

            // The alleys across the bridges
            for (Alley a: alleys)
                a.draw(g);

            // The bumper/hoops at each end
            for (Bumper b: bumpers)
                b.draw(g);

            // Finally, the robot!
            rs.draw(g);
        } finally {
            // Back to standard locations.
            g.translate(-getWidth() / 2, 0);
        }
    }

    private Point pixelToFieldLocation(Point pixel) {
        int width = getWidth();
        int height = getHeight();

        // The ratio of the pixels to the total with determines the
        // location, but the X axis is centered, so we subtract half of
        // the width to center it.
        int fieldX = (int)FIELD_WIDTH * pixel.x / width;
        fieldX -= (int)(FIELD_WIDTH / 2);

        // Simple ratio
        int fieldY = (int)FIELD_LENGTH * pixel.y / height;
        return new Point(fieldX, fieldY);
    }

    private class RobotSprite implements MouseMotionListener {
        Robot robot;
        int w, h;

        // Arbritrarily chosen
        Color color = Color.YELLOW;

        RobotSprite(int fieldWidth, int fieldHeight, Robot _robot) {
            robot = _robot;
            w = (int)(ROBOT_WIDTH * scale);
            h = (int)(ROBOT_LENGTH * scale);

            // Keep track of mouse events
            addMouseMotionListener(this);
        }

        public void mouseDragged(MouseEvent e) {
            Point field = pixelToFieldLocation(e.getPoint());
            robot.setXOffset((int)field.getX());
            robot.setZOffset((int)field.getY());
        }

        public void mouseMoved(MouseEvent ignored) { }

        // Pre-calculate the math for drawing a line at 27 degrees to
        // each side to indicate the 54 degree FOV for the camera.
        // (90 - 27 = 63).
        private final double FOV_MATH = 2.0 / Math.tan(Math.toRadians(63));

        void draw(Graphics g) {
            // Convert robot position to x/y in current co-ordinates.
            int x = (int)(robot.getXOffset() * scale);
            int y = (int)(robot.getZOffset() * scale);

            // Draw the robot!
            Graphics2D g2d = (Graphics2D)g;

            // Set the origin to the center of the robot and rotate the
            // robot's rotation.
            g2d.translate(x, y);
            g2d.rotate(Math.toRadians(robot.getRotation()));

            // Draw the robot.
            g2d.setColor(color);
            int recX = -w / 2;
            int recY = -h / 2;
            g2d.fillRect(recX, recY, w, h);

            // Provide a triangle indicating robot direction
            Polygon dirPoly = new Polygon();
            dirPoly.addPoint(recX, recY);
            dirPoly.addPoint(recX + w / 2, recY - h / 4);
            // XXX - We subtract otherwise it draws one-pixel outside
            // the rectangle
            dirPoly.addPoint(recX + w - 1, recY);
            g2d.drawPolygon(dirPoly);

            // Draw a line at 27 degrees to each side to indicate the
            // FOV for the camera.  (90 - 27 = 63).
            // Choose a really big number like 2y for deltaY.
            int dx = (int)(y * FOV_MATH);
            g2d.translate(recX + w / 2, recY);
            g2d.drawLine(0, 0, -dx, -y * 2);
            g2d.drawLine(0, 0, dx, -y * 2);
            g2d.translate(-(recX + w / 2), -recY);

            // Restore the previous graphics context
            g2d.rotate(Math.toRadians(-robot.getRotation()));
            g2d.translate(-x, -y);
        }
    }

    private class Alley {
        int recX, recY;
        int recW, recH;

        Color alleyColor;

        Alley(int fieldWidth, int fieldHeight, boolean isBlue) {
            recW = (int)(BRIDGE_WIDTH * scale);
            recH = fieldHeight / 2;

            if (isBlue) {
                alleyColor = Color.BLUE;
                recX = fieldWidth / 2 - recW;
                recY = fieldHeight / 2;
            } else {
                alleyColor = Color.RED;
                recX = -fieldWidth / 2;
                recY = 0;
            }
        }

        void draw(Graphics g) {
            g.setColor(alleyColor);
            g.drawRect(recX, recY, recW, recH);
        }
    }

    private class Bumper {
        Color color;
        int recW, recH;
        int recX, recY;
        int crcX, crcY;
        int crcR;

        Bumper(int fieldWidth, int fieldHeight, boolean isBlue) {
            recW = (int)(BUMPER_WIDTH * scale);
            recH = (int)(BUMPER_LENGTH * scale);
            crcR = recH;
            recX = -recW / 2;
            crcX = -crcR / 2;
            if (isBlue) {
                color = Color.BLUE;
                recY = 0;
                crcY = recY;
            } else {
                color = Color.RED;
                recY = fieldHeight - recH;
                crcY = recY;
            }
        }

        void draw(Graphics g) {
            g.setColor(color);
            g.fillRect(recX, recY, recW, recH);
            // Add a hoop in the center
            g.setColor(Color.BLACK);
            g.drawArc(crcX, crcY, crcR, crcR, 0, 360);
        }
    }

    private class Bridge {
        Color bridgeColor;
        int w, h;
        int x, y;

        Bridge(int fieldWidth, int fieldHeight, int location) {
            w = (int)(BRIDGE_WIDTH * scale);
            h = (int)(BRIDGE_LENGTH * scale);

            y = (fieldHeight - h) / 2;
            switch (location) {
            case 0:
                // Left (Red)
                bridgeColor = Color.RED;
                x = -fieldWidth / 2;
                break;
            case 1:
                // Middle (Neutral)
                bridgeColor = Color.WHITE;
                x = -w / 2;
                break;
            case 2:
                // Right (Blue)
                bridgeColor = Color.BLUE;
                x = fieldWidth / 2 - w;
                break;
            }
        }

        void draw(Graphics g) {
            g.setColor(bridgeColor);
            g.fillRect(x, y, w, h);
        }
    }

    private class Key {
        int recW, recH;
        int recX, recY;

        int ovlW, ovlH;
        int ovlX, ovlY;
        int ovlArc;

        Color fieldColor;

        Key(int fieldWidth, int fieldHeight, boolean isBlue) {
            recW = (int)(KEY_WIDTH * scale);
            recH = (int)(KEY_LENGTH * scale);
            ovlH = (int)(KEY_RADIUS * scale);

            // Note, we're doing a half-height circle, but the height is
            // normally the diameter, so multiply times 2.  Thanks Dan!
            ovlH *= 2;

            recX = -recW / 2;

            ovlX = recX;
            ovlW = recW;

            if (isBlue) {
                fieldColor = Color.BLUE;
                recY = 0;
                ovlY = recY + recH - ovlH / 2;
                ovlArc = -180;
            } else {
                fieldColor = Color.RED;
                recY = fieldHeight - recH;
                ovlY = recY - ovlH / 2;
                ovlArc = 180;
            }
        }

        void draw(Graphics g) {
            g.setColor(fieldColor);
            g.drawRect(recX, recY, recW, recH);
            g.fillArc(ovlX, ovlY, ovlW, ovlH, 0, ovlArc);
        }
    }
}
