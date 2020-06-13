#include "constants.h"
struct specialist {
	enum type { TAIL, BODY, HEAD };
	int expert_count;
	int guild_table_count;
	int initial_skeleton_count;
	int lamport_clock_value;
	int team_ids[3];
	int mission_unassigned;
};