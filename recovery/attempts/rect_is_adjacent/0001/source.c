struct RECTANGLE { int left; int right; int top; int bottom; };

int rect_is_adjacent(struct RECTANGLE* r1, struct RECTANGLE* r2) {
 if (r1->bottom == r2->top) { if (r1->left != r2->left) return 0; if (r1->right != r2->right) return 0; return 1; }
 else if (r1->top == r2->bottom) { if (r1->left != r2->left) return 0; if (r1->right != r2->right) return 0; return 1; }
 else if (r1->right == r2->left) { if (r1->top != r2->top) return 0; if (r1->bottom != r2->bottom) return 0; return 1; }
 else if (r2->right == r1->left) { if (r1->top != r2->top) return 0; if (r1->bottom != r2->bottom) return 0; return 1; }
 return 0;
}
