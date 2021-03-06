﻿/**
 * File:   file_chooser.c
 * Author: AWTK Develop Team
 * Brief:  file/folder choosers
 *
 * Copyright (c) 2020 - 2020 Guangzhou ZHIYUAN Electronics Co.,Ltd.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * License file for more details.
 *
 */

/**
 * History:
 * ================================================================
 * 2020-01-12 Li XianJing <xianjimli@hotmail.com> created
 *
 */

#include "tkc/mem.h"
#include "tkc/utils.h"
#include "base/window.h"
#include "file_browser/file_chooser.h"
#include "file_browser/file_browser_view.h"

file_chooser_t* file_chooser_create(const char* init_dir, const char* filter) {
  file_chooser_t* chooser = TKMEM_ZALLOC(file_chooser_t);
  return_value_if_fail(chooser != NULL, NULL);

  chooser->init_dir = tk_str_copy(chooser->init_dir, init_dir);
  chooser->filter = tk_str_copy(chooser->filter, filter);
  str_init(&(chooser->cwd), 0);
  str_init(&(chooser->filename), 0);

  return chooser;
}

ret_t file_chooser_set_on_result(file_chooser_t* chooser, file_chooser_on_result_t on_result,
                                 void* on_result_ctx) {
  return_value_if_fail(chooser != NULL && on_result != NULL, RET_BAD_PARAMS);

  chooser->on_result = on_result;
  chooser->on_result_ctx = on_result_ctx;

  return RET_OK;
}

static ret_t file_choose_on_click_to_close(void* ctx, event_t* e) {
  widget_t* win = widget_get_window(WIDGET(e->target));
  file_chooser_t* chooser = (file_chooser_t*)ctx;
  widget_t* widget = widget_lookup_by_type(win, WIDGET_TYPE_FILE_BROWSER_VIEW, TRUE);
  file_browser_view_t* file_browser = FILE_BROWSER_VIEW(widget);

  chooser->aborted = FALSE;
  if (chooser->on_result != NULL) {
    chooser->on_result(chooser);
  }

  widget_close_window(win);
  file_chooser_destroy(chooser);

  return RET_OK;
}

static ret_t file_choose_on_ok(void* ctx, event_t* e) {
  widget_t* win = widget_get_window(WIDGET(e->target));
  file_chooser_t* chooser = (file_chooser_t*)ctx;
  widget_t* widget = widget_lookup_by_type(win, WIDGET_TYPE_FILE_BROWSER_VIEW, TRUE);
  file_browser_view_t* file_browser = FILE_BROWSER_VIEW(widget);
  widget_t* selected_file = widget_lookup(widget, FILE_BROWSER_VIEW_SELECTED_FILE, TRUE);

  str_set(&(chooser->cwd), file_browser_view_get_cwd(widget));
  if (selected_file != NULL) {
    str_from_wstr(&(chooser->filename), selected_file->text.str);
  }

  chooser->aborted = FALSE;
  if (chooser->on_result != NULL) {
    if (chooser->on_result(chooser) == RET_OK) {
      widget_close_window(win);
      file_chooser_destroy(chooser);
    }
  }

  return RET_OK;
}

ret_t file_chooser_choose(file_chooser_t* chooser) {
  widget_t* win = window_open(chooser->ui);

  widget_child_on(win, FILE_CHOOSER_OK, EVT_CLICK, file_choose_on_ok, chooser);
  widget_child_on(win, FILE_CHOOSER_CANCEL, EVT_CLICK, file_choose_on_click_to_close, chooser);

  return RET_OK;
}

ret_t file_chooser_choose_file_for_save(file_chooser_t* chooser) {
  return_value_if_fail(chooser != NULL && chooser->on_result != NULL, RET_BAD_PARAMS);

  chooser->ui = FILE_CHOOSER_UI_CHOOSE_FILE_FOR_SAVE;

  return file_chooser_choose(chooser);
}

ret_t file_chooser_choose_file_for_open(file_chooser_t* chooser) {
  return_value_if_fail(chooser != NULL && chooser->on_result != NULL, RET_BAD_PARAMS);
  chooser->ui = FILE_CHOOSER_UI_CHOOSE_FILE_FOR_OPEN;

  return file_chooser_choose(chooser);
}

ret_t file_chooser_choose_folder(file_chooser_t* chooser) {
  return_value_if_fail(chooser != NULL && chooser->on_result != NULL, RET_BAD_PARAMS);

  chooser->ui = FILE_CHOOSER_UI_CHOOSE_FOLDER;

  return file_chooser_choose(chooser);
}

ret_t file_chooser_destroy(file_chooser_t* chooser) {
  return_value_if_fail(chooser != NULL, RET_BAD_PARAMS);

  str_reset(&(chooser->cwd));
  str_reset(&(chooser->filename));

  TKMEM_FREE(chooser->filter);
  TKMEM_FREE(chooser->init_dir);
  TKMEM_FREE(chooser);

  return RET_OK;
}
