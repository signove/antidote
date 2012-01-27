/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*- */
/** @file log.h Logging utilities functions.
 **
 * Copyright (C) 2010 Signove Tecnologia Corporation.
 * All rights reserved.
 * Contact: Signove Tecnologia Corporation (contact@signove.com)
 *
 * $LICENSE_TEXT:BEGIN$
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation and appearing
 * in the file LICENSE included in the packaging of this file; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA
 * $LICENSE_TEXT:END$
 **
 ** \date Aug 12, 2010
 ** \author Adrian Guedes
 */

#ifndef LOG_H_
#define LOG_H_

#include <stdio.h>

/**
 * \def LOG_OUTPUT
 * Output of the log.
 */
#define LOG_OUTPUT stderr

/**
 * @brief Log to the output defined in LOG_OUTPUT.
 * @param level log level.
 * @param ... va_args like in printf.
 * @see printf
 */
#ifdef ANDROID
#include <android/log.h>
#define LOG(level, ...) \
	{ \
		__android_log_print(ANDROID_LOG_WARN, "antidote", level); \
		__android_log_print(ANDROID_LOG_WARN, "antidote",  __VA_ARGS__); \
	}
#else
#define LOG(level, ...) \
	{ \
		fprintf(LOG_OUTPUT, level); \
		fprintf(LOG_OUTPUT, "<%s in %s:%d> ", __FUNCTION__, __FILE__, __LINE__); \
		fprintf(LOG_OUTPUT, __VA_ARGS__); \
		fprintf(LOG_OUTPUT, "\n"); \
		fflush(LOG_OUTPUT); \
	}
#endif

/**
 * @brief Logs a debug level message at the log output.
 * @param ... va_args like in printf.
 * @see printf
 */
#define DEBUG(...)   LOG("DEBUG   ", __VA_ARGS__)

/**
 * @brief Logs a error level message at the log output.
 * @param ... va_args like in printf.
 * @see printf
 */
#define ERROR(...)   LOG("ERROR   ", __VA_ARGS__)

/**
 * @brief Logs a warning level message at the log output.
 * @param ... va_args like in printf.
 * @see printf
 */
#define WARNING(...) LOG("WARNING ", __VA_ARGS__)

/**
 * @brief Logs a information level message at the log output.
 * @param ... va_args like in printf.
 * @see printf
 */
#define INFO(...)    LOG("INFO    ", __VA_ARGS__)

#endif /* LOG_H_ */
