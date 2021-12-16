#pragma once

#define HANG(x) while (x) delay(10uL)
#define STATE_WEIGHT(parent, state) ((parent == state) * 0.25f)
#define GET_ROT_WEIGHT(rot, ths, off) ((rot >= ths) * ((rot - ths) / off))
#define AVG(x, y) ((x + y) / 2.0f)