package com.muggle.service;

import com.muggle.mapper.AccountMapper;
import com.muggle.model.Account;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.cache.annotation.CachePut;
import org.springframework.cache.annotation.Cacheable;
import org.springframework.cache.annotation.EnableCaching;
import org.springframework.data.redis.core.ValueOperations;
import org.springframework.stereotype.Service;
import org.springframework.transaction.annotation.Transactional;

import javax.annotation.Resource;
import java.util.List;

@Service
@EnableCaching
public class AccountService {

    @Autowired
    private AccountMapper accountMapper;

    @Resource(name = "redisTemplate")
    private ValueOperations<String, Account> accountOps;

    public int getAccountInfoCnt() {
        return accountMapper.getAccountInfoCnt();
    }

    public int getAccountPasswdCnt() {
        return accountMapper.getAccountPasswdCnt();
    }

    @Transactional
    @CachePut(cacheNames = "account:id", key = "#account.id")
    public Account CreateAccount(Account account) throws Exception {
        int ret = accountMapper.createAccountInfo(account);
        if (ret <= 0) {
            return null;
        }

        ret = accountMapper.createAccountPasswd(account);
        if (ret <= 0) {
            return null;
        }

        return account;
    }

    public List<Account> selectAll() {
        return accountMapper.selectAll();
    }

    public Account getAccountByIdFromDB(long id) {
        List<Account> accountList = accountMapper.selectByID(id);
        if (accountList.size() == 1) {
//            System.out.println("Success get account by id from db: " + id);
            return accountList.get(0);
        } else if (accountList.size() <= 0) {
            System.out.println("Failed get account by id: " + id);
            return null;
        } else {
            System.out.println("Multiple math account by id: " + id);
            return null;
        }
    }

    @Cacheable(cacheNames = "account:id", key = "#id")
    public Account getAccountByIdFromCacheAuto(long id) {
        return getAccountByIdFromDB(id);
    }

    public Account getAccountByIdFromCacheManual(long id) {
        Account account = accountOps.get("account:id::" + Long.toString(id));
        if (account == null) {
            account = getAccountByIdFromDB(id);
            if (account != null) {
                accountOps.set("account:id::" + Long.toString(id), account);
            }
        }
        return account;
    }
}
