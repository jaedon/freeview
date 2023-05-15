/*
 * EventType.c
 */

#include "EventType.h"
#include "utils.h"

bool
EventType_isAsync(EventType t)
{
	switch(t)
	{
	case EventType_is_available:
		return false;

	case EventType_content_available:
		return true;

	case EventType_is_deleted:
		return false;

	case EventType_is_running:
		return false;

	case EventType_is_stopped:
		return false;

	case EventType_user_input:
		return true;

	case EventType_anchor_fired:
		return true;

	case EventType_timer_fired:
		return true;

	case EventType_asynch_stopped:
		return true;

	case EventType_interaction_completed:
		return true;

	case EventType_token_moved_from:
		return false;

	case EventType_token_moved_to:
		return false;

	case EventType_stream_event: /* CI+ MHEG Profile (CI+ spec) 에서는 Stream Actions and Stream Events are not required... 이다 */
		return true;

	case EventType_stream_playing: /* CI+ MHEG Profile (CI+ spec) 에서는 Stream Actions and Stream Events are not required... 이다 */
		return true;

	case EventType_stream_stopped: /* CI+ MHEG Profile (CI+ spec) 에서는 Stream Actions and Stream Events are not required... 이다 */
		return true;

	case EventType_counter_trigger: /* CI+ MHEG Profile (CI+ spec) 에서는 Stream Actions and Stream Events are not required... 이다 */
		return true;

	case EventType_highlight_on:
		return false;

	case EventType_highlight_off:
		return false;

	case EventType_cursor_enter: /* UK MHEG Profile (D-BOOK 5.0) / CI+ MHEG Profile (CI+ spec) 에서는 CursorShape 클래스가 사용되지 않음 */
		return true;

	case EventType_cursor_leave: /* UK MHEG Profile (D-BOOK 5.0) / CI+ MHEG Profile (CI+ spec) 에서는 CursorShape 클래스가 사용되지 않음 */
		return true;

	case EventType_is_selected: /* UK MHEG Profile (D-BOOK 5.0) / CI+ MHEG Profile (CI+ spec) 에서는 Button 클래스들, 즉, Hotspot, PushButton, SwitchButton 클래스들이 사용되지 않음 */
		return false;

	case EventType_is_deselected: /* UK MHEG Profile (D-BOOK 5.0) / CI+ MHEG Profile (CI+ spec) 에서는 Button 클래스들, 즉, Hotspot, PushButton, SwitchButton 클래스들이 사용되지 않음 */
		return false;

	case EventType_test_event:
		return false;

	case EventType_first_item_presented:
		return false;

	case EventType_last_item_presented:
		return false;

	case EventType_head_items:
		return false;

	case EventType_tail_items:
		return false;

	case EventType_item_selected:
		return false;

	case EventType_item_deselected:
		return false;

	case EventType_entry_field_full:
		return true;

	case EventType_engine_event:
		return true;

	case EventType_focus_moved: /* UK MHEG Profile (D-BOOK 5.0) 추가 사항 */
		return true;

	case EventType_slider_value_changed: /* UK MHEG Profile (D-BOOK 5.0) 추가 사항 */
		return true;

	default:
		error("Unknown EventType: %d", t);
		return false;
	}
}

char *
EventType_name(EventType t)
{
	switch(t)
	{
	case EventType_is_available:
		return "IsAvailable";

	case EventType_content_available:
		return "ContentAvailable";

	case EventType_is_deleted:
		return "IsDeleted";

	case EventType_is_running:
		return "IsRunning";

	case EventType_is_stopped:
		return "IsStopped";

	case EventType_user_input:
		return "UserInput";

	case EventType_anchor_fired:
		return "AnchorFired";

	case EventType_timer_fired:
		return "TimerFired";

	case EventType_asynch_stopped:
		return "AsynchStopped";

	case EventType_interaction_completed:
		return "InteractionCompleted";

	case EventType_token_moved_from:
		return "TokenMovedFrom";

	case EventType_token_moved_to:
		return "TokenMovedTo";

	case EventType_stream_event: /* CI+ MHEG Profile (CI+ spec) 에서는 Stream Actions and Stream Events are not required... 이다 */
		return "StreamEvent";

	case EventType_stream_playing: /* CI+ MHEG Profile (CI+ spec) 에서는 Stream Actions and Stream Events are not required... 이다 */
		return "StreamPlaying";

	case EventType_stream_stopped: /* CI+ MHEG Profile (CI+ spec) 에서는 Stream Actions and Stream Events are not required... 이다 */
		return "StreamStopped";

	case EventType_counter_trigger: /* CI+ MHEG Profile (CI+ spec) 에서는 Stream Actions and Stream Events are not required... 이다 */
		return "CounterTrigger";

	case EventType_highlight_on:
		return "HighlightOn";

	case EventType_highlight_off:
		return "HighlightOff";

	case EventType_cursor_enter: /* UK MHEG Profile (D-BOOK 5.0) / CI+ MHEG Profile (CI+ spec) 에서는 CursorShape 클래스가 사용되지 않음 */
		return "CursorEnter";

	case EventType_cursor_leave: /* UK MHEG Profile (D-BOOK 5.0) / CI+ MHEG Profile (CI+ spec) 에서는 CursorShape 클래스가 사용되지 않음 */
		return "CursorLeave";

	case EventType_is_selected: /* UK MHEG Profile (D-BOOK 5.0) / CI+ MHEG Profile (CI+ spec) 에서는 Button 클래스들, 즉, Hotspot, PushButton, SwitchButton 클래스들이 사용되지 않음 */
		return "IsSelected";

	case EventType_is_deselected: /* UK MHEG Profile (D-BOOK 5.0) / CI+ MHEG Profile (CI+ spec) 에서는 Button 클래스들, 즉, Hotspot, PushButton, SwitchButton 클래스들이 사용되지 않음 */
		return "IsDeselected";

	case EventType_test_event:
		return "TestEvent";

	case EventType_first_item_presented:
		return "FirstItemPresented";

	case EventType_last_item_presented:
		return "LastItemPresented";

	case EventType_head_items:
		return "HeadItems";

	case EventType_tail_items:
		return "TailItems";

	case EventType_item_selected:
		return "ItemSelected";

	case EventType_item_deselected:
		return "ItemDeselected";

	case EventType_entry_field_full:
		return "EntryFieldFull";

	case EventType_engine_event:
		return "EngineEvent";

	case EventType_focus_moved: /* UK MHEG Profile (D-BOOK 5.0) 추가 사항 */
		return "FocusMoved";

	case EventType_slider_value_changed: /* UK MHEG Profile (D-BOOK 5.0) 추가 사항 */
		return "SliderValueChanged";

	default:
		error("Unknown EventType: %d", t);
		return NULL;
	}
}

