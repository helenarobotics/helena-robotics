package robotics.helena.widget;

import edu.wpi.first.smartdashboard.gui.StaticWidget;
import edu.wpi.first.smartdashboard.properties.*;
import edu.wpi.first.wpilibj.tables.ITable;
import edu.wpi.first.wpilibj.tables.ITableListener;
import edu.wpi.first.wpilibj.tables.ITable;
import java.awt.*;
import javax.swing.JComponent;

public class CrossAnalog extends StaticWidget implements ITableListener{
    public static final String NAME = "CrossAnalog MPWidget (EK)";

    private Double x = null;
    private Double y = null;

    public final ColorProperty xColor = new ColorProperty(this, "X Color", Color.blue);
    public final ColorProperty yColor = new ColorProperty(this, "Y Color", Color.green);
    public final ColorProperty textColor = new ColorProperty(this, "Text Color", Color.black);
    public final IntegerProperty textSize = new IntegerProperty(this, "Text Size", 15);
    public final BooleanProperty drawText = new BooleanProperty(this, "Draw Text", true);
    public final StringProperty xVar = new StringProperty(this, "X Variable:", "Joystick 2 X");
    public final StringProperty yVar = new StringProperty(this, "Y Variable:", "Joystick 2 Y");

    protected void paintComponent(Graphics g){
        if(x != null && y != null){
            //X line
            g.setColor(xColor.getValue());
            g.fillRect((int)((x+1)/2 * (getSize().getWidth()-1)),0,1,(int)(getSize().getHeight()));

            //Y line
            g.setColor(yColor.getValue());
            g.fillRect(0,(int)((1-((y+1)/2)) * (getSize().getHeight()-1)),(int)(getSize().getWidth()),1);

            if(drawText.getValue()){
                //Percents
                g.setColor(textColor.getValue());
                g.setFont(new Font("Dialog", Font.PLAIN, textSize.getValue()));
                g.drawString("("+x*100+", "+y*100+")",(int)((getSize().getWidth()-1)/2),(int)((getSize().getHeight()-1)/2));
            }
        }
    }

    public void init() {
        setPreferredSize(new Dimension(100,100));
        TableVars.table.addTableListener(this);
    }

    public void propertyChanged(Property property) {
        //Nothing important needs to happen here
    }

    @Override
    public void valueChanged(ITable itable, String key, Object o, boolean bln) {
        if(key.equals(xVar.getValue())){
            x = (double)o;
            repaint();
        }else if(key.equals(yVar.getValue())){
            y = (double)o;
            repaint();
        }
    }
}
