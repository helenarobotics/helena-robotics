import javax.swing.*;
import javax.swing.filechooser.*;
import javax.swing.filechooser.FileFilter;
import java.awt.*;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.KeyEvent;
import java.io.*;

public class PointCounter extends JFrame implements ActionListener{
	private final PointArea mainPA = new PointArea();
	private final RecArea mainRA = new RecArea();
    private final JMenu fileM = new JMenu("File");
    private final JMenu fileNewSM = new JMenu("New...");
	private final JMenu fileOpenSM = new JMenu("Open...");
	private final JMenuItem fileNewRoutMi = new JMenuItem("Routine File");
	private final JMenuItem fileNewRecMi = new JMenuItem("Record File");
	private final JMenuItem fileOpenRoutMi = new JMenuItem("Routine File");
	private final JMenuItem fileOpenRecMi = new JMenuItem("Record File");
	private final JMenuItem fileSaveMi = new JMenuItem("Save",new ImageIcon(getClass().getResource("images/filesave.png")));
	private final JMenuItem fileSaveAsMi = new JMenuItem("Save As", new ImageIcon(getClass().getResource("images/filesaveas.png")));
	private final JMenuItem fileExitMi = new JMenuItem("Exit",new ImageIcon(getClass().getResource("images/fileclose.png")));
	private int type;
	private String filename = "";
    private JLabel fileLa = new JLabel();
    private JFileChooser fc = new JFileChooser();

	public PointCounter(){
		type = 0;
		setTitle("FTC Bowl It Over Point System");
		initAL();
		setJMenuBar(mkMenu(1));
		setSize(500,405);
		setVisible(true);
	}

	public PointCounter(int type){
		if(type == 1){
			this.type = type;
			setTitle("FTC Bowl It Over Point Counter");
			setJMenuBar(mkMenu(1));
			setContentPane(this.mkPane(1));
			setSize(500,405);
		}
		if(type == 2){
			this.type = type;
			setTitle("FTC Bowl It Over Point Record");
			setJMenuBar(mkMenu(2));
			setContentPane(this.mkPane(2));
			setSize(500,405);
		}
		initAL();
		setVisible(true);
	}

	public PointCounter(int type, String data, String filename){
		if(type == 1){
			this.type = type;
			setTitle("FTC Bowl it Over PointCounter");
            fileLa.setText(filename);
			setJMenuBar(mkMenu(1));
			setContentPane(this.mkPane(1));
			setSize(500,405);
		}
		if(type == 2){
			this.type = type;
			setTitle("FTC Bowl It Over Point Record");
            fileLa.setText(filename);
			setJMenuBar(mkMenu(2));
			setContentPane(this.mkPane(2));
			setSize(500,405);
		}
        this.filename = filename;
		initAL();
		mainPA.integrate(new Routine(data));
		setVisible(true);
	}

	void initAL(){
		fileNewRoutMi.addActionListener(this);
		fileNewRecMi.addActionListener(this);
		fileOpenRoutMi.addActionListener(this);
		fileOpenRecMi.addActionListener(this);
		fileSaveMi.addActionListener(this);
		fileSaveAsMi.addActionListener(this);
		fileExitMi.addActionListener(this);
	}

	JPanel mkPane(int type){
	    JPanel c = new JPanel(new BorderLayout());
		switch(type){
			case 1:
				c.add(mainPA,BorderLayout.CENTER);
                c.add(fileLa, BorderLayout.SOUTH);
				break;
			case 2:
				c.add(mainRA,BorderLayout.CENTER);
                c.add(fileLa, BorderLayout.SOUTH);
				break;
		}
		return c;
	}

	JMenuBar mkMenu(int type){
        JMenuBar mb = new JMenuBar();
		fileM.removeAll();
		fileNewSM.removeAll();
		fileOpenSM.removeAll();
		switch(type){
			case 1:
				fileM.setMnemonic(KeyEvent.VK_F);
				fileNewSM.setMnemonic(KeyEvent.VK_N);
				fileNewSM.setIcon(new ImageIcon(getClass().getResource("images/filenew.png")));
				fileOpenSM.setMnemonic(KeyEvent.VK_O);
				fileOpenSM.setIcon(new ImageIcon(getClass().getResource("images/fileopen.png")));
				fileNewRoutMi.setMnemonic(KeyEvent.VK_T);
				fileNewRecMi.setMnemonic(KeyEvent.VK_C);
				fileOpenRoutMi.setMnemonic(KeyEvent.VK_T);
				fileOpenRecMi.setMnemonic(KeyEvent.VK_C);
				fileSaveMi.setMnemonic(KeyEvent.VK_S);
				fileSaveAsMi.setMnemonic(KeyEvent.VK_A);
				fileExitMi.setMnemonic(KeyEvent.VK_X);
				mb.add(fileM);
				fileM.add(fileNewSM);
				fileNewSM.add(fileNewRoutMi);
				fileNewSM.add(fileNewRecMi);
				fileM.add(fileOpenSM);
				fileOpenSM.add(fileOpenRoutMi);
				fileOpenSM.add(fileOpenRecMi);
				fileM.add(fileSaveMi);
				fileM.add(fileSaveAsMi);
				fileM.addSeparator();
				fileM.add(fileExitMi);
			case 2:
				mb = new JMenuBar();
				fileM.setMnemonic(KeyEvent.VK_F);
				fileNewSM.setMnemonic(KeyEvent.VK_N);
				fileNewSM.setIcon(new ImageIcon(getClass().getResource("images/filenew.png")));
				fileOpenSM.setMnemonic(KeyEvent.VK_O);
				fileOpenSM.setIcon(new ImageIcon(getClass().getResource("images/fileopen.png")));
				fileNewRoutMi.setMnemonic(KeyEvent.VK_T);
				fileNewRecMi.setMnemonic(KeyEvent.VK_C);
				fileOpenRoutMi.setMnemonic(KeyEvent.VK_T);
				fileOpenRecMi.setMnemonic(KeyEvent.VK_C);
				fileSaveMi.setMnemonic(KeyEvent.VK_S);
				fileSaveAsMi.setMnemonic(KeyEvent.VK_A);
				fileExitMi.setMnemonic(KeyEvent.VK_X);
				mb.add(fileM);
				fileM.add(fileNewSM);
				fileNewSM.add(fileNewRoutMi);
				fileNewSM.add(fileNewRecMi);
				fileM.add(fileOpenSM);
				fileOpenSM.add(fileOpenRoutMi);
				fileOpenSM.add(fileOpenRecMi);
				fileM.add(fileSaveMi);
				fileM.add(fileSaveAsMi);
				fileM.addSeparator();
				fileM.add(fileExitMi);
				break;
		}
		return mb;
	}

