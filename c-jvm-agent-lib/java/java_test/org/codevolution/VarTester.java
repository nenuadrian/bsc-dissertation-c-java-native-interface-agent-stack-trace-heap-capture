package org.codevolution;
import java.util.Timer;
import java.util.TimerTask;

public class VarTester {
  static int staticIntVariable = 4;
  static String staticStringVariable = "staasdfasdf";

  int testInstanceVariableInTester = 2;
  class CustomException extends Exception{
    public CustomException(String message) {
      super(message);
    }
  }

  public static < E > void testGenericPrintArray( E[] inputArray ) {
    try {
      for ( E element : inputArray ){ }
      throw new Exception ("testGenericPrintArray");
    } catch(Exception ex) { ex.printStackTrace(); }
  }


public VarTester() {
  int localIntVariable = 44234;
  boolean localBooleanVariable = true;
  boolean localBooleanVariableFalse = false;
  char localCharVariable = 'A';
  short localShortVariable = 423;
  long localLongVariable = 422223;
  float localFloatVariable = 423.3f;
  double localDoubleVariable = 423.4;
  
  try {
    int testVariableInTesterTryCatch = 5;
    Integer [] testArray = new Integer[100];
    testArray[0] = 100;
    //testGenericPrintArray(testArray);


    throw new Exception ("Manually thrown var tester caught exception");
  } catch(Exception ex) {
    ex.printStackTrace();
  }

    // long running
  Timer timer = new Timer();
  timer.schedule(new TimerTask() {
    @Override
    public void run() {
      try {
        int testExceptionVar = 5;
        if (false) 
          throw new CustomException ("Manually thrown timer based custom exception");
      } catch(CustomException ex) {
        ex.printStackTrace();
      }
    }
  }, 10 * 1000);

    // fatal
  timer.schedule(new TimerTask() {
    @Override
    public void run() {
      long zeroMustEqualOneHundredPercent = 0;
      long helloKitty = 5 / zeroMustEqualOneHundredPercent;
    }
  }, 10 * 60 * 1000);
}
}
