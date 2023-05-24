#ifndef DAO_USER_H_
#define DAO_USER_H_

#include "mysql.h"

struct dao_user {
	uint64_t id;
	char name[32];
	int age;
	MYSQL_TIME update_ts;
};

#endif // !DAO_USER_H_
