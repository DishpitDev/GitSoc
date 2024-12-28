package com.gitsoc.web;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.Scanner;

import com.sun.net.httpserver.HttpExchange;
import com.sun.net.httpserver.HttpHandler;

public class UrlHandler implements HttpHandler {
    @Override
    public void handle(HttpExchange exchange) throws IOException {
    	
    	int returncode = 200;
        String response = "";
        String uri = exchange.getRequestURI().toString();
        
        // Log the interaction.
        System.out.println("Recieved request for \"" + uri + "\" by " + exchange.getRemoteAddress() + " "
        		+ exchange.getProtocol() + " User Agent: \"" + exchange.getRequestHeaders().getFirst("User-Agent")
        		+ "\" Request method " + exchange.getRequestMethod());
        
        if (!"GET".equals(exchange.getRequestMethod())) {
            // Return 405
            exchange.sendResponseHeaders(405, response.length());
            OutputStream os = exchange.getResponseBody();
            os.write(response.getBytes());
            os.close();
            
            return;
        }
        
        // If the URI is root or empty, set it to index.html.
        if ("/".equals(uri)||uri.isBlank())
        	uri = "/index.html";
        
        
        /*
         * I've decided that (at this point) it'll be better to bake the HTML files into the server jar rather than
         * having it be retrieved from elsewhere, if neccessary it can always be changed but rn it is better to do
         * it this way.
         */
        InputStream resource = UrlHandler.class.getResourceAsStream("/site/"+uri);

        if (resource==null)
            // Couldn't find resource, try adding ".html" to the end.
        	resource = UrlHandler.class.getResourceAsStream("/site/"+uri+".html");
        
        if (resource==null) {
            // Still couldn't find resource, load 404 instead.
        	resource = UrlHandler.class.getResourceAsStream("/site/404.html");
        	returncode = 404;
        }
    	
        // Reading the resource
        
        Scanner scanner = new Scanner(resource); // Create the scanner
        
        while (scanner.hasNextLine()) { // Is there any text to read?
        	// Read the next line then add a newline.
        	response+=scanner.nextLine()+"\n";
        }
        
        // Close the scanner
        scanner.close();
        
        // Send response and returncode.
        exchange.sendResponseHeaders(returncode, response.length());
        OutputStream os = exchange.getResponseBody();
        os.write(response.getBytes());
        os.close();
    }
}
