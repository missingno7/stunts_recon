
struct RECTANGLE { int left, right, top, bottom; };
extern short video_flag2_is1;
extern short video_flag3_isFFFF;
extern void fatal_error(char *);
void rect_union(struct RECTANGLE *, struct RECTANGLE *, struct RECTANGLE *);
char rect_intersect(struct RECTANGLE *, struct RECTANGLE *);
char rect_is_overlapping(struct RECTANGLE *, struct RECTANGLE *);
char rect_is_inside(struct RECTANGLE *, struct RECTANGLE *);
char rect_is_adjacent(struct RECTANGLE *, struct RECTANGLE *);
void rectlist_add_rect(char *, struct RECTANGLE *, struct RECTANGLE *);
void rectlist_add_rects(unsigned char, char *, struct RECTANGLE *, struct RECTANGLE *, struct RECTANGLE *, char *, struct RECTANGLE *);

void rect_union(struct RECTANGLE *r1, struct RECTANGLE *r2, struct RECTANGLE *out)
{
    out->left = r1->left <= r2->left ? r1->left : r2->left;
    out->right = r1->right >= r2->right ? r1->right : r2->right;
    out->top = r1->top <= r2->top ? r1->top : r2->top;
    out->bottom = r1->bottom >= r2->bottom ? r1->bottom : r2->bottom;
    if (video_flag2_is1 != 1) {
        out->right = (out->right + video_flag2_is1 - 1) & video_flag3_isFFFF;
    }
}

char rect_intersect(struct RECTANGLE *r1, struct RECTANGLE *r2)
{
    if (r1->right < r1->left) return 1;
    if (r2->right <= r1->left) return 1;
    if (r1->right <= r2->left) return 1;
    if (r1->top >= r2->bottom) return 1;
    if (r1->bottom <= r2->top) return 1;

    if (r1->left < r2->left) r1->left = r2->left;
    if (r1->right > r2->right) r1->right = r2->right;
    if (r1->top < r2->top) r1->top = r2->top;
    if (r1->bottom > r2->bottom) r1->bottom = r2->bottom;
    return 0;
}

void rectlist_add_rect(char* arg_rect_array_length_ptr, struct RECTANGLE* arg_rect_array_ptr, struct RECTANGLE* rect) {	
	struct RECTANGLE bottom_rect;
	struct RECTANGLE merged;
	char shift_index;
	char rect_count;
	char top_needed;
	struct RECTANGLE* entry_ptr;
	struct RECTANGLE top_piece;
	char bottom_needed;

	if (video_flag2_is1 != 1) {
		rect->right = (rect->right + video_flag2_is1 - 1) & video_flag3_isFFFF;
		/*
		mov     bx, [bp+arg_rectptr]
		mov     si, bx
		mov     ax, [si+RECTANGLE.rc_right]
		add     ax, video_flag2_is1
		dec     ax
		and     ax, video_flag3_isFFFF
		mov     [bx+RECTANGLE.rc_right], ax*/
	}
	
	for (rect_count = 0; rect_count < *arg_rect_array_length_ptr; rect_count++) {
		entry_ptr = &arg_rect_array_ptr[rect_count];
		if (rect_is_overlapping(rect, entry_ptr) == 0)
			continue;
		if (rect_is_inside(rect, entry_ptr) != 0)
			return ;

		if (rect_is_inside(entry_ptr, rect) != 0) {
			shift_index = rect_count;

			while (((*arg_rect_array_length_ptr) - 1) > shift_index) {
				arg_rect_array_ptr[shift_index] = arg_rect_array_ptr[shift_index + 1];
				shift_index++;
			}
			(*arg_rect_array_length_ptr)--;
			continue;
		}

		merged = *entry_ptr;
		if (entry_ptr->top < rect->top) {
			top_piece = *entry_ptr;
			top_piece.bottom = rect->top;
			merged.top = rect->top;
			top_needed = 1;
		} else {
			if (rect->top < entry_ptr->top) {
				top_piece = *rect;
				top_piece.bottom = entry_ptr->top;
				top_needed = 1;
			} else {
				top_needed = 0;
			}
		}

		if (entry_ptr->bottom > rect->bottom) {
			bottom_rect = *entry_ptr;
			bottom_rect.top = rect->bottom;
			merged.bottom = rect->bottom;
			bottom_needed = 1;
		} else {
			if (rect->bottom > entry_ptr->bottom) {
				bottom_rect = *rect;
				bottom_rect.top = entry_ptr->bottom;
				bottom_needed = 1;
			} else {
				bottom_needed = 0;
			}
		}

		merged.left = rect->left > entry_ptr->left ? entry_ptr->left : rect->left;

		merged.right = rect->right < entry_ptr->right ? entry_ptr->right : rect->right;

		shift_index = rect_count;

		while (((*arg_rect_array_length_ptr) - 1) > shift_index) {
			arg_rect_array_ptr[shift_index] = arg_rect_array_ptr[shift_index + 1];
			shift_index ++;
		}
		(*arg_rect_array_length_ptr)--;
		if (top_needed != 0) {
			rectlist_add_rect(arg_rect_array_length_ptr, arg_rect_array_ptr, &top_piece);
		}

		rectlist_add_rect(arg_rect_array_length_ptr, arg_rect_array_ptr, &merged);
		if (bottom_needed != 0) {
			rectlist_add_rect(arg_rect_array_length_ptr, arg_rect_array_ptr, &bottom_rect);
			return ;
		}
		return ;
	}

	for (rect_count = 0; rect_count < *arg_rect_array_length_ptr; rect_count++) {
		entry_ptr = &arg_rect_array_ptr[rect_count];

		if (rect_is_adjacent(entry_ptr, rect) != 0) {
		merged.left = entry_ptr->left <= rect->left ? entry_ptr->left : rect->left;
		merged.right = entry_ptr->right >= rect->right ? entry_ptr->right : rect->right;
		merged.top = entry_ptr->top <= rect->top ? entry_ptr->top : rect->top;
		merged.bottom = entry_ptr->bottom >= rect->bottom ? entry_ptr->bottom : rect->bottom;

		shift_index = rect_count;

		while (((*arg_rect_array_length_ptr) - 1) > shift_index) {
			arg_rect_array_ptr[shift_index] = arg_rect_array_ptr[shift_index + 1];
			shift_index ++;
		}
		(*arg_rect_array_length_ptr)--;
		rectlist_add_rect(arg_rect_array_length_ptr, arg_rect_array_ptr, &merged);
		return ;
		}
	}

	arg_rect_array_ptr[*arg_rect_array_length_ptr] = *rect;
	(*arg_rect_array_length_ptr)++;
}

