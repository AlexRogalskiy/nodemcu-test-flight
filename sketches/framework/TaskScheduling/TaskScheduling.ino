/* Task Scheduling in framework
 *
 * This example illustrate adding tasks in framework
 */

#include <EwingsEsp8266Stack.h>

int variable_time_task_id = 0;

// should run only one time at specified duration after EwStack initialization
void timeout_task(){
	Serial.print(millis());
	Serial.println(F(" : running timeout task once"));
}

// should run continueously at specific intervals after EwStack initialization
void interval_task(){
	Serial.print(millis());
	Serial.println(F(" : running interval task"));
}

// update interval task after some time to run it at rate 1000 milliseconds
void update_interval_task(){
	Serial.print(millis());
	Serial.println(F(" : ******* updating interval task @1000 milliseconds ************"));
	variable_time_task_id = __task_scheduler.updateInterval(
		variable_time_task_id,
		interval_task,
		MILLISECOND_DURATION_1000
	);
}

// clear interval task after specified duration
void clear_interval_task(){
	Serial.print(millis());
	Serial.println(F(" : ******* clearing interval task ********"));
	__task_scheduler.clearInterval( variable_time_task_id );
}


void setup() {

	// NOTE : Please disable framework serial log for this demo or framework log will get printed alongwith this demo log
	// Disable it by commenting ==> #define EW_SERIAL_LOG line in src/config/Common.h file of this framework library

	Serial.begin(115200);
	Serial.printf("Hold on!!!, Stack will initialize and begin within next %d seconds !\n", WIFI_STATION_CONNECT_ATTEMPT_TIMEOUT);

	EwStack.initialize();

	// run timeout task one time at 1000 milliseconds after EwStack initialization
	// dont worry about clearing timeout tasks, they will die automatically after their one time execution
	__task_scheduler.setTimeout( timeout_task, MILLISECOND_DURATION_1000 );

	// run interval task every 3000 milliseconds after EwStack initialization
	// shcedular provide unique id for each task to update task later in runtime if we want
	variable_time_task_id = __task_scheduler.setInterval( interval_task, MILLISECOND_DURATION_1000*3 );

	// adding timeout task which will update above interval task duration after 15000 milliseconds
	__task_scheduler.setTimeout( update_interval_task, MILLISECOND_DURATION_5000*3 );

	// adding timeout task which will clear above interval task duration after 30000 milliseconds
	__task_scheduler.setTimeout( clear_interval_task, MILLISECOND_DURATION_5000*6 );
}

void loop() {
	EwStack.serve();
}
