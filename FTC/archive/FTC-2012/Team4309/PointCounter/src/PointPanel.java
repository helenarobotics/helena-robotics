import javax.swing.*;
import java.awt.*;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.geom.Rectangle2D;
import java.text.DecimalFormat;

public class PointPanel extends JPanel implements Requester{
    //TODO Add Comments to clear up code
	private final JCheckBox homeGoalBox = new JCheckBox("On Home Goal", false);
	private final DecimalFormat df = new DecimalFormat("#,###.##");
	private JPanel contentPa = new JPanel();
	private final JPanel base = new JPanel(new BorderLayout());
	private final JPanel[] extra = new JPanel[10];
	private final JTextField lengthFi = new JTextField(3);
	private JComboBox listCmb = new JComboBox();
    private RTask tTask = new RTask();
	private final JTextField countFi = new JTextField(3);
	private final JTextField baseHeightFi = new JTextField(3);
	private final JTextField crateNumFi = new JTextField(3);
	private final JButton removeBu = new JButton("-");

	private final ActionListener enterLr = new ActionListener(){
		public void actionPerformed(ActionEvent e){
			procTime();
		}
	};

	private final ActionListener typeCh = new ActionListener(){
		public void actionPerformed(ActionEvent e){
			pts = 0;
			procType();
		}
	};

	private final ActionListener cmbCh = new ActionListener(){
		public void actionPerformed(ActionEvent e){
			pts = 0;
			procPts(e);
		}
	};

	private final ActionListener removeLr = new ActionListener(){
		public void actionPerformed(ActionEvent e){
			Message removeM = new Message(Integer.toString(id), PointPanel.this);
			rem.request(removeM);
		}
	};

	private final String[] autoScore = {"Robot Park: Front","Robot Park: Back","Bowling Ball Park: Front","Bowling Ball Park: Back","Upright Crates"};
	private final String[] teleScore = {"Balls - Protected Zone","Balls - Crate","Crate Stack","Magnet Balls"};
	private final String[] endScore = {"Balls - Protected Zone","Balls - Crate","Crate Stack","Magnet Balls","Bowling Ball Park: Home Goal","Bowling Ball Park: Home Zone"};
	private final String[] types =  {"Autonomous","Tele-Op","End Game"};
	private final JComboBox typeCmb = new JComboBox(types);

	private int length = 0, scoretype = 0, buildN = 0;
	private double pts = 0.0f, pps = 0.0f;

	private final Requester rem, upd;

	private int id;

	public PointPanel(int id, Requester remove, Requester update){
		super(new BorderLayout());
		this.id = id;
		rem = remove;
		upd = update;
		init();
		contentPa = build(0);
		add(contentPa,BorderLayout.CENTER);
		add(removeBu,BorderLayout.SOUTH);
        updateUI();
	}

    public PointPanel(int id, Requester remove, Requester update, RTask in){
        super(new BorderLayout());
        this.id = id;
		rem = remove;
		upd = update;
        lengthFi.setText(Integer.toString(in.getLength()));
        setFields(in);
        pts = procIDpts(in.getID());
        upd.request(new Message("",this));
        init();
        contentPa = build(in.getID());
		add(contentPa,BorderLayout.CENTER);
		add(removeBu, BorderLayout.SOUTH);
        updateUI();
        tTask = in;
        buildN = in.getID();
    }

