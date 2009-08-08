/* (c) 2003-2007 Claudio Clemens <asturio at gmx dot net>

  This program is free software; you can redistribute it and/or modify it under
  the terms of the GNU General Public License as published by the Free Software
  Foundation; either version 2, or (at your option) any later version.
 
  This program is distributed in the hope that it will be useful, but WITHOUT
  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
  FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
  details.
 
  You should have received a copy of the GNU General Public License along with
  this program; if not, write to the Free Software Foundation, Inc., 59 Temple
  Place - Suite 330, Boston, MA 02111-1307, USA.
*/

/*  
    $Author: claudio $
    $Date: 2007-03-26 18:01:25 $
*/

#include <config.h>
#include <stdio.h>
#include "ringbuf.h"

/** \file ringbuf.c
 * \brief An implementation of a ringbuffer framework. */

char ringbuf[MAXBUFS][BUFSIZE]; /**< The ring buffer */
int ringindex = 0; /**< The index of the current buffer */

/** \brief Increment the ringbuf index
 */
void nextbuf() {
    ringindex = (ringindex + 1) % MAXBUFS;
}

/** \brief Increment a given index
 * \param i Index to increment
 * \return the incremented index
 */
int incIndex(int i) {
    return (i + 1) % MAXBUFS;
}

/** \brief Get the actual ring index
 * \return the ring index
 */
int getRingIndex() {
    return ringindex;
}

/** \brief Gets the message of a given index
 * \param i Index of the message to retrieve
 * \return The message
 */
char *getMessage(int i) {
    if (i < 0 || i >= MAXBUFS) {
        return "";
    }
    return ringbuf[i];
}

/** \brief Store a message in the ring.
 * After the message is stored in the ring, the internal index will be
 * incremented
 * \param s A message to store in the ring
 */
void storeMessage(char *s) {
    snprintf(ringbuf[ringindex], BUFSIZE, "%s", s);
    nextbuf();
}
