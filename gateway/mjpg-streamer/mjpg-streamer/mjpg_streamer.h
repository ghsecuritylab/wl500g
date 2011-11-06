/*******************************************************************************
#                                                                              #
#      MJPG-streamer allows to stream JPG frames from an input-plugin          #
#      to several output plugins                                               #
#                                                                              #
#      Copyright (C) 2007 Tom Stöveken                                         #
#                                                                              #
# This program is free software; you can redistribute it and/or modify         #
# it under the terms of the GNU General Public License as published by         #
# the Free Software Foundation; version 2 of the License.                      #
#                                                                              #
# This program is distributed in the hope that it will be useful,              #
# but WITHOUT ANY WARRANTY; without even the implied warranty of               #
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the                #
# GNU General Public License for more details.                                 #
#                                                                              #
# You should have received a copy of the GNU General Public License            #
# along with this program; if not, write to the Free Software                  #
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA    #
#                                                                              #
*******************************************************************************/
#define SOURCE_VERSION "2.0"
#define MAX_OUTPUT_PLUGINS 10
#include <linux/types.h>          /* for videodev2.h */
#include <linux/videodev2.h>

#ifdef DEBUG
#define DBG(...) fprintf(stderr, " DBG(%s, %s(), %d): ", __FILE__, __FUNCTION__, __LINE__); fprintf(stderr, __VA_ARGS__)
#else
#define DBG(...)
#endif

#define LOG(...) { char _bf[1024] = {0}; snprintf(_bf, sizeof(_bf)-1, __VA_ARGS__); fprintf(stderr, "%s", _bf); syslog(LOG_INFO, "%s", _bf); }

#include "plugins/input.h"
#include "plugins/output.h"

/* global variables that are accessed by all plugins */
typedef struct _globals globals;

/* commands which can be send to mjpg-streamer */
typedef enum _control_cmd_type control_cmd_type;
enum _control_cmd_type {
  CONTROL_CMD_UNKNOWN = 0,
  CONTROL_CMD_RECONFIGURE_INPUT
};

struct _globals {
  int stop;
  char *pidfile;

  /* signal fresh frames */
  pthread_mutex_t db;
  pthread_cond_t  db_update;

  /* global JPG frame, this is more or less the "database" */
  unsigned char *buf;
  int size;

  /* input plugin */
  input in;

  /* output plugin */
  output out[MAX_OUTPUT_PLUGINS];
  int outcnt;

  /* pointer to control functions */
  int (*control)(int command, char *details);
};