    public void setFields(RTask in){
        length = in.getLength();
        int build = in.getID();
        switch(build){
            case 0:
            case 1:
            case 2:
                scoretype = 0;
                break;
            case 11:
            case 12:
            case 13:
            case 14:
            case 15:
            case 115:
                scoretype = 1;
                typeCmb.setSelectedIndex(0);
                break;
            case 21:
            case 22:
            case 23:
            case 24:
            case 121:
            case 122:
            case 123:
            case 124:
                scoretype = 2;
                typeCmb.setSelectedIndex(1);
                break;
            case 25:
            case 26:
                scoretype = 2;
                typeCmb.setSelectedIndex(2);
                break;
        }switch(build){
            case 0:
            case 1:
            case 2:
                break;
            case 11:
            case 12:
            case 13:
            case 14:
                listCmb.removeAll();
                listCmb = new JComboBox(autoScore);
                listCmb.setSelectedIndex(build-11);
                typeCmb.setSelectedIndex(0);
                break;
            case 15:
            case 115:
                listCmb.removeAll();
                listCmb = new JComboBox(teleScore);
                listCmb.setSelectedIndex(4);
                typeCmb.setSelectedIndex(0);
                break;
            case 121:
            case 122:
            case 123:
            case 124:
                listCmb.removeAll();
                listCmb = new JComboBox(teleScore);
                listCmb.setSelectedIndex(build - 121);
                typeCmb.setSelectedIndex(1);
                break;
            case 21:
            case 22:
            case 23:
            case 24:
                listCmb.removeAll();
                listCmb = new JComboBox(teleScore);
                listCmb.setSelectedIndex(build-21);
                typeCmb.setSelectedIndex(1);
                break;
            case 25:
            case 26:
                listCmb.removeAll();
                listCmb = new JComboBox(endScore);
                listCmb.setSelectedIndex(build-21);
                typeCmb.setSelectedIndex(2);
                break;
        }
        switch(build){
            case 0:
            case 1:
            case 2:
                break;
            case 11:
            case 12:
            case 13:
            case 14:
            case 25:
            case 26:
                break;
            case 115:
            case 121:
            case 122:
            case 124:
                countFi.setText(Integer.toString((int)in.getParam(0)));
                break;
            case 123:
                baseHeightFi.setText(Float.toString(in.getParam(0)));
                crateNumFi.setText(Integer.toString((int)in.getParam(1)));
                if(in.getParam(2) == 0.0)
                    homeGoalBox.setSelected(false);
                if(in.getParam(2) == 1.0)
                    homeGoalBox.setSelected(true);
                break;
        }
    }

    public double procIDpts(int build){
        boolean cont = false;
        switch(build){
            case 11:
            case 12:
            case 13:
            case 14:
            case 115:
            case 121:
            case 122:
            case 123:
            case 124:
            case 25:
            case 26:
                cont = true;
                break;
        }
        if(cont){
            double points = 0.0;
            switch(build){
                case 11:
                    points = 10.0;
                    break;
                case 12:
                    points = 5.0;
                    break;
                case 13:
                    points = 10.0;
                    break;
                case 14:
                    points = 20.0;
                    break;
				case 115:
					points = Float.parseFloat(countFi.getText())*5;
					break;
				case 121:
					points = Float.parseFloat(countFi.getText());
					break;
				case 122:
					points = Float.parseFloat(countFi.getText())*2;
					break;
                case 123:
			        float bheight = Float.parseFloat(baseHeightFi.getText());
			        int crateN = Integer.parseInt(crateNumFi.getText());
                    if(!homeGoalBox.isSelected()){
                        for(int i = 0; i < crateN; i++){
                            if(bheight+(i+1)*6.125 > 10.5){
                                points+=10*Math.ceil((bheight+(i+1)*6.125 - 10.5)/6);
                            }
                        }
                    }else{
                        for(int i = 0; i < crateN; i++){
                            if(bheight+(i+1)*6.125 + 6.125 > 10.5){
                                points+=10*Math.ceil((bheight+(i+1)*6.125 + 6.125 - 10.5)/6);
                            }
                        }
                    }
                    break;
				case 124:
					points = Float.parseFloat(countFi.getText())*25;
					break;
                case 25:
                    points = 30.0;
                    break;
                case 26:
                    points = 20.0;
                    break;
            }
        pps = points/length;
        return points;
        }else
            return 0;

    }

    public double getPts(){
        return pts;
    }

    public int getLength(){
        return length;
    }

    public void setID(int id){
        this.id = id;
    }

    public RTask getTask(){
        return tTask;
    }

	void procTime(){
		if(!lengthFi.getText().equals("")){
			length = Integer.parseInt(lengthFi.getText());
            tTask.setLength(length);
			contentPa = build(1);
			updateUI();
		}
	}

	void procType(){
		if(typeCmb.getSelectedIndex() == 2){
			scoretype = 2;
			listCmb.removeAllItems();
            for (String anEndScore : endScore) listCmb.addItem(anEndScore);
		}

		else if(typeCmb.getSelectedIndex() == 1){
			scoretype = 2;
			listCmb.removeAllItems();
            for (String aTeleScore : teleScore) listCmb.addItem(aTeleScore);
		}

		else{
			scoretype = 1;
			listCmb.removeAllItems();
            for (String anAutoScore : autoScore) listCmb.addItem(anAutoScore);
		}
		contentPa = build(2);
		pts = 0;
		updateUI();
	}

