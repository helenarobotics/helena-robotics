package view;

import java.awt.Color;
import java.awt.Dimension;
import java.awt.Graphics;

import java.util.Observable;
import java.util.Observer;

import javax.swing.JPanel;

import model.Robot;

public class FieldPanel extends JPanel {
    static final long serialVersionUID = -5499028701932093871L;

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

    // To make it fit on the screen, we scale the field.
    private static final double scale = 7.0 / 12.0;

    // Internal objects to draw on the field
    private Alley alleys[];
    private Bumper bumpers[];
    private Bridge bridges[];
    private Key keys[];

    public FieldPanel(Robot _robot) {
        // Turn on double-buffering
        super(true);

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

        // Finally, the end bumper/hoops
        bumpers = new Bumper[2];
        bumpers[0] = new Bumper(width, height, true);
        bumpers[1] = new Bumper(width, height, false);

        // Keep track of the objects we're controlling
        robot = _robot;
    }

    public void paint(Graphics g) {
        // Parent constructor first...
        super.paint(g);

        try {
            // Make the center of the screen 0, 0
            g.translate(getWidth() / 2, getHeight() / 2);

            // Draw a mid-line marker of three pixels wide.
            int edge = getWidth() / 2;
            g.setColor(Color.WHITE);
            g.fillRect(-edge, 1, getWidth(), 3);

            // Draw the bridges
            for (Bridge b: bridges)
                b.draw(g);

            // Draw the shooting keys
            for (Key k: keys)
                k.draw(g);

            // The alleys across the bridges
            for (Alley a: alleys)
                a.draw(g);

            // Finally, the bumper/hoops
            for (Bumper b: bumpers)
                b.draw(g);
        } finally {
            // Back to standard locations.
            g.translate(-getWidth() / 2, -getHeight() / 2);
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
                recY = 0;
            } else {
                alleyColor = Color.RED;
                recX = -fieldWidth / 2;
                recY = -fieldHeight / 2;
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
                recY = -fieldHeight / 2;
                crcY = recY;
            } else {
                color = Color.RED;
                recY = fieldHeight / 2 - recH;
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

            y = -h / 2;
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
            recY = -fieldHeight / 2;

            ovlX = recX;
            ovlW = recW;

            if (isBlue) {
                fieldColor = Color.BLUE;
                ovlY = recY + recH - ovlH / 2;
                ovlArc = -180;
            } else {
                fieldColor = Color.RED;
                recY = -(recY + recH);
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
