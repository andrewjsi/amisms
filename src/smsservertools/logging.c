/*
SMS Server Tools 3
Copyright (C) 2006- Keijo Kasvi
http://smstools3.kekekasvi.com/

Based on SMS Server Tools 2, http://stefanfrings.de/smstools/
SMS Server Tools version 2 and below are Copyright (C) Stefan Frings.

This program is free software unless you got it under another license directly
from the author. You can redistribute it and/or modify it under the terms of
the GNU General Public License as published by the Free Software Foundation.
Either version 2 of the License, or (at your option) any later version.
*/

#include "logging.h"
#include "extras.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <syslog.h>
#include <time.h>
#include <fcntl.h>
#include <unistd.h>
#include "smsd_cfg.h"
#include "stats.h"

int Filehandle = -1;
int Level;
int SavedLevel;
int Filehandle_trouble = -1;
char *trouble_logging_buffer = 0;
int last_flush_was_clear = 1;
unsigned long long trouble_logging_start_time = 0; // 3.1.16beta. For total time of continuous trouble.
int logging_start_printed = 0; // 3.1.16beta.

int change_loglevel(int new_level)
{

  SavedLevel = Level;
  Level = new_level;

  return SavedLevel;
}

void restore_loglevel()
{

  Level = SavedLevel;
}

// 3.1.14:
int get_loglevel()
{

  return Level;
}

int openlogfile(char *filename, int facility, int level)
{
  int result = 0;

  closelogfile();

  Level = level;

  if (filename==0 || filename[0]==0 || strcmp(filename,"syslog")==0 || strcmp(filename,"0")==0)
  {
    openlog("smsd", LOG_CONS, facility);
    Filehandle = -1;
    Filehandle_trouble = -1;
  }
  else if (strcmp(filename, "1") == 0 || strcmp(filename, "2") == 0) //(is_number(filename))
  {
    int oldfilehandle;
    oldfilehandle=atoi(filename);
    Filehandle=dup(oldfilehandle);
    if (Filehandle<0)
    {
      fprintf(stderr, "Cannot duplicate logfile handle\n");
      exit(1);
    }
    else
      result = Filehandle;
  }
  else
  {
    Filehandle=open(filename,O_APPEND|O_WRONLY|O_CREAT,0640);
    if (Filehandle<0)
    {
      fprintf(stderr, "Cannot open logfile\n");
      exit(1);
    }
    else
    {
      result = Filehandle;

      if (smart_logging) // 3.1.16beta. && level < 7)
      {
        char filename2[PATH_MAX];
        int error = 0;
        int i;

        if ((size_t)snprintf(filename2, sizeof(filename2), "%s", filename) >= sizeof(filename2))
          error = 1;
        else
        {
          if (strlen(filename2) > 4 && !strcmp(filename2 +strlen(filename2) -4, ".log"))
          {
            filename2[strlen(filename2) -4] = 0;
            i = sizeof(filename2) - strlen(filename2);
            if (snprintf(strchr(filename2, 0), i, "_trouble.log") >= i)
              error = 2; 
          }
          else
          {
            i = sizeof(filename2) - strlen(filename2);
            if (snprintf(strchr(filename2, 0), i, ".trouble") >= i)
              error = 3;
          }
        }

        if (!error)
        {
          Filehandle_trouble = open(filename2, O_APPEND | O_WRONLY | O_CREAT, 0640);
          if (Filehandle_trouble < 0)
            error = 4;
        }

        if (error)
        {
          closelogfile();
          fprintf(stderr, "Cannot open logfile for smart logging (error: %i)\n", error);
          exit(1);
        }
      }
    }
  }

  return result;
}

void closelogfile()
{
  if (Filehandle>=0)
  {
    close(Filehandle);
    Filehandle = -1;
  }

  if (Filehandle_trouble >= 0)
  {
    close(Filehandle_trouble);
    Filehandle_trouble = -1;
  }

  if (trouble_logging_buffer)
  {
    free(trouble_logging_buffer);
    trouble_logging_buffer = 0;
  }

  trouble_logging_started = 0;
  trouble_logging_start_time = 0;
}

void writelogfile0(int severity, int trouble, char *text)
{
  writelogfile(severity, trouble, "%s", text);
}

void writelogfile(int severity, int trouble, char* format, ...)
{
  va_list argp;
  char text[SIZE_LOG_LINE];
  char text2[SIZE_LOG_LINE];
  char timestamp[40];

  // make a string of the arguments
  va_start(argp,format);
  vsnprintf(text,sizeof(text),format,argp);
  va_end(argp);

  // 3.1.6: Remove \r from the end:
  while (strlen(text) > 0 && text[strlen(text) - 1] == '\r')
    text[strlen(text) - 1] = 0;

  if (severity<=Level)
  {
    if (Filehandle<0)
    {
      if (strcmp(process_title, "smsd") == 0)
        syslog(severity, "MAINPROCESS: %s", text);
      else
        syslog(severity, "%s: %s", process_title, text);
    }
    else
    {
      make_datetime_string(timestamp, sizeof(timestamp), 0, 0, logtime_format);
      snprintf(text2, sizeof(text2),"%s,%i, %s: %s\n", timestamp, severity, process_title, text);

      // 3.1.5:
      if (text2[strlen(text2) -1] != '\n')
        strcpy(text2 +sizeof(text2) -5, "...\n");

      write(Filehandle,text2,strlen(text2));
    }
  }

  if (smart_logging) // 3.1.16beta. && Level < 7)
  {
    // 3.1.16beta: Mark the start of communication:
    if (!logging_start_printed)
    {
      logging_start_printed = 1;

      make_datetime_string(timestamp, sizeof(timestamp), 0, 0, logtime_format);
      snprintf(text2, sizeof(text2),"%s,%i, %s: Start:\n", timestamp, severity, process_title);
      strcat_realloc(&trouble_logging_buffer, text2, 0);
    }

    if (trouble)
    {
      if (!trouble_logging_started)
      {
        trouble_logging_started = 1;

        // 3.1.16beta:
        if (!trouble_logging_start_time)
        {
          if (put_command_sent)
            trouble_logging_start_time = put_command_sent;
          else
            trouble_logging_start_time = time_usec();
        }

        // Global process_id is the same as int device in many functions calls.
        if (process_id >= 0)
          statistics[process_id]->status = 't';

        // 3.1.16beta: Mark the start of trouble (not when a process is starting, value 2):
        if (trouble == 1)
        {
          make_datetime_string(timestamp, sizeof(timestamp), 0, 0, logtime_format);
          snprintf(text2, sizeof(text2),"%s,%i, %s: Trouble:\n", timestamp, severity, process_title);
          strcat_realloc(&trouble_logging_buffer, text2, 0);
        }

      }
    }

    // Any message is stored:
    make_datetime_string(timestamp, sizeof(timestamp), 0, 0, logtime_format);
    snprintf(text2, sizeof(text2),"%s,%i, %s: %s\n", timestamp, severity, process_title, text);

    if (text2[strlen(text2) -1] != '\n')
      strcpy(text2 +sizeof(text2) -5, "...\n");

    strcat_realloc(&trouble_logging_buffer, text2, 0);
  }
}

