package com.gitsoc.web;

import java.io.IOException;
import java.net.InetSocketAddress;

import com.sun.net.httpserver.HttpServer;

public class Main {
	
	public static final int PORT = 3000;
	
    public static void main(String[] args) throws IOException {
    	
    	// Create the HttpServer on the port specified above.
        HttpServer server = HttpServer.create(new InetSocketAddress(PORT), 0);

        // Create a context for the root of the site
        server.createContext("/", new UrlHandler());

        // Start the server
        server.start();
        
        System.out.println("Server is running on port " + PORT);
    }
}
