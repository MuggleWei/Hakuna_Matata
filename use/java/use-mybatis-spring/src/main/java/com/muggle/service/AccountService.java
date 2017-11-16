package com.muggle.service;

import com.muggle.mapper.AccountMapper;
import com.muggle.model.Account;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;
import org.springframework.transaction.annotation.Transactional;

import java.util.List;

@Service
public class AccountService {

    @Autowired
    private AccountMapper accountMapper;

    public List<Account> selectAll() {
        return accountMapper.selectAll();
    }

    @Transactional
    public boolean CreateAccount(Account account) throws Exception {
        int ret = accountMapper.createAccountInfo(account);
        if (ret <= 0) {
            return false;
        }

        ret = accountMapper.createAccountPasswd(account);
        if (ret <= 0) {
            return false;
        }

        return true;
    }

    @Transactional
    public boolean BadCreateAccount(Account account) throws Exception {
        int ret = accountMapper.createAccountInfo(account);
        if (ret <= 0) {
            return false;
        }

        account.setId(10000L);

        ret = accountMapper.createAccountPasswd(account);
        if (ret <= 0) {
            return false;
        }

        return true;
    }

    public int getAccountInfoCnt() {
        return accountMapper.getAccountInfoCnt();
    }

    public int getAccountPasswdCnt() {
        return accountMapper.getAccountPasswdCnt();
    }
}
