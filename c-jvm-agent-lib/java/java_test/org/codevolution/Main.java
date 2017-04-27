package org.codevolution;


public class Main {
  public static void main(String[] args) {
    Logger.info("Starting APP");
    			System.out.println(Main.class.getProtectionDomain().getCodeSource().getLocation());

    try {
      Thread.sleep(3000);
    } catch (Exception e0) {}
  
     VarTester doTest = new VarTester();
     
     if (true) return;
    int beforeTryCatch = 2;
    try {
      int tryCatch = 4;
      throw new Exception("test");

    } catch ( Exception e ) {}
    int afterTryCatch = 2;
   // int afterTryCatch2 = 2;
   // int afterTryCatch3 = 2;

   
    
    //Logger.info(" ^^: " + Main.class.getProtectionDomain().getCodeSource());
    
    Logger.info("Triggering var test in 4s");
    try {
      Thread.sleep(4000);
    } catch (Exception e1) {}
    VarTester nullObject1 = null;
     if (true) return;
   
    VarTester nullObject2 = null;

    Logger.info("Web server startup in 4s");
    try {
      Thread.sleep(4000);
    } catch (Exception e2) {}

    MyServer myServer = new MyServer(); 
  }
 // private static native int countInstances(Class klass);
}
