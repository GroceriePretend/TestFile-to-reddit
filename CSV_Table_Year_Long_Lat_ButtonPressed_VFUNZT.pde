import processing.serial.*;
Serial myPort;
float sensorVal; 
boolean portInitialized = false; //Variable for no port is found 
String val;
Table table;

void setup() {
  size (1000, 1000);
  tableVoid();
  portVoid();
}

  void tableVoid () {

    table = loadTable("test.csv", "header");
    // println("CheckSetup1");
  }

  void portVoid() {
    if (myPort !=null) {
    myPort.stop(); //schließt port 
    }
    
    myPort = new Serial(this, "COM7", 115200); 
    
    if (portInitialized==false) {
      sensorVal = map((mouseX), 0, width, 0, 255); //if no port accesible, use mouse
    }
    if ( portInitialized && myPort.available() >= 0) {
      val = myPort.readStringUntil('\n');             // if you receive data 
      try {
        sensorVal = Integer.valueOf(val.trim());       //Write it to sensorVal
      }
      catch(Exception e) { 
       // println("Error");
      }
      //println(sensorVal);
    }
    //println(sensorVal);
    //println("CheckSetup2");
  }

void draw(){
portVoid();

}

void serialEvent(Serial myPort) {

  String message = trim(myPort.readString());//whitespace(zeilenumbruch) abschneiden
  println(message); //ausgeben in console 

}

  
  void keyPressed () {
    println("CheckPressed1");

    if (key == 'a') {
      println("OK");
      if (sensorVal <= 127) {     //if value is lower than 1/2 of max sensor value 255
        
        for (int i=0; i<5; i++) {
          TableRow row = table.getRow(i);              //store row i
          int x;  // initialise values
          int y;
          int t;
          println(row.getInt("YEAR"));

          if ((row.getInt("YEAR") >= 1940) && (row.getInt("YEAR") <= 1951)) {     //test rows example
            t = row.getInt("YEAR");
            x = row.getInt("LAT"); 
            y = row.getInt("LONG"); 
            println("year" +":" + t);
            println("X latitutde" + ":" + x); 
            println("Y longitude" + ":" + y);
            myPort.write("x" + str(x));
            delay (200);
          }
        }
    }
  }
}
