struct SHAPE3DHEADER {
    unsigned char numverts;
    unsigned char numprimitives;
    unsigned char numpaints;
    unsigned char reserved;
};

struct SHAPE3D {
    unsigned short numverts;
    char far *verts;
    unsigned short numprimitives;
    unsigned char numpaints;
    unsigned char reserved;
    char far *primitives;
    char far *cull1;
    char far *cull2;
};

void shape3d_init_shape(char far *shapeptr, struct SHAPE3D *gameshape)
{
    struct SHAPE3DHEADER far *hdr = (struct SHAPE3DHEADER far *)shapeptr;

    gameshape->numverts = hdr->numverts;
    gameshape->numprimitives = hdr->numprimitives;
    gameshape->numpaints = hdr->numpaints;
    gameshape->verts = shapeptr + 4;
    gameshape->cull1 = shapeptr + gameshape->numverts * 6 + 4;
    gameshape->cull2 = shapeptr + gameshape->numprimitives * 4
                       + gameshape->numverts * 6 + 4;
    gameshape->primitives = shapeptr + gameshape->numprimitives * 8
                            + gameshape->numverts * 6 + 4;
}
