package com.muggle.config;


import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.context.annotation.Bean;
import org.springframework.context.annotation.Configuration;
import org.springframework.context.annotation.PropertySource;
import org.springframework.core.env.Environment;
import org.springframework.data.redis.cache.RedisCacheConfiguration;
import org.springframework.data.redis.cache.RedisCacheManager;
import org.springframework.data.redis.connection.jedis.JedisConnectionFactory;
import org.springframework.data.redis.core.RedisTemplate;
import org.springframework.data.redis.serializer.GenericJackson2JsonRedisSerializer;
import org.springframework.data.redis.serializer.RedisSerializationContext;
import org.springframework.data.redis.serializer.StringRedisSerializer;
import redis.clients.jedis.JedisPoolConfig;

@Configuration
@PropertySource(
        value = {"file:config/redis.properties"},
        ignoreResourceNotFound = true)
public class CacheConfig {

    @Autowired
    Environment env;

    @Bean(name = "jedisPoolConfig")
    JedisPoolConfig jedisPoolConfig() {
        JedisPoolConfig jedisPoolConfig = new JedisPoolConfig();
        jedisPoolConfig.setMaxIdle(Integer.parseInt(env.getProperty("redis.maxIdle")));
        jedisPoolConfig.setTestOnBorrow(Boolean.parseBoolean(env.getProperty("redis.testOnBorrow")));
        return jedisPoolConfig;
    }

    @Bean(name = "jedisConnectionFactory")
    JedisConnectionFactory jedisConnectionFactory() {
        JedisConnectionFactory jedisConnectionFactory = new JedisConnectionFactory(jedisPoolConfig());
        jedisConnectionFactory.setUsePool(true);
        jedisConnectionFactory.setHostName(env.getProperty("redis.host"));
        jedisConnectionFactory.setPort(Integer.parseInt(env.getProperty("redis.port")));
        jedisConnectionFactory.setTimeout(Integer.parseInt(env.getProperty("redis.timeout")));
        jedisConnectionFactory.setDatabase(Integer.parseInt(env.getProperty("redis.default.db")));
        return jedisConnectionFactory;
    }

    @Bean(name = "redisTemplate")
    RedisTemplate redisTemplate() {
        RedisTemplate redisTemplate = new RedisTemplate();
        redisTemplate.setConnectionFactory(jedisConnectionFactory());
        redisTemplate.setKeySerializer(new StringRedisSerializer());
        redisTemplate.setHashKeySerializer(new StringRedisSerializer());
        redisTemplate.setValueSerializer(new GenericJackson2JsonRedisSerializer());
        redisTemplate.setHashValueSerializer(new GenericJackson2JsonRedisSerializer());
        return redisTemplate;
    }

    @Bean(name = "redisCacheManager")
    RedisCacheManager redisCacheManager() {
        RedisCacheConfiguration config = RedisCacheConfiguration.defaultCacheConfig()
                .disableCachingNullValues()
                .serializeKeysWith(RedisSerializationContext.SerializationPair.fromSerializer(new StringRedisSerializer()))
                .serializeValuesWith(RedisSerializationContext.SerializationPair.fromSerializer(new GenericJackson2JsonRedisSerializer()));

        return RedisCacheManager.builder(jedisConnectionFactory())
                .cacheDefaults(config)
                .transactionAware()
                .build();
    }
}
