/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

package org.test;

/**
 *
 * @author jmiller
 */
public class Config {
    //Victor Channels
    public static final int FRONT_LEFT_VICTOR =  1;
    public static final int FRONT_RIGHT_VICTOR = 2;
    public static final int BACK_LEFT_VICTOR =   3;
    public static final int BACK_RIGHT_VICTOR =  4;
   
    //Solenoid(s) Channel
    public static final int LIFT_SOLENOID = 1;
    public static final int JAW_SOLENOID = 2;
    
    //DRIVE TYPES
    public static final int ARCADE =   1;
    public static final int MECHANUM = 2;
    
    //Drive Type Option
    public static final int DRIVE_TYPE = MECHANUM;
    
    //Joystick Channels
    public static final int DRIVE_JOYSTICK = 1;
    public static final int AUXILIARY_JOYSTICK = 2;
    
    //Drive Joystick Buttons

    //Auxilary Joystick Buttons
    public static final int LIFT_BUTTON = 1;
    public static final int JAW_BUTTON = 2;
}
