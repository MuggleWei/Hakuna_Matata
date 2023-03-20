#include "codec.h"

NS_MUGGLE_DEMO_BEGIN

Codec::Codec()
	: prev_(nullptr)
	, next_(nullptr)
{}
Codec::~Codec()
{}

void Codec::append(Codec *codec)
{
	next_ = codec;
	codec->prev_ = this;
}

NS_MUGGLE_DEMO_END
