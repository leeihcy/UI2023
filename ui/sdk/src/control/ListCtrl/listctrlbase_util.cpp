#include "stdafx.h"

// 用于解决一棵树中，多种类型的item之间 id 可能重复的问题

#define TYPE_ENCODE_OFFSET  60   // 高四位用于item type
#define KEY_TYPE_MASK    ((1<<4)-1)
#define KEY_REALID_MASK  (((LONGLONG)1<<TYPE_ENCODE_OFFSET)-1)

// 声明
LONGLONG  encode_id(long type, LONGLONG lId);
LONGLONG  decode_id(LONGLONG lId, long* ptye);

LONGLONG  encode_id(long type, LONGLONG lId)
{
    LONGLONG lType = (LONGLONG)type;
    LONGLONG lEncodeId = (lType << TYPE_ENCODE_OFFSET) | lId;
    return lEncodeId;
}
LONGLONG  decode_id(LONGLONG lId, long* ptype)
{
    LONGLONG lMask = KEY_TYPE_MASK;
    if (ptype)
        *ptype = (lId >> TYPE_ENCODE_OFFSET) & lMask;

    lMask = KEY_REALID_MASK;
    LONGLONG lRealId = lId & lMask;
    return lRealId;
}