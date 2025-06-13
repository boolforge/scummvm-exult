/*
 * Exult - An Ultima 7 Engine
 * Copyright (C) 1998-2008 The Exult Team
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifndef U7FILEMAN_H
#define U7FILEMAN_H

#include "files/u7file.h"
#include "common/types.h"

namespace Exult {

class U7FileManager {
public:
	U7FileManager();
	~U7FileManager();

	U7File *openFile(const char *name, bool must_exist = true);
	void closeFile(U7File *f);

private:
	// TODO: Implement file management logic
};

} // End of namespace Exult

#endif // U7FILEMAN_H

