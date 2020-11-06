#include <SDL2/SDL.h>
#include <stdio.h>
#include "task.h"
#include "render.h"
#include "microui.h"
#include "altid.h"

static  char mainbuf[64000];
static  char statbuf[640];
static  char titlbuf[640];
static  char tabsbuf[16000];
static  float   bg[3] = {
    90,
    95,
    100,
};

static const char button_map[256] = {
  [ SDL_BUTTON_LEFT   & 0xff ] =  MU_MOUSE_LEFT,
  [ SDL_BUTTON_RIGHT  & 0xff ] =  MU_MOUSE_RIGHT,
  [ SDL_BUTTON_MIDDLE & 0xff ] =  MU_MOUSE_MIDDLE,
};

static const char key_map[256] = {
  [ SDLK_LSHIFT       & 0xff ] = MU_KEY_SHIFT,
  [ SDLK_RSHIFT       & 0xff ] = MU_KEY_SHIFT,
  [ SDLK_LCTRL        & 0xff ] = MU_KEY_CTRL,
  [ SDLK_RCTRL        & 0xff ] = MU_KEY_CTRL,
  [ SDLK_LALT         & 0xff ] = MU_KEY_ALT,
  [ SDLK_RALT         & 0xff ] = MU_KEY_ALT,
  [ SDLK_RETURN       & 0xff ] = MU_KEY_RETURN,
  [ SDLK_BACKSPACE    & 0xff ] = MU_KEY_BACKSPACE,
};

static int text_width(mu_Font font, const char *text, int len) {
  if (len == -1) { len = strlen(text); }
  return r_get_text_width(text, len);
}

static int text_height(mu_Font font) {
  return r_get_text_height();
}

void
write_file(int fid, const char *text)
{
    /* Setter function for strings, guarded by mutex */
    pthread_mutex_lock(&lock);
    switch(fid){
    case MAINBUF:
        strcat(mainbuf, text);
        break;
    case STATBUF:
        strcat(statbuf, text);
        break;
    case TITLBUF:
        strcat(titlbuf, text);
        break;
    case TABSBUF:
        strcat(tabsbuf, text);
        break;
    }
    pthread_mutex_unlock(&lock);
}

static void
show_services(mu_Context *ctx)
{
    if(mu_begin_window(ctx, "list", mu_rect(100, 100, 300, 400))){
        Service *service;

        for(service = scanmdns(); service != NULL; service = service->next){
            printf("%s service\n", service->name);
            mu_layout_row(ctx, 1, (int[]) { -1 }, 0);
            if(mu_button_ex(ctx, service->name, 0, 0)){
                //connect(service);
                printf("Pressed %s\n", service->name);
            }
        }
        mu_end_window(ctx);
        freeservice(service);
    }
}

static void
draw_window(mu_Context *ctx)
{
    char title[512];
    const int opt = MU_OPT_NOTITLE| MU_OPT_NOFRAME | MU_OPT_NOSCROLL;

    /* Get title from c9 title */
    if (mu_begin_window_ex(ctx, "main", mu_rect(0, 0, 800, 600), opt)) {
        sprintf(title, "%s - %s", "current buffer", titlbuf);
        mu_layout_row(ctx, 1, (int[]) { -1 }, 0 );
        if(mu_begin_treenode(ctx, title)){
            if(mu_button_ex(ctx, "lorem ipsum solor det amit", 0, 0)){
                write_file(MAINBUF, "top\n");
            }
            if(mu_button_ex(ctx, "ipsum", 0, 0)){
                write_file(MAINBUF, "second\n");
            }
            if(mu_button_ex(ctx, "find services", 0, 0)){
                show_services(ctx);
            }
            mu_end_treenode(ctx);
        }

        /* buffer text panel */
        mu_layout_row(ctx, 1, (int[]) { -1 }, -50);
        mu_begin_panel(ctx, "titlebar");
        mu_Container *panel = mu_get_current_container(ctx);
        mu_layout_row(ctx, 1, (int[]) { -1 }, -1);
        mu_text(ctx, mainbuf);
        mu_end_panel(ctx);
        panel->scroll.y = panel->content_size.y;
    
        /* Status bar */
        mu_layout_row(ctx, 1, (int[]) { -1 }, 0);
        mu_text(ctx, statbuf);

        /* input textbox + submit button */
        static char buf[128];
        int submitted = 0;
        mu_layout_row(ctx, 2, (int[]) { -70, -1 }, 0);
        if (mu_textbox(ctx, buf, sizeof(buf)) & MU_RES_SUBMIT) {
            mu_set_focus(ctx, ctx->last_id);
            submitted = 1;
        }
        if (mu_button(ctx, "input"))
            submitted = 1;
        if (submitted) {
            //send_input(buf);
            strcat(buf, "\n");
            write_file(MAINBUF, buf);
            buf[0] = '\0';
        }
    
        mu_end_window(ctx);
    }
}

void
draw_loop(void)
{
    int b, c;
    /* init SDL and renderer */
    SDL_Init(SDL_INIT_EVERYTHING);
    r_init();

    write_file(MAINBUF, "Not currently connected to anything.\n");
    write_file(TITLBUF, "Welcome to Altid");
    write_file(STATBUF, "Select/add a service from the drop down list");
    /* init microui */
    mu_Context *ctx = malloc(sizeof(mu_Context));
    mu_init(ctx);

    ctx->text_width = text_width;
    ctx->text_height = text_height;

    /* main loop */
    for(;;){
        SDL_Event e;
        while(SDL_PollEvent(&e)){
            switch(e.type){
            case SDL_QUIT:
                exit(EXIT_SUCCESS);
                break;
            case SDL_MOUSEMOTION:
                mu_input_mousemove(ctx, e.motion.x, e.motion.y);
                break;
            case SDL_MOUSEWHEEL:
                mu_input_scroll(ctx, 0, e.wheel.y * SCROLLSTEP);
                break;
            case SDL_TEXTINPUT:
                mu_input_text(ctx, e.text.text);
                break;
            case SDL_MOUSEBUTTONDOWN:
                b = button_map[e.button.button & 0xff];
                mu_input_mousedown(ctx, e.button.x, e.button.y, b);
                break;
            case SDL_MOUSEBUTTONUP:
                b = button_map[e.button.button & 0xff];
                mu_input_mouseup(ctx, e.button.x, e.button.y, b);
                break;
            case SDL_KEYDOWN:
                c = key_map[e.key.keysym.sym & 0xff];
                mu_input_keydown(ctx, c);
                break;
            case SDL_KEYUP:
                c = key_map[e.key.keysym.sym & 0xff];
                mu_input_keyup(ctx, c);
                break;
            }
        }

        mu_begin(ctx);
        draw_window(ctx);
        mu_end(ctx);

        /* render */
        r_clear(mu_color(bg[0], bg[1], bg[2], 255));
        mu_Command *cmd = NULL;
        while (mu_next_command(ctx, &cmd)) {
            switch (cmd->type) {
            case MU_COMMAND_TEXT: 
                r_draw_text(cmd->text.str, cmd->text.pos, cmd->text.color);
                break;
            case MU_COMMAND_RECT:
                r_draw_rect(cmd->rect.rect, cmd->rect.color);
                break;
            case MU_COMMAND_ICON:
                r_draw_icon(cmd->icon.id, cmd->icon.rect, cmd->icon.color);
                break;
            case MU_COMMAND_CLIP:
                r_set_clip_rect(cmd->clip.rect);
                break;
            }    
        }
        r_present();
    }
}