	void procPts(ActionEvent e){
		if(e.getSource() == listCmb){
			if(scoretype == 1){
				buildN = listCmb.getSelectedIndex()+11;
				switch(buildN){
					default:
						break;
					case 11:
						pts = 10.0;
						break;
					case 12:
						pts = 5.0;
						break;
					case 13:
						pts = 10.0;
						break;
					case 14:
						pts = 20.0;
						break;
				}
			countFi.setText("");
			pps = pts/Double.parseDouble(Integer.toString(length));
			contentPa = build(buildN);
			updateUI();
			}else if(scoretype == 2){
				buildN = listCmb.getSelectedIndex()+21;
				switch(buildN){
					default:
						break;
					case 25:
						pts = 30.0;
						break;
					case 26:
						pts = 20.0;
						break;
				}
			countFi.setText("");
			pps = pts/Double.parseDouble(Integer.toString(length));
			contentPa = build(buildN);
			updateUI();
		    }
		}else if(e.getSource() == countFi){
            tTask.setParam(0, Float.parseFloat(countFi.getText()));
			switch(buildN){
				case 15:
					pts = Float.parseFloat(countFi.getText())*5;
					break;
				case 21:
					pts = Float.parseFloat(countFi.getText());
					break;
				case 22:
					pts = Float.parseFloat(countFi.getText())*2;
					break;
				case 24:
					pts = Float.parseFloat(countFi.getText())*25;
					break;
			}

			pps = pts/Double.parseDouble(Integer.toString(length));
			contentPa = build(buildN+100);
			updateUI();
		}else{
			float bheight = Float.parseFloat(baseHeightFi.getText());
			int crateN = Integer.parseInt(crateNumFi.getText());
            tTask.setParam(0, bheight);
            tTask.setParam(1, (float)crateN);
            if(homeGoalBox.isSelected())
                tTask.setParam(2, 1f);
            else
                tTask.setParam(2, 0f);
			pts = 0;
			if(!homeGoalBox.isSelected()){
				for(int i = 0; i < crateN; i++){
					if(bheight+(i+1)*6.125 > 10.5){
						pts+=10*Math.ceil((bheight+(i+1)*6.125 - 10.5)/6);
					}
				}
			}else{
				for(int i = 0; i < crateN; i++){
					if(bheight+(i+1)*6.125 + 6.125 > 10.5){
						pts+=10*Math.ceil((bheight+(i+1)*6.125 + 6.125 - 10.5)/6);
					}
				}
			}
			pps = pts/Double.parseDouble(Integer.toString(length));
			contentPa = build(123);
			updateUI();
		}
	}

	void init(){
		lengthFi.addActionListener(enterLr);
		typeCmb.addActionListener(typeCh);
		listCmb.addActionListener(cmbCh);
		countFi.addActionListener(cmbCh);
		crateNumFi.addActionListener(cmbCh);
		removeBu.addActionListener(removeLr);

		for(int i = 0; i < extra.length; i++)
			extra[i] = new JPanel();

		setPreferredSize(new Dimension(230,280));
	}

