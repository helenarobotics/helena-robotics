import javax.swing.*;
import java.awt.*;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.text.DecimalFormat;
import java.util.Vector;

class PointArea extends JPanel{

	private final JLabel pointsLa = new JLabel();
	private final JLabel ppsLa = new JLabel();
	private final JLabel appsLa = new JLabel();
	private final DecimalFormat df = new DecimalFormat("#,###.##");

	private final Requester remove = new Requester(){
		public void request(Message msg){
			int id = Integer.parseInt(msg.getMessage());
			if(id > 0){
				dbase.remove(id);
				mainPa.removeAll();
				extraPa[1].removeAll();
				mainPa.add(build());
				for(int i = dbase.size()-1; i > id-1; i--){
					drawPa = dbase.get(i);
					drawPa.setID(i);
					dbase.set(i, drawPa);
				}
				updateUI();
			}
		}
	};

	private final Requester update = new Requester(){
		public void request(Message msg){
			float pts = 0;
                	int length = 0;
                	for (PointPanel aDbase : dbase) {
                    		pts += aDbase.getPts();
                    		length += aDbase.getLength();
                	}
                	float avpps = pts / Float.parseFloat(Integer.toString(length));
                	float pps = pts / 150.0f;
               		pointsLa.setText("pts: " + df.format((int)pts));
			ppsLa.setText("gpps: " + df.format(pps));
			appsLa.setText("apps: " + df.format(avpps));
		}
	};

	private final JPanel mainPa = new JPanel(new BorderLayout());
	private final JPanel subPa = new JPanel();
	private final JPanel[] extraPa = new JPanel[5];
	private PointPanel drawPa = new PointPanel(0, remove, update);
	private final Vector<PointPanel> dbase = new Vector<PointPanel>();
    private final JButton addBu = new JButton("+");

    public PointArea(){
		drawPa.setBorder(BorderFactory.createLineBorder(Color.black));
		drawPa.setID(dbase.size()-1);
		dbase.add(drawPa);
        ActionListener addLr = new ActionListener() {
            public void actionPerformed(ActionEvent e) {
                drawPa = new PointPanel(dbase.size(), remove, update);
                drawPa.setBorder(BorderFactory.createLineBorder(Color.black));
                dbase.add(drawPa);
                mainPa.removeAll();
                mainPa.add(build());
                updateUI();
            }
        };
        addBu.addActionListener(addLr);
	for(int i = 0; i < extraPa.length; i++)
		extraPa[i] = new JPanel();
	mainPa.add(build());
        JScrollPane horiz = new JScrollPane(mainPa, ScrollPaneConstants.VERTICAL_SCROLLBAR_NEVER, ScrollPaneConstants.HORIZONTAL_SCROLLBAR_AS_NEEDED);
        horiz.setPreferredSize(new Dimension(465, 290));
		add(horiz,BorderLayout.CENTER);
		add(extraPa[2], BorderLayout.SOUTH);
		updateUI();
	}

    public Routine makeRoutine(){
        Routine rou = new Routine();
        for(int i = 0; i < dbase.size(); i++)
            rou.addTask(dbase.get(i).getTask());
        return rou;
    }

    public void integrate(Routine in){
        dbase.setSize(in.getSize());
        for(int i = 0; i < in.getSize(); i++){
            PointPanel x = new PointPanel(i, remove, update, in.getTask(i));
            x.setBorder(BorderFactory.createLineBorder(Color.BLACK));
            dbase.set(i, x);
        }
        mainPa.removeAll();
        mainPa.add(build());
        updateUI();
    }

	JPanel build(){
        for (JPanel anExtraPa : extraPa) anExtraPa.removeAll();
		subPa.setLayout(new BorderLayout());
		if(dbase.size()>1)
			extraPa[1].setLayout(new GridLayout(1,dbase.size(),2,2));
		else
			extraPa[1].setLayout(new GridLayout(1,2,2,2));

        for (PointPanel aDbase : dbase) {
            extraPa[1].add(aDbase);
        }
		if(dbase.size() == 1)
			extraPa[1].add(new JLabel());
		subPa.add(extraPa[1], BorderLayout.CENTER);
		extraPa[2].setLayout(new BorderLayout());
		extraPa[2].add(addBu,BorderLayout.SOUTH);
		extraPa[2].add(extraPa[3], BorderLayout.CENTER);
		extraPa[3].setLayout(new BorderLayout(30,30));
		extraPa[3].add(pointsLa, BorderLayout.WEST);
		extraPa[3].add(ppsLa, BorderLayout.CENTER);
		extraPa[3].add(appsLa, BorderLayout.EAST);
		return subPa;
	}
}