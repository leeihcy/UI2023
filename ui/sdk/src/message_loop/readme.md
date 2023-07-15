
glib的循环逻辑

https://github.com/GNOME/glib/blob/main/glib/gmain.c

g_main_loop_run ->
g_main_context_interate ->
g_main_context_iterate_unlocked ->
  g_main_context_prepare_unlocked