import java.awt.Color;
import java.awt.Graphics;

import javax.swing.JPanel;

class Hoop {
    // What color is the tape?
    private static final Color TAPE_COLOR = Color.RED;

    // All values are in inches!
    private static final double backboardOutsideWidth = 44.0;
    private static final double backboardOutsideHeight = 31.5;

    // The tape box inside the backboard!
    private static final double tapeOutsideWidth = 24.0;
    private static final double tapeOutsideHeight = 18.0;

    private static final double tapeInsideWidth = 20.0;
    private static final double tapeInsideHeight = 14.0;

    // The offset of this hoop in inches from the 'center' of the
    // origin.
    private final double xOffset;
    private final double yOffset;

    Hoop(double _xOffset, double _yOffset) {
        xOffset = _xOffset;
        yOffset = _yOffset;
    }

    void paint(Graphics g, int xOrigin, int yOrigin, double scale) {
        Color origColor = null;
        try {
            // Grab the current color
            origColor = g.getColor();
            
            // Scale the offset by the scale amount
            xOrigin += (int)(xOffset * scale);
            yOrigin += (int)(yOffset * scale);

            // Draw the backboard!
            int width = (int)(backboardOutsideWidth * scale);
            int height = (int)(backboardOutsideHeight * scale);
            int startX = xOrigin - width / 2;
            int startY = yOrigin - height / 2;
            g.drawRect(startX, startY, width, height);

            // Draw the reflective tape with fillRect to give it width
            width = (int)(tapeOutsideWidth * scale);
            height = (int)(tapeOutsideHeight * scale);
            startX = xOrigin - width / 2;
            startY = yOrigin - height / 2;
            g.setColor(TAPE_COLOR);
            g.fillRect(startX, startY, width, height);

            // 'Unfill' to the inner amount
            width = (int)(tapeInsideWidth * scale);
            height = (int)(tapeInsideHeight * scale);
            startX = xOrigin - width / 2;
            startY = yOrigin - height / 2;
            g.setColor(Color.BLACK);
            g.fillRect(startX, startY, width, height);
        } finally {
            if (origColor != null)
                g.setColor(origColor);
        }
    }
}