	JPanel build(int stage){
		base.removeAll();
        for (JPanel anExtra : extra) anExtra.removeAll();
        tTask.setID(stage);
		switch(stage){
			case 0: //initial
				extra[0].setLayout(new GridLayout(2,1,2,2));
				extra[0].add(new JLabel("Time",SwingConstants.CENTER));
				extra[1].add(lengthFi);
				extra[0].add(extra[1]);
				base.add(extra[0], BorderLayout.NORTH);
				break;
			case 1: //select type
				extra[0].setLayout(new GridLayout(3,1,2,2));
				extra[0].add(new JLabel("Time",SwingConstants.CENTER));
				extra[1].add(lengthFi);
				extra[0].add(extra[1]);
				extra[0].add(typeCmb);
				base.add(extra[0], BorderLayout.NORTH);
				break;
			case 2: //select action
				extra[0].setLayout(new GridLayout(3,1,2,2));
				extra[0].add(new JLabel("Time",SwingConstants.CENTER));
				extra[1].add(lengthFi);
				extra[0].add(extra[1]);
				extra[0].add(typeCmb);
				base.add(extra[0], BorderLayout.NORTH);
				extra[2].setLayout(new BorderLayout());
				extra[2].add(listCmb, BorderLayout.NORTH);
				extra[2].add(new JLabel(), BorderLayout.CENTER);
				base.add(extra[2], BorderLayout.CENTER);
				break;
			case 11: //score type a1 Park Robot - Front
			case 12: //score type a2 Park Robot - Back
			case 13: //score type a3 Park Bowling Ball - Front
			case 14: //score type a4 Park Bowling Ball - Back
				extra[0].setLayout(new GridLayout(3,1,2,2));
				extra[0].add(new JLabel("Time",SwingConstants.CENTER));
				extra[1].add(lengthFi);
				extra[0].add(extra[1]);
				extra[0].add(typeCmb);
				base.add(extra[0], BorderLayout.NORTH);
				extra[2].setLayout(new BorderLayout());
				extra[2].add(listCmb, BorderLayout.NORTH);
				extra[2].add(new JLabel(), BorderLayout.CENTER);
				base.add(extra[2], BorderLayout.CENTER);
				extra[3].setLayout(new GridLayout(1,2,2,2));
				extra[3].add(new JLabel(df.format(pps)+"pps",SwingConstants.LEFT));
				extra[3].add(new JLabel(df.format(pts)+"pts",SwingConstants.RIGHT));
				upd.request(new Message("",this));
				base.add(extra[3], BorderLayout.SOUTH);
				break;
			case 15: //score type a5 Uprighting Crate w/o pts
				extra[0].setLayout(new GridLayout(3,1,2,2));
				extra[0].add(new JLabel("Time",SwingConstants.CENTER));
				extra[1].add(lengthFi);
				extra[0].add(extra[1]);
				extra[0].add(typeCmb);
				base.add(extra[0], BorderLayout.NORTH);
				extra[2].setLayout(new BorderLayout());
				extra[2].add(listCmb, BorderLayout.NORTH);
				extra[3].setLayout(new GridLayout(1,2,2,2));
				extra[3].add(new JLabel("# of crates"));
				extra[3].add(countFi);
				extra[2].add(extra[3], BorderLayout.CENTER);
				base.add(extra[2], BorderLayout.CENTER);
				break;
			case 115: //score type a5 Uprighting Crate w/ pts
				extra[0].setLayout(new GridLayout(3,1,2,2));
				extra[0].add(new JLabel("Time",SwingConstants.CENTER));
				extra[1].add(lengthFi);
				extra[0].add(extra[1]);
				extra[0].add(typeCmb);
				base.add(extra[0], BorderLayout.NORTH);
				extra[2].setLayout(new BorderLayout());
				extra[2].add(listCmb, BorderLayout.NORTH);
				extra[3].setLayout(new GridLayout(1,2,2,2));
				extra[3].add(new JLabel("# of crates"));
				extra[3].add(countFi);
				extra[2].add(extra[3], BorderLayout.CENTER);
				base.add(extra[2], BorderLayout.CENTER);
				extra[4].setLayout(new GridLayout(1,2,2,2));
				extra[4].add(new JLabel(df.format(pps)+"pps",SwingConstants.LEFT));
				extra[4].add(new JLabel(df.format(pts)+"pts",SwingConstants.RIGHT));
				upd.request(new Message("",this));
				base.add(extra[4], BorderLayout.SOUTH);
				break;
			case 21: //score type t1 Balls - Protected Zone w/o pts
				extra[0].setLayout(new GridLayout(3,1,2,2));
				extra[0].add(new JLabel("Time",SwingConstants.CENTER));
				extra[1].add(lengthFi);
				extra[0].add(extra[1]);
				extra[0].add(typeCmb);
				base.add(extra[0], BorderLayout.NORTH);
				extra[2].setLayout(new BorderLayout());
				extra[2].add(listCmb, BorderLayout.NORTH);
				extra[3].setLayout(new GridLayout(1,2,2,2));
				extra[3].add(new JLabel("# of balls:",SwingConstants.CENTER));
				extra[3].add(countFi);
				extra[2].add(extra[3], BorderLayout.CENTER);
				base.add(extra[2], BorderLayout.CENTER);
				break;
			case 121:
				extra[0].setLayout(new GridLayout(3,1,2,2));
				extra[0].add(new JLabel("Time",SwingConstants.CENTER));
				extra[1].add(lengthFi);
				extra[0].add(extra[1]);
				extra[0].add(typeCmb);
				base.add(extra[0], BorderLayout.NORTH);
				extra[2].setLayout(new BorderLayout());
				extra[2].add(listCmb, BorderLayout.NORTH);
				extra[3].setLayout(new GridLayout(1,2,2,2));
				extra[3].add(new JLabel("# of balls:",SwingConstants.CENTER));
				extra[3].add(countFi);
				extra[2].add(extra[3], BorderLayout.CENTER);
				base.add(extra[2], BorderLayout.CENTER);
				extra[4].setLayout(new GridLayout(1,2,2,2));
				extra[4].add(new JLabel(df.format(pps)+"pps",SwingConstants.LEFT));
				extra[4].add(new JLabel(df.format(pts)+"pts",SwingConstants.RIGHT));
				upd.request(new Message("",this));
				base.add(extra[4], BorderLayout.SOUTH);
				break;
			case 22: //score type t2 Balls - Crate w/o pts
				extra[0].setLayout(new GridLayout(3,1,2,2));
				extra[0].add(new JLabel("Time",SwingConstants.CENTER));
				extra[1].add(lengthFi);
				extra[0].add(extra[1]);
				extra[0].add(typeCmb);
				base.add(extra[0], BorderLayout.NORTH);
				extra[2].setLayout(new BorderLayout());
				extra[2].add(listCmb, BorderLayout.NORTH);
				extra[3].setLayout(new GridLayout(1,2,2,2));
				extra[3].add(new JLabel("# of balls:",SwingConstants.CENTER));
				extra[3].add(countFi);
				extra[2].add(extra[3], BorderLayout.CENTER);
				base.add(extra[2], BorderLayout.CENTER);
				break;
			case 122:
				extra[0].setLayout(new GridLayout(3,1,2,2));
				extra[0].add(new JLabel("Time",SwingConstants.CENTER));
				extra[1].add(lengthFi);
				extra[0].add(extra[1]);
				extra[0].add(typeCmb);
				base.add(extra[0], BorderLayout.NORTH);
				extra[2].setLayout(new BorderLayout());
				extra[2].add(listCmb, BorderLayout.NORTH);
				extra[3].setLayout(new GridLayout(1,2,2,2));
				extra[3].add(new JLabel("# of balls:",SwingConstants.CENTER));
				extra[3].add(countFi);
				extra[2].add(extra[3], BorderLayout.CENTER);
				base.add(extra[2], BorderLayout.CENTER);
				extra[4].setLayout(new GridLayout(1,2,2,2));
				extra[4].add(new JLabel(df.format(pps)+"pps",SwingConstants.LEFT));
				extra[4].add(new JLabel(df.format(pts)+"pts",SwingConstants.RIGHT));
				upd.request(new Message("",this));
				base.add(extra[4], BorderLayout.SOUTH);
				break;
			case 23: //score type t3 Crate System w/o pts
				extra[0].setLayout(new GridLayout(3,1,2,2));
				extra[0].add(new JLabel("Time",SwingConstants.CENTER));
				extra[1].add(lengthFi);
				extra[0].add(extra[1]);
				extra[0].add(typeCmb);
				base.add(extra[0], BorderLayout.NORTH);
				extra[2].setLayout(new BorderLayout());
				extra[2].add(listCmb, BorderLayout.NORTH);
				extra[3].setLayout(new GridLayout(2,2,2,2));
				extra[3].add(new JLabel("Base Height:",SwingConstants.CENTER));
				extra[3].add(baseHeightFi);
				extra[3].add(new JLabel("# of crates",SwingConstants.CENTER));
				extra[3].add(crateNumFi);
				extra[2].add(extra[3], BorderLayout.CENTER);
				extra[2].add(homeGoalBox, BorderLayout.SOUTH);
				base.add(extra[2], BorderLayout.CENTER);
				break;
			case 123:
				extra[0].setLayout(new GridLayout(3,1,2,2));
				extra[0].add(new JLabel("Time",SwingConstants.CENTER));
				extra[1].add(lengthFi);
				extra[0].add(extra[1]);
				extra[0].add(typeCmb);
				base.add(extra[0], BorderLayout.NORTH);
				extra[2].setLayout(new BorderLayout());
				extra[2].add(listCmb, BorderLayout.NORTH);
				extra[3].setLayout(new GridLayout(2,2,2,2));
				extra[3].add(new JLabel("Base Height:",SwingConstants.CENTER));
				extra[3].add(baseHeightFi);
				extra[3].add(new JLabel("# of crates",SwingConstants.CENTER));
				extra[3].add(crateNumFi);
				extra[2].add(extra[3], BorderLayout.CENTER);
				extra[2].add(homeGoalBox, BorderLayout.SOUTH);
				base.add(extra[2], BorderLayout.CENTER);
				extra[4].setLayout(new GridLayout(1,2,2,2));
				extra[4].add(new JLabel(df.format(pps)+"pps",SwingConstants.LEFT));
				extra[4].add(new JLabel(df.format(pts)+"pts",SwingConstants.RIGHT));
				upd.request(new Message("",this));
				base.add(extra[4], BorderLayout.SOUTH);
				break;
			case 24: //score type t4 Magnet Balls w/o pts
				extra[0].setLayout(new GridLayout(3,1,2,2));
				extra[0].add(new JLabel("Time",SwingConstants.CENTER));
				extra[1].add(lengthFi);
				extra[0].add(extra[1]);
				extra[0].add(typeCmb);
				base.add(extra[0], BorderLayout.NORTH);
				extra[2].setLayout(new BorderLayout());
				extra[2].add(listCmb, BorderLayout.NORTH);
				extra[3].setLayout(new GridLayout(1,2,2,2));
				extra[3].add(new JLabel("# of magnet balls:",SwingConstants.CENTER));
				extra[3].add(countFi);
				extra[2].add(extra[3], BorderLayout.CENTER);
				base.add(extra[2], BorderLayout.CENTER);
				break;
			case 124:
				extra[0].setLayout(new GridLayout(3,1,2,2));
				extra[0].add(new JLabel("Time",SwingConstants.CENTER));
				extra[1].add(lengthFi);
				extra[0].add(extra[1]);
				extra[0].add(typeCmb);
				base.add(extra[0], BorderLayout.NORTH);
				extra[2].setLayout(new BorderLayout());
				extra[2].add(listCmb, BorderLayout.NORTH);
				extra[3].setLayout(new GridLayout(1,2,2,2));
				extra[3].add(new JLabel("# of magnet balls:",SwingConstants.CENTER));
				extra[3].add(countFi);
				extra[2].add(extra[3], BorderLayout.CENTER);
				base.add(extra[2], BorderLayout.CENTER);
				extra[4].setLayout(new GridLayout(1,2,2,2));
				extra[4].add(new JLabel(df.format(pps)+"pps",SwingConstants.LEFT));
				extra[4].add(new JLabel(df.format(pts)+"pts",SwingConstants.RIGHT));
				upd.request(new Message("",this));
				base.add(extra[4], BorderLayout.SOUTH);
				break;
			case 25: //score type e1 Bowling Ball Park - Home Goal
			case 26: //score type e2 Bowling Ball Park - Home Zone
				extra[0].setLayout(new GridLayout(3,1,2,2));
				extra[0].add(new JLabel("Time",SwingConstants.CENTER));
				extra[1].add(lengthFi);
				extra[0].add(extra[1]);
				extra[0].add(typeCmb);
				base.add(extra[0], BorderLayout.NORTH);
				extra[2].setLayout(new BorderLayout());
				extra[2].add(listCmb, BorderLayout.NORTH);
				extra[2].add(new JLabel(), BorderLayout.CENTER);
				base.add(extra[2], BorderLayout.CENTER);
				extra[3].setLayout(new GridLayout(1,2,2,2));
				extra[3].add(new JLabel(df.format(pps)+"pps",SwingConstants.LEFT));
				extra[3].add(new JLabel(df.format(pts)+"pts",SwingConstants.RIGHT));
				upd.request(new Message("",this));
				base.add(extra[3], BorderLayout.SOUTH);
				break;
		}
		return base;
	}

    public void request(Message msg){}
}