package com.muggle;

import org.junit.Assert;
import org.junit.Test;

import java.util.Calendar;
import java.util.GregorianCalendar;
import java.util.TimeZone;

public class TimeZoneTest {
    @Test
    public void JustTest() {
        System.out.println("all TimeZone id: ");
        for (String tz : TimeZone.getAvailableIDs()) {
            System.out.print(tz + "  ");
        }
        System.out.println("\n");

        Calendar calendar = new GregorianCalendar();
        long msSinceEpoch = calendar.getTimeInMillis();
        System.out.println("ms since epoch: " + msSinceEpoch);

        calendar.setTimeZone(TimeZone.getDefault());
        System.out.println("local hour: " + calendar.get(calendar.HOUR_OF_DAY));

        calendar.setTimeZone(TimeZone.getTimeZone("UTC"));
        System.out.println("UTC hour: " + calendar.get(calendar.HOUR_OF_DAY));

        Assert.assertTrue(true);
    }
}
