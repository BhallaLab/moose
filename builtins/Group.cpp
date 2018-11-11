/**********************************************************************
** This program is part of 'MOOSE', the
** Messaging Object Oriented Simulation Environment.
**           Copyright (C) 2003-2010 Upinder S. Bhalla. and NCBS
** It is made available under the terms of the
** GNU Lesser General Public License version 2.1
** See the file COPYING.LIB for the full notice.
**********************************************************************/

#include <queue>
#include "../basecode/header.h"
#include "Group.h"

//////////////////////////////////////////////////////////////
// MsgSrc Definitions
//////////////////////////////////////////////////////////////
static SrcFinfo0 *group() {
	static SrcFinfo0 group(
			"group",
			"Handle for grouping Elements"
			);
	return &group;
}

const Cinfo* Group::initCinfo()
{

	static Finfo* groupFinfos[] = {
		group(),	// Value
	};

	static Dinfo< Group > dinfo;
	static Cinfo groupCinfo (
		"Group",
		Neutral::initCinfo(),
		groupFinfos,
		sizeof( groupFinfos ) / sizeof ( Finfo* ),
		&dinfo
	);

	return &groupCinfo;
}

static const Cinfo* groupCinfo = Group::initCinfo();

Group::Group()
{
	;
}
