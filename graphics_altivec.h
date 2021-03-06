/* -*- C++ -*-
 * 
 *  graphics_altivec.h - graphics routines using PPC Altivec cpu functionality
 *
 *  Copyright (c) 2009 Mion. All rights reserved.
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifdef USE_PPC_GFX

void imageFilterMean_Altivec(unsigned char *src1, unsigned char *src2, unsigned char *dst, int length);
void imageFilterAddTo_Altivec(unsigned char *dst, unsigned char *src, int length);
void imageFilterSubFrom_Altivec(unsigned char *dst, unsigned char *src, int length);

#endif