	public void actionPerformed(ActionEvent e){
		if(e.getSource() == fileExitMi)
			System.exit(0);
		if(e.getSource() == fileNewRoutMi){
			if(type == 0){
				type = 1;
				setTitle("FTC Bowl It Over Point Counter");
				setJMenuBar(mkMenu(1));
				setContentPane(this.mkPane(1));
				setSize(500,395);
				setVisible(false);
				setVisible(true);
			}else
               	new PointCounter(1);
        }if(e.getSource() == fileNewRecMi){
			if(type == 0){
				type = 2;
				setTitle("FTC Bowl It Over Point Record");
				setJMenuBar(mkMenu(2));
				setContentPane(this.mkPane(2));
				setSize(500,395);
				setVisible(false);
				setVisible(true);
			}else
				new PointCounter(2);
		}
		if(e.getSource() == fileOpenRoutMi)
			openRout();
		if(e.getSource() == fileOpenRecMi)
			openRec();
        if(e.getSource() == fileSaveMi){
 			if(type == 1)
 				saveRout();
 			if(type == 2)
 				saveRec();
		}
        if(e.getSource() == fileSaveAsMi){
            if(type == 1)
                saveAsRout();
            if(type == 2)
                saveAsRec();
        }
	}

	void openRout(){
        if(fc.showOpenDialog(this) == JFileChooser.APPROVE_OPTION){
            filename = fc.getSelectedFile().getAbsolutePath();
		    StringBuffer buffer = new StringBuffer();
		    try{
                FileInputStream fis = new FileInputStream(filename);
                InputStreamReader isr = new InputStreamReader(fis,"UTF8");
                BufferedReader in = new BufferedReader(isr);
                int ch;
                while ((ch = in.read()) > -1) {
                        buffer.append((char)ch);
                }
                in.close();
			} catch (FileNotFoundException e) {
                e.printStackTrace();  //To change body of catch statement use File | Settings | File Templates.
            } catch (UnsupportedEncodingException e) {
                e.printStackTrace();  //To change body of catch statement use File | Settings | File Templates.
            } catch (IOException e) {
                e.printStackTrace();  //To change body of catch statement use File | Settings | File Templates.
            }
            if(type == 0)
                setVisible(false);

            new PointCounter(1,buffer.toString(), filename);
        }
	}

	void openRec(){

	}

	void saveRout(){
            if(filename.equals("")){
                if(fc.showOpenDialog(this) == JFileChooser.APPROVE_OPTION){
                    File f = fc.getSelectedFile();
                    filename = f.getAbsolutePath();
                if(!filename.substring(filename.length()-4, filename.length()).equals(".rou"))
                    filename = filename + ".rou";
                fileLa.setText(filename);
                }
            }
            String write = mainPA.makeRoutine().represent();
            FileOutputStream out = null;
            char[] writeChars = write.toCharArray();
            byte[] writeBytes = new byte[writeChars.length];
            for(int i = 0; i < writeChars.length; i++)
                writeBytes[i] = (byte)writeChars[i];
            try{
                out = new FileOutputStream(new File(filename));
                out.write(writeBytes);
                out.close();
            }catch(FileNotFoundException fne){}
            catch(IOException ioe){}
	}

	void saveAsRout(){
        if(fc.showOpenDialog(this) == JFileChooser.APPROVE_OPTION){
            File f = fc.getSelectedFile();
            filename = f.getAbsolutePath();
            if(!filename.substring(filename.length() - 4, filename.length()).equals(".rou"))
                filename = filename + ".rou";
            fileLa.setText(filename);
            String write = mainPA.makeRoutine().represent();
            FileOutputStream out = null;
            char[] writeChars = write.toCharArray();
            byte[] writeBytes = new byte[writeChars.length];
            for(int i = 0; i < writeChars.length; i++)
                writeBytes[i] = (byte)writeChars[i];
            try{
                out = new FileOutputStream(new File(filename));
                out.write(writeBytes);
                out.close();
            }catch(FileNotFoundException fne){}
            catch(IOException ioe){}
        }
	}

	void saveRec(){
	}

    void saveAsRec(){

    }

	public static void main(String[] args){
		new PointCounter();
	}

	class RecArea extends JPanel{
		public RecArea(){

		}
	}
}