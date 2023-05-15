/* -*- Mode: c++; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4; -*-
 *
 * Copyright (C) 1995-2011 Opera Software AS.  All rights reserved.
 * May not be redistributed without prior written permission.
 *
 */

#ifndef OIPF_ACTION_H
#define	OIPF_ACTION_H

class ResizeOipfWindowAction : public OperaAction
{
	public:
		ResizeOipfWindowAction(GogiOperaWindow* _window, Asteriks* _asteriks, GogiRect& _rect) : window(_window), asteriks(_asteriks), rect(_rect) {}
		void execute()
		{
			if (!opera_hbbtv_get_window_identifier(window))
				return;

			if (window && asteriks)
				asteriks->resizeGogiWindow(window, rect);
		}
	private:
		GogiOperaWindow* window;
		Asteriks* asteriks;
		GogiRect rect;
};

#endif	/* OIPF_ACTION_H */

