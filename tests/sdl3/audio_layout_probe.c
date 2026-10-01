#include <stddef.h>
#include "stunts_audio_views.h"

/* Backing records in seg027 and the decoded views in seg028 share storage. */
_Static_assert(sizeof(struct AUDIOCHUNK) == 76, "seg027 chunk stride");
_Static_assert(sizeof(struct AudioChunk) == 76, "seg028 chunk stride");
_Static_assert(offsetof(struct AUDIOCHUNK, unk05) == 5, "first stack pointer");
_Static_assert(offsetof(struct AudioChunk, stack) == 5, "decoded stack pointer");
_Static_assert(offsetof(struct AUDIOCHUNK, unk1E) == 30, "sample pointer");
_Static_assert(offsetof(struct AudioChunk, data) == 30, "decoded sample pointer");
_Static_assert(offsetof(struct AUDIOCHUNK, unk2E) == 46, "sample table");
_Static_assert(offsetof(struct AudioChunk, samples) == 46, "decoded sample table");
_Static_assert(offsetof(struct AUDIOCHUNK, unk48) == 72, "callback pointer");
_Static_assert(offsetof(struct AudioChunk, callback) == 72, "decoded callback");

/* A DOS near pointer grows to four bytes in the flat i686 host record. */
_Static_assert(sizeof(struct AUDIOVOICE) == 48, "seg027 voice stride");
_Static_assert(sizeof(struct AudioVoice) == 48, "seg028 voice stride");
_Static_assert(offsetof(struct AUDIOVOICE, unk10) == 16, "voice sample pointer");
_Static_assert(offsetof(struct AudioVoice, data) == 16, "decoded voice sample");
_Static_assert(offsetof(struct AUDIOVOICE, unk2A) == 42, "voice resource pointer");
_Static_assert(offsetof(struct AudioVoice, resource) == 42, "decoded resource");
_Static_assert(offsetof(struct AUDIOVOICE, unk2C) == 46, "voice channel");
_Static_assert(offsetof(struct AudioVoice, channelNumber) == 46, "decoded channel");

/* seg007 is private, but its host view must keep 16-bit counters. */
_Static_assert(sizeof(struct AudioPayload) == 48, "audio payload size");
_Static_assert(sizeof(struct AudioTimer) == 76, "audio timer size");
_Static_assert(offsetof(struct AudioTimer, handle) == 2, "audio timer handle");
_Static_assert(offsetof(struct AudioTimer, channels) == 16, "audio timer channels");
_Static_assert(offsetof(struct AudioTimer, payload) == 28, "audio timer payload");
