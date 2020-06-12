package com.muggle.use.mybatis.mapper;

import com.muggle.use.mybatis.model.Fund;
import org.apache.ibatis.annotations.Param;

import java.util.List;

public interface FundMapper {

    List<Fund> queryFunds(
            @Param("userId") Long userId,
            @Param("currencyType") Integer currencyType);

    int insertFund(Fund fund);

}