char rect_is_overlapping(struct RECTANGLE* r1, struct RECTANGLE* r2) {
	if (r1->right <= r2->left) {
		return 0;
	}
	
	if (r2->right <= r1->left) {
		return 0;
	}
	
	if (r1->top >= r2->bottom) {
		return 0;
	}
	
	if (r1->bottom <= r2->top) {
		return 0;
	}
	
	return 1;
}

char rect_is_inside(struct RECTANGLE *a,struct RECTANGLE *b){return a->right<=b->right && a->left>=b->left && a->top>=b->top && a->bottom<=b->bottom;}

char rect_is_adjacent(struct RECTANGLE* r1, struct RECTANGLE* r2) {
 if (r1->bottom == r2->top) { if (r1->left != r2->left) return 0; if (r1->right != r2->right) return 0; return 1; }
 else if (r1->top == r2->bottom) { if (r1->left != r2->left) return 0; if (r1->right != r2->right) return 0; return 1; }
 else if (r1->right == r2->left) { if (r1->top != r2->top) return 0; if (r1->bottom != r2->bottom) return 0; return 1; }
 else if (r2->right == r1->left) { if (r1->top != r2->top) return 0; if (r1->bottom != r2->bottom) return 0; return 1; }
 return 0;
}

void rectlist_add_rects(unsigned char arg_rectcount, char* arg_rectarray_indices, 
	struct RECTANGLE* arg_rectarray1, struct RECTANGLE* arg_rectarray2, 
	struct RECTANGLE* arg_rectptr, char* arg_rect_array_length_ptr, struct RECTANGLE* arg_rect_array_ptr) 
{
	char has_result;
	struct RECTANGLE* selected_rect_ptr;
	struct RECTANGLE cover_rect;
	struct RECTANGLE* first_ptr;
	char rect_counter;
	char source_flags;
	struct RECTANGLE input_rect;
	struct RECTANGLE* second_rect_ptr;
/*
	return ported_rect_clip_combined_(
		arg_rectcount, arg_rectarray_indices, arg_rectarray1, arg_rectarray2, arg_rectptr,
		arg_rect_array_length_ptr, arg_rect_array_ptr);
	*/
	for (rect_counter = 0; rect_counter < arg_rectcount; rect_counter++) {

		source_flags = arg_rectarray_indices[rect_counter];
		if ((source_flags & 1) != 0) {
			first_ptr = &arg_rectarray1[rect_counter];
		}

		if ((source_flags & 2) != 0) {
			second_rect_ptr = &arg_rectarray2[rect_counter];
		}

		if ((source_flags & 1) != 0 && first_ptr->right > first_ptr->left) {
			if ((source_flags & 2) != 0 && second_rect_ptr->right > second_rect_ptr->left) {
				rect_union(first_ptr, second_rect_ptr, &cover_rect);
				selected_rect_ptr = &cover_rect;
			} else {
				selected_rect_ptr = first_ptr;
			}
			has_result = 1;
		} else if ((source_flags & 2) != 0 && second_rect_ptr->right > second_rect_ptr->left) {
			selected_rect_ptr = second_rect_ptr;
			has_result = 1;
		} else {
			has_result = 0;
		}
		if (has_result != 0) {
			input_rect = *selected_rect_ptr;
			if (rect_intersect(&input_rect, arg_rectptr) == 0) {
				rectlist_add_rect(arg_rect_array_length_ptr, arg_rect_array_ptr, &input_rect);
			}
		}
	}

}