int make_duration_string(char *dest, size_t size_dest, unsigned long long time_start, unsigned long long time_now)
{
  int duration;

  *dest = 0;

  duration = (int)(time_now - time_start) / 100000;
  if (duration >= 10)
  {
    if (duration < 600)
      snprintf(dest, size_dest, "%.1f sec.", (double)duration / 10);
    else
    {
      duration = (int)duration / 10;
      snprintf(dest, size_dest, "%i min %i sec.", (int)duration / 60, (int)(duration - duration / 60 * 60));
    }

    return 1;
  }

  return 0;
}

void flush_smart_logging()
{
  static unsigned long long last_flush_time = 0; // 3.1.16beta.
  char text2[SIZE_LOG_LINE];
  char timestamp[40];
  char duration[64];

  if (trouble_logging_started && trouble_logging_buffer)
  {
    write(Filehandle_trouble, trouble_logging_buffer, strlen(trouble_logging_buffer));
    last_flush_was_clear = 0;
    last_flush_time = time_usec();

    if (trouble_logging_start_time)
    {
      make_duration_string(duration, sizeof(duration), trouble_logging_start_time, last_flush_time);
      make_datetime_string(timestamp, sizeof(timestamp), 0, 0, logtime_format);
      snprintf(text2, sizeof(text2), "%s,%i, %s: (flush) %s\n", timestamp, LOG_NOTICE, process_title, duration);
      write(Filehandle_trouble, text2, strlen(text2));
    }
  }
  else
  {
    // 3.1.6: If some errors were printed and now all is ok, print it to the log:
    if (!last_flush_was_clear && Filehandle_trouble >= 0)
    {
      char trouble_time[128];

      make_datetime_string(timestamp, sizeof(timestamp), 0, 0, logtime_format);

      // 3.1.16beta:
      //snprintf(text2, sizeof(text2), "%s,%i, %s: %s\n", timestamp, LOG_NOTICE, process_title, "Everything ok now.");
      *trouble_time = 0;
      if (trouble_logging_start_time && last_flush_time)
        if (make_duration_string(duration, sizeof(duration), trouble_logging_start_time, last_flush_time))
          snprintf(trouble_time, sizeof(trouble_time), " Duration of trouble was %s", duration);

      snprintf(text2, sizeof(text2), "%s,%i, %s: %s%s\n", timestamp, LOG_NOTICE, process_title, "Everything ok now.", trouble_time);
      write(Filehandle_trouble, text2, strlen(text2));
    }

    last_flush_was_clear = 1;
    logging_start_printed = 0;
    trouble_logging_start_time = 0;
    put_command_sent = 0;
    last_flush_time = 0;
  }

  trouble_logging_started = 0;

  if (trouble_logging_buffer)
  {
    free(trouble_logging_buffer);
    trouble_logging_buffer = 0;
  }
}

// ******************************************************************************
// Collect character conversion log, flush it if called with format==NULL.
// Also prints to the stdout, if debugging.
void logch(char* format, ...)
{
  va_list argp;
  char text[2048];
  int flush = 0;

  if (format)
  {
    va_start(argp, format);
    vsnprintf(text, sizeof(text), format, argp);
    va_end(argp);

    if (strlen(logch_buffer) +strlen(text) < sizeof(logch_buffer))
    {
      sprintf(strchr(logch_buffer, 0), "%s", text);
      // Line wrap after space character:
      // Outgoing conversion:
      if (strlen(text) >= 3)
        if (strcmp(text +strlen(text) -3, "20 ") == 0)
          flush = 1;
      // Incoming conversion:
      if (!flush)
        if (strlen(text) >= 6)
          if (strcmp(text +strlen(text) -6, "20[ ] ") == 0)
            flush = 1;
      // Line wrap after a reasonable length reached:
      if (!flush)
        if (strlen(logch_buffer) > 80)
          flush = 1;
    }
#ifdef DEBUGMSG
  printf("%s", text);
#endif
  }
  else
    flush = 1;

  if (flush)
  {
    if (*logch_buffer)
      writelogfile(LOG_DEBUG, 0, "charconv: %s", logch_buffer);
    *logch_buffer = 0;
#ifdef DEBUGMSG
  printf("\n");
#endif
  }
}

char prch(char ch)
{
  if ((unsigned char)ch >= ' ')
    return ch;
  return '.';
}
