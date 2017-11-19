package com.muggle;

import com.muggle.config.CacheConfig;
import com.muggle.config.DBConfig;
import com.muggle.config.MapperConfig;
import com.muggle.config.ServiceConfig;
import com.muggle.model.Account;
import com.muggle.model.TradeRecord;
import com.muggle.service.AccountService;
import com.muggle.service.TradeService;
import org.junit.Assert;
import org.junit.Before;
import org.junit.Test;
import org.springframework.context.ApplicationContext;
import org.springframework.context.annotation.AnnotationConfigApplicationContext;

import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.Random;

public class AccountServiceTest {

    private long accountCnt = 1000;
    private long tradeCnt = 2000;
    private int increIdx = 1;
    private String[] productList = new String[]{ "Google", "Facebook", "Microsoft", "Apple", "Tencent", "Alibaba" };
    private double[] priceList = new double[] { 55.0, 40.0, 51.0, 29.0, 50.2, 50.1 };

    private TradeRecord generateTradeRecord() {

        // generate time
        long time = System.currentTimeMillis();
        Random generator = new Random();
        long dayGoBack = generator.nextInt(365 * 5) + 1;
        long secondGoBack = generator.nextInt(24*60*60);
        time = time - dayGoBack * 24 * 60 * 60 * 1000 - secondGoBack * 1000;
        Date now = new Date(time);

        // generate trade id
        SimpleDateFormat dateFormat = new SimpleDateFormat("yyyyMMdd_HHmmssSSS");
        String id = dateFormat.format(now) + "_" + increIdx + "_serverXXX";
        increIdx++;
        if (increIdx > 100) {
            increIdx = 1;
        }

        // get account id
        long accountId = generator.nextInt((int)accountCnt) + 1L;

        // get product id
        int productIdx = generator.nextInt(productList.length);
        String product = productList[productIdx];
        double price = priceList[productIdx] + generator.nextInt(5) * 2 - 5.0;

        TradeRecord record = new TradeRecord();
        record.setId(id);
        record.setAccount_id(accountId);
        record.setProduct_id(product);
        record.setPrice(price);
        record.setTrade_date(now);
        record.setTrade_time(time);

        return record;
    }

    @Before
    public void InitAccountAndTrade() {
        ApplicationContext ctx = new AnnotationConfigApplicationContext(
                MapperConfig.class, ServiceConfig.class, DBConfig.class, CacheConfig.class
        );
        AccountService accountService = ctx.getBean(AccountService.class);
        TradeService tradeService = ctx.getBean(TradeService.class);
        Assert.assertNotNull(accountService);
        Assert.assertNotNull(tradeService);

        Account lastAccount = accountService.getAccountByIdFromDB(accountCnt);
        if (lastAccount != null) {
            return;
        }

        // create accounts
        long start = System.currentTimeMillis();
        for (int i = 1; i <= (int)accountCnt; ++i) {
            Account account = new Account();
            account.setName("user" + Integer.toString(i));
            account.setMail(Integer.toString(i) + "@user.com");
            account.setPhone("110" + String.format("%05d", i));
            account.setPassword("xxxxxx");
            try {
                accountService.CreateAccount(account);
            } catch (Exception e) {
                System.out.println("Failed create user: " + account.getId());
                e.printStackTrace();
            }
        }
        long end = System.currentTimeMillis();
        long elapsed = end - start;
        System.out.println("create " + accountCnt + " accounts and cache use time: " + elapsed);

        // create trade records
        increIdx = 1;
        start = System.currentTimeMillis();
        for (int i = 0; i < tradeCnt; ++i) {
            TradeRecord record = generateTradeRecord();
            try {
                tradeService.NewTradeRecord(record);
            } catch (Exception e) {
                e.printStackTrace();
            }
        }
        end = System.currentTimeMillis();
        elapsed = end - start;
        System.out.println("create " + tradeCnt + " trade records and cache use time: " + elapsed);
    }

    @Test
    public void accountQueryTest() {
        ApplicationContext ctx = new AnnotationConfigApplicationContext(
                MapperConfig.class, ServiceConfig.class, DBConfig.class, CacheConfig.class
        );
        AccountService service = ctx.getBean(AccountService.class);
        Assert.assertNotNull(service);

        long start;
        long end;
        long elapsed;

        start = System.currentTimeMillis();
        for (int i = 1; i <= (int)accountCnt; ++i) {
            Account account = service.getAccountByIdFromCacheManual(i);
        }
        end = System.currentTimeMillis();
        elapsed = end - start;
        System.out.println("get account by id from cache manually: " + elapsed);

        start = System.currentTimeMillis();
        for (int i = 1; i <= (int)accountCnt; ++i) {
            Account account = service.getAccountByIdFromCacheAuto(i);
        }
        end = System.currentTimeMillis();
        elapsed = end - start;
        System.out.println("get account by id from cache automatic: " + elapsed);

        start = System.currentTimeMillis();
        for (int i = 1; i <= (int)accountCnt; ++i) {
            Account account = service.getAccountByIdFromDB(i);
        }
        end = System.currentTimeMillis();
        elapsed = end - start;
        System.out.println("get account by id from db: " + elapsed);
    }

    @Test
    public void tradeRecordQueryTest() {
        ApplicationContext ctx = new AnnotationConfigApplicationContext(
                MapperConfig.class, ServiceConfig.class, DBConfig.class, CacheConfig.class
        );
        TradeService service = ctx.getBean(TradeService.class);
        Assert.assertNotNull(service);

        long start;
        long end;
        long elapsed;

        start = System.currentTimeMillis();
        for (int i = 1; i <= (int)accountCnt; ++i) {
            service.getRecordByAccountIdFromDB(i);
        }
        end = System.currentTimeMillis();
        elapsed = end - start;
        System.out.println("get trade records by account id from db: " + elapsed);

        start = System.currentTimeMillis();
        for (int i = 1; i <= (int)accountCnt; ++i) {
            service.getRecordByAccountIdFromCacheManual(i);
        }
        end = System.currentTimeMillis();
        elapsed = end - start;
        System.out.println("get trade records by account id from cache: " + elapsed);
    }

    @Test
    public void ConcurrencyCreateTradeRecordTest() {
        ApplicationContext ctx = new AnnotationConfigApplicationContext(
                MapperConfig.class, ServiceConfig.class, DBConfig.class, CacheConfig.class
        );
        TradeService service = ctx.getBean(TradeService.class);
        Assert.assertNotNull(service);

        Runnable r = () -> {
            long threadStart = System.currentTimeMillis();
            for (int i = 0; i < 15000; ++i) {
                TradeRecord record = generateTradeRecord();
                try {
                    service.NewTradeRecord(record);
                } catch (Exception e) {
                    e.printStackTrace();
                }
            }
            long threadEnd = System.currentTimeMillis();
            long threadElapsed = threadEnd - threadStart;
            System.out.println("Thread use time: " + threadElapsed);
        };

        long start = System.currentTimeMillis();

        int size = 8;
        Thread[] threads = new Thread[size];
        for (int i = 0; i < size; ++i) {
            threads[i] = new Thread(r);
            threads[i].start();
        }
        for (Thread thread : threads) {
            try {
                thread.join();
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
        }

        long end = System.currentTimeMillis();
        long elapsed = end - start;
        System.out.println("total use time: " + elapsed + "ms");
    }
}
