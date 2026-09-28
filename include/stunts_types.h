/*
 * Width and segmented-pointer names for the original MSC 5.10 sources.
 *
 * These are macros, not typedefs: expand each name to the exact original MSC
 * keyword (including int versus short and plain char versus signed char).
 * A host port may define any names below before including this header to map
 * them to its fixed-width integer and flat-pointer types. Keeping the legacy
 * compiler's primitive spellings matters because typedef substitutions can
 * reorder OMF EXTDEF/COMDEF records, as observed in Q2, Q3, and Q4.
 */
#ifndef STUNTS_TYPES_H
#define STUNTS_TYPES_H

#ifndef I8
#define I8 char
#endif
#ifndef I8S
#define I8S signed char
#endif
#ifndef U8
#define U8 unsigned char
#endif
#ifndef I16
#define I16 int
#endif
#ifndef U16
#define U16 unsigned int
#endif
#ifndef I16S
#define I16S short
#endif
#ifndef U16S
#define U16S unsigned short
#endif
#ifndef I32
#define I32 long
#endif
#ifndef U32
#define U32 unsigned long
#endif

#ifndef FAR
#define FAR far
#endif
#ifndef NEAR
#define NEAR near
#endif
#ifndef HUGE
#define HUGE huge
#endif

#endif /* STUNTS_TYPES_H */
