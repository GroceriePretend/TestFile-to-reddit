import processing.serial.*;
Serial myPort;
float sensorVal; 
boolean portInitialized = false; //Variable for no port is found 

Table table;

void setup() {
  size (1000, 1000);
  table = loadTable("test.csv", "header");
  println("CheckSetup1");

  if (portInitialized==false) {
    sensorVal = map((mouseX), 0, width, 0, 255); //if no port accesible, use mouse
  }
  if ( portInitialized && myPort.available() >= 0) {
    val = myPort.readStringUntil('\n');             // if you receive data 
    try {
      sensorVal = Integer.valueOf(val.trim());       //Write it to sensorVal
    }
    catch(Exception e) { 
      println("Error");
    }
    println(sensorVal);
    
  }
  println("CheckSetup2");
}



void keyPressed () {
println("CheckPressed1");
 
  if (key == 'a') {
    println("OK");
        for (int i=0; i<5; i++)
         

        if (sensorVal <= 127) { //if value is lower than 1/2 of max sensor value 255

            TableRow row = table.getRow(i);              //store row i
            int x;  // initialise values
            int y;
            int t;

            if ((row.getInt("YEAR") >= 1950)&&(row.getInt("YEAR") <= 1951)) {     //just a test for some rows
              t = row.getInt("YEAR");
              x = row.getInt("LAT"); 
              y = row.getInt("LONG"); 
              println("year" +":" + t);
              println("latitutde" + ":" + x); 
              println("longitude" + ":" + y);
            }
          }
      }
    }
