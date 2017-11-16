package com.muggle;

import com.muggle.config.DBConfig;
import com.muggle.config.MapperConfig;
import com.muggle.config.ServiceConfig;
import com.muggle.model.Account;
import com.muggle.service.AccountService;
import org.junit.Assert;
import org.junit.Test;
import org.springframework.context.ApplicationContext;
import org.springframework.context.annotation.AnnotationConfigApplicationContext;

import java.util.List;

public class AccountServiceTest {
    @Test
    public void selectAllTest() {
        ApplicationContext ctx = new AnnotationConfigApplicationContext(
                DBConfig.class, MapperConfig.class, ServiceConfig.class
        );
        AccountService service = ctx.getBean(AccountService.class);
        List<Account> accountList = service.selectAll();
        Assert.assertNotNull(accountList);
    }

    @Test
    public void CreateAccountTest() {
        ApplicationContext ctx = new AnnotationConfigApplicationContext(
                DBConfig.class, MapperConfig.class, ServiceConfig.class
        );
        AccountService service = ctx.getBean(AccountService.class);

        Account account = new Account();
        account.setName("xxx");
        account.setPassword("xxxxxx");
        try {
            service.CreateAccount(account);
        } catch (Exception e) {
            e.printStackTrace();
        }


    }

    @Test
    public void ConcurrencyCreateAccountTest() {
        ApplicationContext ctx = new AnnotationConfigApplicationContext(
                DBConfig.class, MapperConfig.class, ServiceConfig.class
        );
        AccountService service = ctx.getBean(AccountService.class);

        Runnable r = () -> {
            long threadStart = System.currentTimeMillis();
            for (int i = 0; i < 2000; ++i) {
                Account account = new Account();
                account.setName("xxx");
                account.setPassword("xxxxxx");

                try {
                    long start = System.currentTimeMillis();
                    service.CreateAccount(account);
                    long end = System.currentTimeMillis();
                    long elapsed = end - start;
                    System.out.println("create account: " + account.getId() + "[" + elapsed + "]");
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

        Assert.assertEquals(service.getAccountInfoCnt(), service.getAccountPasswdCnt());
    }
}
