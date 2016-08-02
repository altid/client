// Main drawing functions
void ub_initialize(char *);
void ub_draw();
void ub_destroy();

static Func ub_function[] = {
  { "enter_button", ub_input_enter_button, {.i = 0} },
};
