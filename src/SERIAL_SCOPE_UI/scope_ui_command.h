#pragma once

void refresh_tools      ();
void refresh_caret_pos  ();

void cmd_new    ();
void cmd_open   ();
void cmd_save   ();
void cmd_saveas ();
void cmd_cut    ();
void cmd_copy   ();
void cmd_paste  ();
void cmd_del    ();
void cmd_undo   ();
void cmd_redo   ();
void cmd_run    ();
void cmd_pause  ();
void cmd_stop   ();
void cmd_step   ();

int gui_xml_load_file(TCHAR *);