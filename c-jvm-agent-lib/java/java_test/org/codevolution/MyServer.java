package org.codevolution;

import java.io.IOException;
import java.io.OutputStream;
import java.net.InetSocketAddress;

import com.sun.net.httpserver.HttpExchange;
import com.sun.net.httpserver.HttpHandler;
import com.sun.net.httpserver.HttpServer;
import com.sun.net.httpserver.HttpExchange;
import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.UnsupportedEncodingException;
import java.net.URI;
import java.net.URLDecoder;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import com.sun.net.httpserver.HttpContext;
import java.io.StringWriter;
import java.io.PrintWriter;
import java.net.BindException;

public class MyServer {
  static HttpServer server = null;
  static String base_url = null;
  static String error_message = "";
  static int port = 8090;

  class DBException extends Exception{
    public DBException(String message) {
      super(message);
    }
  }

  HttpServer openServer() throws Exception{
    try {
      return HttpServer.create(new InetSocketAddress("127.0.0.1", port), 0);
    } catch (BindException ex) {
      port++;
      return openServer();
    }
  }
  public MyServer() {
    
    try {
      server = openServer();
      base_url = "http://127.0.0.1:" + port + "/";
      server.createContext("/", new MyHomeHandler());
      server.createContext("/dash", new MyDashHandler());
      server.createContext("/transaction", new MyTransactionHandler());

      server.setExecutor(null); // creates a default executor
      server.start();

      Logger.info("Starting HTTP Server [ " + server.getAddress() + " ]");

    } catch (Exception ex) { ex.printStackTrace(); }
  }

  static void sendResponse(HttpExchange t, String response) throws IOException {
    if (error_message != "") {
      response = "<pre>" + error_message + "</pre>";
      error_message = "";
    }
    response = "<title>Project 1 - Prototype</title>" +
    "<link rel='stylesheet' href='https://maxcdn.bootstrapcdn.com/bootstrap/3.3.6/css/bootstrap.min.css'>" +
    "<link rel='stylesheet' href='https://bootswatch.com/paper/bootstrap.min.css'>" + 
    "<div class='container'><h3 class='text-center'>Demo app</h3><hr/><div class='text-center'><div class='btn-group' role='group' aria-label='...'>" +
    "<a class='btn btn-default' href='" + base_url + "'>Home</a>" +
    "<a class='btn btn-default' href='" + base_url + "dash'>Dash</a>" +
    "</div></div><br/><br/>" + response + "<br/><br/><hr/><div class='text-center'><small>Demo app</small></div></div>"; 
    t.sendResponseHeaders(200, response.length());
    OutputStream os = t.getResponseBody();
    os.write(response.getBytes());
    os.close();
  }

  static Map<String, List<String>> postHandler(HttpExchange t) {
    try {
      if ("post".equalsIgnoreCase(t.getRequestMethod())) {
        Map<String, List<String>> parameters = new HashMap<String, List<String>>();
        InputStreamReader isr =
          new InputStreamReader(t.getRequestBody(), "utf-8");
        BufferedReader br = new BufferedReader(isr);
        String query = br.readLine();
        if (query != null) {
          String pairs[] = query.split("[&]");

          for (String pair : pairs) {
            String param[] = pair.split("[=]");

            String key = key = URLDecoder.decode(param[0],
                System.getProperty("file.encoding"));
            String value = value = URLDecoder.decode(param[1],
                System.getProperty("file.encoding"));

            List <String> values;
            if (parameters.containsKey(key)) {
              values = parameters.get(key);
              values.add(value);
            } else {
              values = new ArrayList<String>();
              values.add(value);
            }
            parameters.put(key, values);
          }
        }
        return parameters;
      }
    } catch(Exception ex) { ex.printStackTrace(); }
    return null;
  }

  static void handleException(Exception ex) {
    StringWriter sw = new StringWriter();
    PrintWriter pw = new PrintWriter(sw);
    ex.printStackTrace(pw);
    error_message += sw.toString();

    ex.printStackTrace();
  }

  class MyTransactionHandler implements HttpHandler {
    @Override
    public void handle(HttpExchange t) throws IOException {
      Map<String, List<String>> post = postHandler(t);
      if (post != null) {
        try {
          String from = post.get("from").get(0);
          String to = post.get("to").get(0);
          Double amount = Double.parseDouble(post.get("amount").get(0));
          boolean isPremium = true;
          int accountId = 345345;
          
          throw new Exception("Transaction failure due to null DB connection.");
        } catch (Exception ex) {
          MyServer.handleException(ex);
        }
      }
    
      MyServer.sendResponse(t, "<div class='alert alert-success'>Transaction has been completed.</div>");
    }
  }

  class MyHomeHandler implements HttpHandler {
    @Override
    public void handle(HttpExchange t) throws IOException {
      MyServer.sendResponse(t, "<div class='well'>Welcome to our bank's transaction manager.</div>");
    }
  }

 

  class MyDashHandler implements HttpHandler {
    @Override
    public void handle(HttpExchange t) throws IOException {
      MyServer.sendResponse(t,  "<div class='panel panel-default'><div class='panel-heading'>New account transaction</div><div class='panel-body'>" + 
        "<form method='post' action='" + base_url + "transaction'>" + 
        "<input type='text' name='from' placeholder='From' value='Adrian' readonly class='form-control'/><br/>" +
        "<input type='text' name='to' placeholder='To' value='Monica' class='form-control'/><br/>" +
        "<input type='text' name='amount' placeholder='Amount' value='5' class='form-control'/><br/>" +
        "<button type='submit' class='btn btn-large btn-primary btn-block'>Send</button></form></div></div>");
    }
  }
}
