import java.io.*;
import java.awt.*;
import javax.swing.*;
import java.awt.event.*;
import java.util.*;
import java.awt.geom.*;
import java.awt.image.BufferedImage;

public class WireGrapher extends JFrame {

  private String fileName = null;
  private int width = 740;
  private int height = 740;
  private float scaleX = 0.0f;
  private float scaleY = 0.0f;
  private int maxX = 0;
  private int maxY = 0;
  
  private ArrayList<Ellipse2D.Float> dots = null;
  private ArrayList<ArrayList<Line2D.Float>> wires = null;
  
  private static final int dotRadius  = 2;
  private static final int wireSize   = 2;
  private static final int boundaries = 50;
  private static final Color bgColor = Color.LIGHT_GRAY;
  private static final Color dotColor = Color.black;
  private static final Color [] wireColors = new Color [] {
    Color.blue, Color.black, Color.green, Color.red, Color.cyan, Color.magenta,
    new Color (90, 200, 90), new Color(187,92, 80), new Color(90, 88, 177)};
  
    private BufferedImage bufImg;

  public void setFileName(String fileName) {
    this.fileName = fileName;
  }
  
  public void setDimensions(int width, int height) {
    this.width = width;
    this.height = height;
  }

  public void paint(Graphics g) {
    Graphics2D g2d = (Graphics2D) g;
    g2d.drawImage(bufImg, null, 0, 0);
  }
  
  public void paintPicture(Graphics g) {
    Graphics2D g2d = (Graphics2D) g;
    Iterator<ArrayList<Line2D.Float>> i;
    Iterator<Line2D.Float> j;
    Iterator<Ellipse2D.Float> k;
    System.out.println("painting...");
    g2d.setRenderingHint(RenderingHints.KEY_ANTIALIASING, 
                         RenderingHints.VALUE_ANTIALIAS_ON);
    g2d.setColor(bgColor);
    g2d.fillRect(0, 0, width, height);
/*    Line2D lala = new Line2D.Float(50, 50, width - 50, height - 50);
    g2d.setColor(wireColor);
    g2d.draw(lala);
*/
    i = wires.iterator();
    ArrayList<Line2D.Float> wire;
    Line2D.Float segment;
    int color_index = 0;
    while(i.hasNext()) {
      g2d.setColor(wireColors[color_index]);
      color_index = (color_index + 1) % wireColors.length;
      wire = (i.next());
      j = wire.iterator();
      while (j.hasNext()) {
        segment = j.next();
        g2d.draw(segment);
      }
    }
    
    g2d.setColor(dotColor);
    k = this.dots.iterator();
    Ellipse2D.Float dot;
    while(k.hasNext()) {
      dot =  k.next();
      g2d.fill(dot);
    }
    
  }
  
  public void go() {
    try {
      BufferedReader br = new BufferedReader(new FileReader(fileName));
      String line;
      String [] pieces;
      line = br.readLine().trim();
      pieces = line.split("\\s+");
      maxX = Integer.parseInt(pieces[0]);
      maxY = Integer.parseInt(pieces[1]);
      
      System.out.println("" + maxX + "x" + maxY);
      scaleX = (width - 2.0f * boundaries)/maxX;
      scaleY = (height - 2.0f * boundaries)/maxY;
      System.out.println("Scale: " + scaleX + "x" + scaleY);
      
      br.readLine(); 
      
      int x0, y0, x1, y1;
      Float coordX0, coordY0, coordX1, coordY1;
      int i;
      ArrayList<Ellipse2D.Float> dots = new ArrayList<Ellipse2D.Float>();
      ArrayList<ArrayList<Line2D.Float>> wires = new ArrayList<ArrayList<Line2D.Float>>();
      
      while ((line = br.readLine()) != null) {
        ArrayList<Line2D.Float> wire = new ArrayList<Line2D.Float>();
        line = line.trim();
        if (line.equals("")) break;
        pieces = line.split("\\s+");
        if (pieces.length < 2 || (pieces.length%2) != 0) 
          throw new Exception (
              "Error, in line " + line + ". Pieces Length=" + pieces.length);
        x1 = Integer.parseInt(pieces[0]);
        y1 = Integer.parseInt(pieces[1]);
              
        coordX1 = boundaries + scaleX * x1 - dotRadius;
        coordY1 = boundaries + scaleX * y1 - dotRadius;
        Ellipse2D.Float dotStart = new Ellipse2D.Float(
                                   coordX1, coordY1, 2*dotRadius, 2*dotRadius);
        dots.add(dotStart);
        
        coordX1 += dotRadius;
        coordY1 += dotRadius;
        
        for (i = 1; i < pieces.length/2; i++) {
          x0 = x1;
          y0 = y1;
          coordX0 = coordX1;
          coordY0 = coordY1;
          
          x1 = Integer.parseInt(pieces[2*i]);
          y1 = Integer.parseInt(pieces[2*i + 1]);
          coordX1 = boundaries + scaleX * x1;
          coordY1 = boundaries + scaleX * y1;
          Line2D.Float segment = new Line2D.Float(
                                  coordX0, coordY0, coordX1, coordY1);
          wire.add(segment);
        }
        
        coordX1 -= dotRadius;
        coordY1 -= dotRadius;
        Ellipse2D.Float dotEnd = new Ellipse2D.Float(
                                 coordX1, coordY1, 2*dotRadius, 2*dotRadius);
        dots.add(dotEnd);
      
        wires.add(wire);  
      }
      this.dots = dots;
      this.wires = wires;
      this.setDimensions(width, height);
      this.setSize(width,height);
      this.setResizable(false);
      this.setBackground(bgColor);

      // compute image
      bufImg = new BufferedImage(width, height, BufferedImage.TYPE_INT_ARGB);
      paintPicture(bufImg.createGraphics());

      setVisible(true);

      addWindowListener(new WindowAdapter() {
        public void windowClosing(WindowEvent e) {
          System.exit(0);
        }
      });
    }
    catch (Exception exc) {
      exc.printStackTrace();
      System.err.println(exc.getMessage()); 
    }
  }

  public static void main (String args[]) {
    WireGrapher g = new WireGrapher();

    if (args.length >= 1) g.setFileName(args[0]);
    else g.setFileName("circuit_128x128_32.txt");
    if (args.length >= 3) {
      g.setDimensions(Integer.parseInt(args[1]),
                      Integer.parseInt(args[2]));
    }
    
    g.go();    
  }
}
