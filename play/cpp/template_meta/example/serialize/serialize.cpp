#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

enum : uint32_t {
	VAR_TYPE_CONST = 0, //!< const width type; e.g. int, float, double
	VAR_TYPE_STR, //!< string type; e.g. char *, const char*
};

typedef struct bytes_buf {
	char bytes[1024];
	uint32_t pos;
} bytes_buf_t;

template <typename T>
bool Serialize(bytes_buf_t &buf, const T &v)
{
	printf("Serialize Template T\n");

	constexpr uint32_t var_type = VAR_TYPE_CONST;
	constexpr uint32_t need = sizeof(v) + sizeof(var_type);
	uint32_t remain = sizeof(buf.bytes) - buf.pos;
	if (remain < need) {
		return false;
	}

	memcpy(buf.bytes + buf.pos, &var_type, sizeof(var_type));
	buf.pos += sizeof(var_type);
	memcpy(buf.bytes + buf.pos, &v, sizeof(v));
	buf.pos += sizeof(v);

	return true;
}

template <unsigned N>
bool Serialize(bytes_buf_t &buf, const char (&v)[N])
{
	printf("Serialize Template nontype param: const char[N]\n");

	constexpr uint32_t var_type = VAR_TYPE_STR;
	constexpr uint32_t v_size = N;
	constexpr uint32_t need = sizeof(v) + sizeof(v_size) + N;
	uint32_t remain = sizeof(buf.bytes) - buf.pos;
	if (remain < need) {
		return false;
	}

	memcpy(buf.bytes + buf.pos, &var_type, sizeof(var_type));
	buf.pos += sizeof(var_type);
	memcpy(buf.bytes + buf.pos, &v_size, sizeof(v_size));
	buf.pos += sizeof(v_size);
	memcpy(buf.bytes + buf.pos, v, N);
	buf.pos += N;

	return true;
}

template <>
bool Serialize(bytes_buf_t &buf, const char *const &v)
{
	printf("Serialize Template specialization: const char*\n");

	constexpr uint32_t var_type = VAR_TYPE_STR;
	uint32_t v_size = strlen(v) + 1;
	uint32_t need = sizeof(v) + sizeof(v_size) + v_size;
	uint32_t remain = sizeof(buf.bytes) - buf.pos;
	if (remain < need) {
		return false;
	}

	memcpy(buf.bytes + buf.pos, &var_type, sizeof(var_type));
	buf.pos += sizeof(var_type);
	memcpy(buf.bytes + buf.pos, &v_size, sizeof(v_size));
	buf.pos += sizeof(v_size);
	memcpy(buf.bytes + buf.pos, v, v_size);
	buf.pos += v_size;

	return true;
}

template <>
bool Serialize(bytes_buf_t &buf, char *const &v)
{
	printf("Serialize Template specialization: char*\n");

	constexpr uint32_t var_type = VAR_TYPE_STR;
	uint32_t v_size = strlen(v) + 1;
	uint32_t need = sizeof(v) + sizeof(v_size) + v_size;
	uint32_t remain = sizeof(buf.bytes) - buf.pos;
	if (remain < need) {
		return false;
	}

	memcpy(buf.bytes + buf.pos, &var_type, sizeof(var_type));
	buf.pos += sizeof(var_type);
	memcpy(buf.bytes + buf.pos, &v_size, sizeof(v_size));
	buf.pos += sizeof(v_size);
	memcpy(buf.bytes + buf.pos, v, v_size);
	buf.pos += v_size;

	return true;
}

template <typename T, typename... Args>
bool SerializeVars(bytes_buf_t &buf, const T &v, const Args &...rest)
{
	if (!Serialize(buf, v)) {
		return false;
	}
	if constexpr (sizeof...(rest) > 0) {
		return SerializeVars(buf, rest...);
	}
	return true;
}

int main()
{
	bytes_buf_t buf = {};

	const char *p = "foo";
	char arr[] = "bar";
	char *q = arr;
	SerializeVars(buf, 1, 2l, 3.14f, "hello", p, q);

	return 0;
}
