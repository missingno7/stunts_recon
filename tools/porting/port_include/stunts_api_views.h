/* PORTING ONLY: strict-central host-probe views; evidence is mapped in
   docs/porting/port-headers.md#port_build-adapter-evidence-and-remaining-residuals. */
#ifndef STUNTS_PORT_API_VIEWS_H
#define STUNTS_PORT_API_VIEWS_H
/* These adapters are for the host probe's strict-central overlay only. */
#if defined(PORT_BUILD) && defined(STUNTS_PROBE_STRICT_CENTRAL)

#if defined(STUNTS_TU_obj_seg000)
/* Port dispatch boundary. The machine contract is three source arguments over four stack words; see declaration-evidence.json. */
extern void *stunts_port_read_file_with_retry_dispatch(I16 type, I8 *name, void *destination);
static inline void *stunts_port_read_file_with_retry_view(I16 type, I8 *name, void *destination)
{
    return stunts_port_read_file_with_retry_dispatch(type, name, destination);
}
static inline I16 stunts_port_call_read_line_view(I8 *buffer, I16 first, I16 second, I16 third, I32 final_words)
{
    /* The final I32 is low word then high word in the six-word target definition. */
    U32 words = (U32)final_words;
    return call_read_line(buffer, first, second, third, (I16)(U16)words,
                          (I16)(U16)(words >> 16));
}
static inline struct SHAPE2D *stunts_port_locate_shape_fatal_shape_view(void *resource, I8 *name)
{
    /* load_20f9d returns through a generic thunk; this cast follows obj_seg000 uses. */
    return (struct SHAPE2D *)locate_shape_fatal(resource, name);
}
#endif

#if defined(STUNTS_TU_obj_seg008)
static inline I16 stunts_port_call_read_line_view(I8 *buffer, I16 first, I16 second, I16 third, I32 final_words)
{
    /* Split the legacy caller's final I32 into the definition's last two words. */
    U32 words = (U32)final_words;
    return call_read_line(buffer, first, second, third, (I16)(U16)words,
                          (I16)(U16)(words >> 16));
}
#endif

#if defined(STUNTS_TU_obj_seg032_group)
static inline void stunts_port_timer_copy_counter_split_view(I16 low_word, I16 high_word)
{
    /* load_227c0 adds BP+6/BP+8 into DX:AX; preserve that two-word input. */
    timer_copy_counter((U32)(U16)low_word | ((U32)(U16)high_word << 16));
}
#endif

#if defined(STUNTS_TU_obj_seg028)
extern void *stunts_port_resolve_far_data_pointer(U16 offset, U16 segment);
static inline void stunts_port_audio_init_chunk_7_view(I16 first, I16 last,
                                                        U16 resource_offset, U16 resource_segment,
                                                        I16 offset, I16 volume, I16 priority)
{
    /* load_27dbc uses seven stack words; the segmented pointer is offset:segment. */
    void *resource = 0;
    if (resource_offset != 0 || resource_segment != 0)
        resource = stunts_port_resolve_far_data_pointer(resource_offset, resource_segment);
    audio_init_chunk(first, last, resource, offset, (U8)volume, (U8)priority);
}
#endif

#if defined(STUNTS_TU_obj_seg007)
/* PORT_BUILD host dispatch policy. The historical I16 result is proven and
   consumed by obj_seg007; the host implementation remains an integration hook. */
extern I16 stunts_port_send_audio_stop_event_dispatch(U16 rate, I16 chunk_index);
static inline I16 stunts_port_send_audio_stop_event_value_view(U16 rate, I16 chunk_index)
{
    return stunts_port_send_audio_stop_event_dispatch(rate, chunk_index);
}
#endif

#if defined(STUNTS_TU_obj_seg001_complete)
#define plan_memres (&plan_memres) /* PORT_BUILD view: one PLANE object; source use at obj_seg001_complete.c:3101. */
#endif
#if defined(STUNTS_TU_obj_seg004)
#define aCar0 ((I8 (*)[5])(&aCar0)) /* PORT_BUILD byte-matrix view of the 76-byte aggregate; see aggregate map. */
#endif

#endif /* PORT_BUILD */
#endif /* STUNTS_PORT_API_VIEWS_H */
