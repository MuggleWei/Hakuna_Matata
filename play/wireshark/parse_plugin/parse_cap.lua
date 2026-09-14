-- 定义协议
local myproto = Proto("myproto", "My Custom Protocol")

-- 定义协议字段
local f_magic = ProtoField.uint32("myproto.magic", "magic number", base.HEX)
local f_flags = ProtoField.uint32("myproto.flags", "flags", base.HEX)
local f_msg_id = ProtoField.uint32("myproto.msg_id", "message id", base.DEC)
local f_payload_len = ProtoField.uint32("myproto.payload_len", "payload length", base.DEC)
local f_payload = ProtoField.bytes("myproto.payload", "payload")
local f_tail = ProtoField.uint64("myproto.tail", "tail", base.DEC)

myproto.fields = { f_magic, f_flags, f_msg_id, f_payload_len, f_payload, f_tail }

local HEADER_LEN = 16
local TAIL_LEN = 8

-- --------------------------------------------------------------
-- 核心 dissector（完全手动处理 TCP 粘包/拆包）
-- --------------------------------------------------------------
function myproto.dissector(tvb, pinfo, tree)
	local len = tvb:len()
	
	 -- 检查是否收齐了头部（8字节）
    if len < HEADER_LEN then
        pinfo.desegment_len = HEADER_LEN - len  -- 告诉 Wireshark 还需要多少字节
        return 0  -- 0 表示当前还没消耗任何数据
    end

    -- 设置协议列
    pinfo.cols.protocol = myproto.name

    -- 创建根节点
    local subtree = tree:add(myproto, tvb:range(0))

    -- 解析 magic
	local magic = tvb(0, 4):uint()
    subtree:add(f_magic, magic)
	
	-- 解析 flags
	local flags = tvb(4, 4):uint()
	subtree:add(f_flags, tvb(4, 4))
	
	-- 解析 msg_id
	local msg_id = tvb(8, 4):le_uint()
	subtree:add(f_msg_id, msg_id)

    -- 解析 payload_len
    local n = tvb(12, 4):le_uint()
    subtree:add(f_payload_len, n)
	
	-- 检查是否收齐了整个包
	local total_len = HEADER_LEN + n + TAIL_LEN
    if len < total_len then
        pinfo.desegment_len = total_len - len  -- 继续等待后续数据
        return 0
    end

    -- 解析 payload
    if n > 0 then
        subtree:add(f_payload, tvb(HEADER_LEN, n))
    end

    -- 解析 tail
    local tail_offset = HEADER_LEN + n
    subtree:add(f_tail, tvb(tail_offset, TAIL_LEN))

    -- 更新 Info 列方便查看
	local old_info = tostring(pinfo.cols.info) or ""
    -- pinfo.cols.info:set(old_info .. ", Magic=" .. tvb(0, 4):string() .. ", MsgId=" .. msg_id .. ", PayloadLen=" .. n )
	pinfo.cols.info:set(old_info .. ", MsgId=" .. msg_id .. ", PayloadLen=" .. n )
	
	return total_len
end

-- --------------------------------------------------------------
-- 4. 注册到 TCP 端口
-- --------------------------------------------------------------
local tcp_port = DissectorTable.get("tcp.port")
tcp_port:add(10102, myproto)
