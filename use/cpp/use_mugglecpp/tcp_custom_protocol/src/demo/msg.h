#ifndef MUGGLE_DEMO_MSG_H_
#define MUGGLE_DEMO_MSG_H_

#include <stdint.h>
#include "demo/macro.h"

; // for avoid vim complain
#pragma pack(push)
#pragma pack(1)

NS_MUGGLE_DEMO_BEGIN

#define MUGGLE_DEMO_MAGIC_WORD "DEMO"
#define MUGGLE_DEMO_FLAG_ENDIAN 0
#define MUGGLE_DEMO_FLAG_VER 1
#define MUGGLE_DEMO_FLAG_COMPRESSED 2
#define MUGGLE_DEMO_FLAG_REVERSED1 3

typedef struct msg_header
{
	char     magic[4];    //!< magic word
	char     flags[4];    //!< flags
	uint32_t msg_id;      //!< message id
	uint32_t payload_len; //!< payload length(not include header)
} msg_hdr_t;

#define NEW_STACK_DEMO_MSG(demo_msg_id, msg_type, var) \
	char msg_placeholder_##var[sizeof(msg_hdr_t) + sizeof(msg_type)]; \
	memset(msg_placeholder_##var, 0, sizeof(msg_placeholder_##var)); \
	msg_hdr_t *hdr_##var = (msg_hdr_t*)msg_placeholder_##var; \
	hdr_##var->msg_id = demo_msg_id; \
	msg_type *var = (msg_type*)(hdr_##var + 1);

#define DEMO_MSG_SIZE(var) (sizeof(msg_hdr_t) + sizeof(var))

#define DEMO_SESSION_SEND_MSG(session, var) \
	session->sendMessage(hdr_##var, sizeof(msg_hdr_t) + sizeof(*var));

NS_MUGGLE_DEMO_END

#pragma pack(pop)

#endif // !MUGGLE_DEMO_MSG_H_
