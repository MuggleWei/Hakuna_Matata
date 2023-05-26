#ifndef USER_MAPPER_H_
#define USER_MAPPER_H_

#include "dao_user.h"
#include <stdbool.h>

bool user_mapper_insert(MYSQL *con, struct dao_user *users, size_t rowcnt);

bool user_mapper_query_by_name(MYSQL *con, struct dao_user *user,
							   struct dao_user **users, size_t *rowcnt);

#endif // !USER_MAPPER_H_
