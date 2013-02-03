package robotics.helena.widget;


import edu.wpi.first.smartdashboard.gui.StaticWidget;
import edu.wpi.first.smartdashboard.properties.BooleanProperty;
import edu.wpi.first.smartdashboard.properties.ColorProperty;
import edu.wpi.first.smartdashboard.properties.IntegerProperty;
import edu.wpi.first.smartdashboard.properties.Property;
import edu.wpi.first.wpilibj.tables.ITable;
import edu.wpi.first.wpilibj.tables.ITableListener;

import javax.swing.*;
import java.awt.*;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

public class TimerPWidget extends StaticWidget implements ITableListener, ActionListener {
    public static final String NAME = "MatchTimer PWidget (EK)";

    public final IntegerProperty startTimeAmt = new IntegerProperty(this, "Start Time", 135);
    public final IntegerProperty textSize = new IntegerProperty(this, "Text Size", 20);
    public final ColorProperty stopColor = new ColorProperty(this, "Not Running Color", Color.red);
    public final ColorProperty runColor = new ColorProperty(this, "Running Color", Color.green);
    public final ColorProperty finColor = new ColorProperty(this, "Finished Color", Color.blue);
    public final BooleanProperty controlButtons = new BooleanProperty(this, "Control Buttons", false);

    private int timeRemaining = startTimeAmt.getValue();
    private boolean running = false;
    private Timer clock;
    private JLabel timeLa = new JLabel(Integer.toString(timeRemaining));
    private JButton stopBu = new JButton("Stop/Start");
    private JButton resetBu = new JButton("Reset");

    public void init() {
        setLayout(new BorderLayout(2,2));
        timeLa.setFont(new Font("Dialog",Font.BOLD, textSize.getValue()));
        if(controlButtons.getValue()){
            add(timeLa, BorderLayout.CENTER);
            add(stopBu, BorderLayout.SOUTH);
            add(resetBu, BorderLayout.NORTH);
        }else{
            add(timeLa, BorderLayout.CENTER);
        }
        stopBu.addActionListener(this);
        resetBu.addActionListener(this);
        clock = new Timer(1000, new ActionListener() {
            public void actionPerformed(ActionEvent e) {
                if(running){
                    timeRemaining--;
                    timeLa.setText(Integer.toString(timeRemaining));
                    if(timeRemaining == 0){
                        clock.stop();
                        running = false;
                        changeColor();
                    }
                }
            }
        });
        TableVars.table.addTableListener(this);
    }

    public boolean validatePropertyChange(Property property, Object value){
        if(property == startTimeAmt || property == textSize){
            int set = ((Integer)value).intValue();
            if(set <= 0)
                return false;
        }
        return true;
    }

    public void actionPerformed(ActionEvent e){
        if(e.getSource() == stopBu){
            if(running){
                running = false;
                clock.stop();
            }else if(!running && timeRemaining > 0){
                running = true;
                clock.start();
            }
        }else{
            running = false; //Reset to false, in case it is reset while running
            clock.stop();
            timeRemaining = startTimeAmt.getValue();
            timeLa.setText(Integer.toString(startTimeAmt.getValue()));
        }
        changeColor();
    }

    public void propertyChanged(Property property) {
        if(property == textSize){
            timeLa.setFont(new Font("Dialog",Font.BOLD, textSize.getValue()));
        }else if(property instanceof ColorProperty){
            changeColor();
        }else if(property == controlButtons){
            if(controlButtons.getValue()){
                add(timeLa, BorderLayout.CENTER);
                add(stopBu, BorderLayout.SOUTH);
                add(resetBu, BorderLayout.NORTH);
            }else{
                add(timeLa, BorderLayout.CENTER);
            }
        }
    }

    @Override
    public void valueChanged(ITable itable, String key, Object o, boolean bln) {
        if(key.equals(TableVars.timer)){
            if(o == 0 && timeRemaining > 0 && !clock.isRunning()){ //Start Timer
                running = true;
                clock.start();
            }if(o == 1 && clock.isRunning()){ //Stop Timer
                running = false;
                clock.stop();
            }if(o == 2 && timeRemaining != startTimeAmt.getValue()) {  //Reset Timer
                running = false; //Reset to false, in case it is reset while running
                clock.stop();
                timeRemaining = startTimeAmt.getValue();
                timeLa.setText(Integer.toString(startTimeAmt.getValue()));
            }
            changeColor();
        }
    }

    public void changeColor(){
        if(running)
            timeLa.setForeground(runColor.getValue());
        else if(timeRemaining == 0)
            timeLa.setForeground(finColor.getValue());
        else
            timeLa.setForeground(stopColor.getValue());
    }
}
