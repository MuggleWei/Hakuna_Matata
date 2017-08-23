package com.muggle.app;

/**
 * Hello world!
 *
 */
public class App 
{
    public static void main( String[] args )
    {
        System.out.println( "use log4j 2.8.2" );
        System.out.println("if System.setProperty(\"log4j.configurationFile\", \"config/log4j2.xml\"); not invoke, " +
                "rememeber add -Dlog4j.configurationFile=config/log4j2.xml");
    }
}
