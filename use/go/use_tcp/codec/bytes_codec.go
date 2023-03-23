package codec

import (
	"bufio"
	"bytes"
	"encoding/binary"
	"errors"
	"io"
)

const MSG_FLAGS_ENDIAN = 0
const MSG_FLAGS_VER = 1
const MSG_FLAG_COMPRESSED = 2
const MSG_FLAG_REVERSED1 = 3

const sMagicByte0 = byte('D')
const sMagicByte1 = byte('E')
const sMagicByte2 = byte('M')
const sMagicByte3 = byte('O')

const sFlagsEndian = byte(0)
const sFlagsVer = byte(1)
const sFlagsCompressed = byte(0)
const sFlagsReversed = byte('\x00')

const sHdrSize = 16

type MsgHdr struct {
	Magic      [4]byte // magic word
	Flags      [4]byte // flags
	MsgId      uint32  // message id
	PayloadLen uint32  // payload length(not include header)
}

type BytesCodec struct {
	MaxPayloadLimit uint32 // max message payload
}

func (this *BytesCodec) Encode(msgId uint32, payload []byte) ([]byte, error) {
	hdr := MsgHdr{
		Magic:      [4]byte{sMagicByte0, sMagicByte1, sMagicByte2, sMagicByte3},
		Flags:      [4]byte{sFlagsEndian, sFlagsVer, sFlagsCompressed, sFlagsReversed},
		MsgId:      msgId,
		PayloadLen: uint32(len(payload)),
	}

	packet := bytes.NewBuffer(make([]byte, 0, len(payload)+sHdrSize))

	err := binary.Write(packet, binary.LittleEndian, hdr)
	if err != nil {
		return nil, err
	}

	err = binary.Write(packet, binary.LittleEndian, payload)
	if err != nil {
		return nil, err
	}

	return packet.Bytes(), nil
}

func (this *BytesCodec) Decode(reader *bufio.Reader) (*MsgHdr, []byte, error) {
	// fetch message head bytes
	hdrBytes, err := reader.Peek(sHdrSize)
	if err != nil {
		return nil, nil, err
	}

	// convert bytes to message head
	hdr := MsgHdr{}
	err = binary.Read(bytes.NewBuffer(hdrBytes), binary.LittleEndian, &hdr)
	if err != nil {
		return nil, nil, err
	}

	// check magic word
	if hdr.Magic[0] != sMagicByte0 ||
		hdr.Magic[1] != sMagicByte1 ||
		hdr.Magic[2] != sMagicByte2 ||
		hdr.Magic[3] != sMagicByte3 {
		return nil, nil, errors.New("invalid magic word")
	}

	// check flags
	if hdr.Flags[MSG_FLAGS_ENDIAN] != sFlagsEndian ||
		hdr.Flags[MSG_FLAGS_VER] != sFlagsVer ||
		hdr.Flags[MSG_FLAG_COMPRESSED] != sFlagsCompressed ||
		hdr.Flags[MSG_FLAG_REVERSED1] != sFlagsReversed {
		return nil, nil, errors.New("invalid flags")
	}

	// check message length
	if this.MaxPayloadLimit > 0 && hdr.PayloadLen > this.MaxPayloadLimit {
		return nil, nil, errors.New("payload length beyond the limit")
	}

	// skip head
	reader.Discard(sHdrSize)

	// if readable < expect, need loop to read
	payload := make([]byte, hdr.PayloadLen)
	idx := uint32(0)
	for {
		// read data
		n, err := reader.Read(payload[idx:])
		if err != nil {
			if err == io.EOF {
				return nil, nil, err
			}
		}
		idx += uint32(n)

		if idx == hdr.PayloadLen {
			break
		} else if idx > hdr.PayloadLen {
			return nil, nil, errors.New("exception! read beyond expect")
		}
	}

	return &hdr, payload, nil
}
