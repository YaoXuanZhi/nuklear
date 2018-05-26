/*
 * Nuklear - 1.32.0 - public domain
 * no warrenty implied; use at your own risk.
 * authored from 2015-2016 by Micha Mettke
 */
/*
 * ==============================================================
 *
 *                              API
 *
 * ===============================================================
 */
#ifndef NK_GDI_H_
#define NK_GDI_H_

#define NK_INCLUDE_DEFAULT_ALLOCATOR

#include "../../nuklear.h"
#include "../../contrib/CSimWndFramework.h"
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

struct GdiFont {
    nk_user_font nk;
    int height;
    HFONT handle;
    HDC dc;
};

struct GdiCtx {
    HBITMAP bitmap;
    HDC window_dc;
    HDC memory_dc;
    unsigned int width;
    unsigned int height;
    nk_context ctx;
} ;

class CImGuiProvider{
public:
    CImGuiProvider(){}
    virtual ~CImGuiProvider(){}
protected:
    GdiCtx gdi;
public:
    struct nk_context* nk_gdi_init(GdiFont *font, HDC window_dc, unsigned int width, unsigned int height);
    int nk_gdi_handle_event(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
    void nk_gdi_render(struct nk_color clear);
    void nk_gdi_shutdown(void);

    /* font */
    GdiFont* nk_gdifont_create(const char *name, int size);
    void nk_gdifont_del(GdiFont *font);
    void nk_gdi_set_font(GdiFont *font);

protected:
    void nk_create_image(struct nk_image * image, const char * frame_buffer, const int width, const int height);
    void nk_delete_image(struct nk_image * image);
    void nk_gdi_draw_image(short x, short y, unsigned short w, unsigned short h,
    struct nk_image img, struct nk_color col);
    COLORREF convert_color(struct nk_color c);
    void nk_gdi_scissor(HDC dc, float x, float y, float w, float h);
    void nk_gdi_stroke_line(HDC dc, short x0, short y0, short x1,
        short y1, unsigned int line_thickness, struct nk_color col);
    void nk_gdi_stroke_rect(HDC dc, short x, short y, unsigned short w,
        unsigned short h, unsigned short r, unsigned short line_thickness, struct nk_color col);
    void nk_gdi_fill_rect(HDC dc, short x, short y, unsigned short w,
        unsigned short h, unsigned short r, struct nk_color col);
    void nk_gdi_fill_triangle(HDC dc, short x0, short y0, short x1,
        short y1, short x2, short y2, struct nk_color col);
    void nk_gdi_stroke_triangle(HDC dc, short x0, short y0, short x1,
        short y1, short x2, short y2, unsigned short line_thickness, struct nk_color col);
    void nk_gdi_fill_polygon(HDC dc, const struct nk_vec2i *pnts, int count, struct nk_color col);
    void nk_gdi_stroke_polygon(HDC dc, const struct nk_vec2i *pnts, int count,
        unsigned short line_thickness, struct nk_color col);
    void nk_gdi_stroke_polyline(HDC dc, const struct nk_vec2i *pnts,
        int count, unsigned short line_thickness, struct nk_color col);
    void nk_gdi_fill_circle(HDC dc, short x, short y, unsigned short w,
        unsigned short h, struct nk_color col);
    void nk_gdi_stroke_circle(HDC dc, short x, short y, unsigned short w,
        unsigned short h, unsigned short line_thickness, struct nk_color col);
    void nk_gdi_stroke_curve(HDC dc, struct nk_vec2i p1,
    struct nk_vec2i p2, struct nk_vec2i p3, struct nk_vec2i p4,
        unsigned short line_thickness, struct nk_color col);
    void nk_gdi_draw_text(HDC dc, short x, short y, unsigned short w, unsigned short h,
        const char *text, int len, GdiFont *font, struct nk_color cbg, struct nk_color cfg);
    void nk_gdi_clear(HDC dc, struct nk_color col);
    void nk_gdi_blit(HDC dc);

public:
    static float nk_gdifont_get_text_width(nk_handle handle, float height, const char *text, int len);
    static void nk_gdi_clipbard_paste(nk_handle usr, struct nk_text_edit *edit);
    static void nk_gdi_clipbard_copy(nk_handle usr, const char *text, int len);
};

#endif